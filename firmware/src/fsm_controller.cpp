// ============================================================
// fsm_controller.cpp — 9-State FSM Task (vTaskFSMController)
// Project: Smart Railway Level Crossing, KMUTNB 010123119
// ============================================================

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "config.h"
#include "fsm_types.h"

// ---- State & track ownership ----
static volatile SystemState_t    currentState      = S0_IDLE;
static volatile TrackDirection_t activeTrackOwner  = TRACK_FREE;

// ---- Actuator stubs (implemented in actuator_control.cpp) ----
extern void SetRoadTrafficLight(uint8_t r, uint8_t y, uint8_t g);
extern void SetTrainSignal(TrackDirection_t dir, uint8_t green);
extern void ControlBarrierMotor(bool lower);
extern void SetBuzzerAlarm(bool on);
extern void SwitchToBackupBattery(void);
extern void SendEmergencyBrakeToTrainCab(TrackDirection_t dir);
extern void SendAlertToControlCenter(const char *msg);
extern void TriggerCrossingAreaScan(bool enable);

// ---- Main FSM Task ----
void vTaskFSMController(void *pvParameters) {
    SystemEvent_t receivedEvent;

    for (;;) {
        // Block indefinitely on event queue
        if (xQueueReceive(xEventQueue, &receivedEvent, portMAX_DELAY) != pdPASS)
            continue;

        // --- Global override: Power Failure (highest priority) ---
        if (receivedEvent == EVT_POWER_FAILURE) {
            currentState = S8_SAFE_STATE;
            SwitchToBackupBattery();
            ControlBarrierMotor(true);          // Lower all gates (gravity fail-safe)
            SetTrainSignal(TRAIN_FROM_EAST, 0);
            SetTrainSignal(TRAIN_FROM_WEST, 0);
            SetRoadTrafficLight(1, 0, 0);
            SetBuzzerAlarm(true);
            SendAlertToControlCenter("POWER_FAILURE");
            continue;
        }

        switch (currentState) {

        // --------------------------------------------------------
        case S0_IDLE:
            if (receivedEvent == EVT_TRAIN_APPROACH_EAST ||
                receivedEvent == EVT_TRAIN_APPROACH_WEST) {

                // Try to acquire shared single-track mutex (non-blocking)
                if (xSemaphoreTake(xTrackMutex, 0) == pdTRUE) {
                    activeTrackOwner = (receivedEvent == EVT_TRAIN_APPROACH_EAST)
                                       ? TRAIN_FROM_EAST : TRAIN_FROM_WEST;
                    currentState = S1_WARN_ROAD;
                    SetRoadTrafficLight(0, 1, 0);   // YELLOW
                    SetBuzzerAlarm(true);
                    // Yellow phase timer -> EVT_ROAD_LIGHT_RED_DONE via actuator task
                } else {
                    // Track busy: second train must wait — signal STOP
                    SetTrainSignal(activeTrackOwner == TRAIN_FROM_EAST
                                   ? TRAIN_FROM_WEST : TRAIN_FROM_EAST, 0);
                }
            }
            break;

        // --------------------------------------------------------
        case S1_WARN_ROAD:
            if (receivedEvent == EVT_ROAD_LIGHT_RED_DONE) {
                SetRoadTrafficLight(1, 0, 0);       // Full RED
                currentState = S2_CLOSING_GATE;
                ControlBarrierMotor(true);           // Start lowering
                xTimerStart(xGateWatchdog, 0);       // Arm 12 s watchdog
            }
            break;

        // --------------------------------------------------------
        case S2_CLOSING_GATE:
            if (receivedEvent == EVT_GATE_CLOSED_OK) {
                xTimerStop(xGateWatchdog, 0);
                currentState = S3_SCAN_AREA;
                TriggerCrossingAreaScan(true);       // Start radar+camera
            } else if (receivedEvent == EVT_GATE_TIMEOUT_FAULT) {
                ControlBarrierMotor(false);          // Stop motor
                currentState = S6_FAULT_GATE;
                SetTrainSignal(activeTrackOwner, 0); // Keep train RED
                SendAlertToControlCenter("GATE_FAULT");
            }
            break;

        // --------------------------------------------------------
        case S3_SCAN_AREA:
            if (receivedEvent == EVT_AREA_CLEAR) {
                currentState = S4_TRAIN_PASSING;
                SetTrainSignal(activeTrackOwner, 1); // Train GREEN
            } else if (receivedEvent == EVT_OBSTACLE_DETECTED) {
                currentState = S7_EMERGENCY_OBS;
                SetTrainSignal(activeTrackOwner, 0); // Train RED
                SendEmergencyBrakeToTrainCab(activeTrackOwner);
                SendAlertToControlCenter("OBSTACLE_ON_CROSSING");
            }
            break;

        // --------------------------------------------------------
        case S4_TRAIN_PASSING:
            if (receivedEvent == EVT_OBSTACLE_DETECTED) {
                currentState = S7_EMERGENCY_OBS;
                SetTrainSignal(activeTrackOwner, 0);
                SendEmergencyBrakeToTrainCab(activeTrackOwner);
            } else if (receivedEvent == EVT_TRAIN_CLEARED_CROSSING) {
                // Axle counter confirmed: train fully exited block
                SetTrainSignal(activeTrackOwner, 0);
                currentState = S5_OPENING_GATE;
                TriggerCrossingAreaScan(false);
                ControlBarrierMotor(false);          // Raise gate
            }
            break;

        // --------------------------------------------------------
        case S5_OPENING_GATE:
            if (receivedEvent == EVT_GATE_OPENED_OK) {
                ControlBarrierMotor(false);
                SetBuzzerAlarm(false);
                SetRoadTrafficLight(0, 0, 1);       // Road GREEN
                // Release shared track
                activeTrackOwner = TRACK_FREE;
                xSemaphoreGive(xTrackMutex);
                currentState = S0_IDLE;
            }
            break;

        // --------------------------------------------------------
        case S6_FAULT_GATE:
        case S8_SAFE_STATE:
            if (receivedEvent == EVT_POWER_RESTORED ||
                receivedEvent == EVT_MANUAL_RESET) {
                // Operator has verified safety and reset system
                currentState = S3_SCAN_AREA;
                TriggerCrossingAreaScan(true);
            }
            break;

        // --------------------------------------------------------
        case S7_EMERGENCY_OBS:
            if (receivedEvent == EVT_AREA_CLEAR) {
                // Obstacle removed; re-verify before resuming
                currentState = S3_SCAN_AREA;
                TriggerCrossingAreaScan(true);
            }
            break;

        default:
            break;
        }
    }
}
