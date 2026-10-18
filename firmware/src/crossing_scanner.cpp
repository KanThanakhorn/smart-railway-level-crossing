// ============================================================
// crossing_scanner.cpp — Obstacle Scanner Task (Sensor Fusion)
// mmWave Radar (HLK-LD2410) + AI Camera (ESP32-CAM / TinyML)
// Scan rate: 20 Hz (every 50 ms) | Logic: OR Fusion
// Project: Smart Railway Level Crossing, KMUTNB 010123119
// ============================================================

#include "Arduino.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "config.h"
#include "fsm_types.h"

// ---- Sensor reading stubs ----
// Replace with actual UART/SPI driver implementations
static bool ReadRadarObstacle(void) {
    // TODO: Parse HLK-LD2410 UART frame
    // Returns true if target detected in crossing zone
    return false;
}

static bool ReadCameraObstacle(void) {
    // TODO: Run TinyML / YOLOv8-Tiny inference on ESP32-S3 NPU
    // Returns true if object class matches vehicle/person
    return false;
}

// ---- Scanner Task ----
void vTaskCrossingScanner(void *pvParameters) {
    static bool       scanEnabled      = false;
    static uint8_t    consecutiveClear = 0;  // Debounce: need 3x clear
    static uint8_t    consecutiveObs   = 0;  // Debounce: need 3x obstacle
    static bool       lastObsState     = false;
    SystemEvent_t     evt;
    TickType_t        xLastWakeTime    = xTaskGetTickCount();

    for (;;) {
        // Strict 50 ms periodic execution (20 Hz)
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SCAN_PERIOD_MS));

        if (!scanEnabled) {
            consecutiveClear = 0;
            consecutiveObs   = 0;
            continue;
        }

        // ---- Sensor Fusion: OR Logic ----
        bool radarDetect  = ReadRadarObstacle();
        bool cameraDetect = ReadCameraObstacle();
        bool obstacleNow  = radarDetect || cameraDetect;

        // ---- Debounce: 3 consecutive same readings = confirmed ----
        if (obstacleNow) {
            consecutiveClear = 0;
            consecutiveObs++;
        } else {
            consecutiveObs   = 0;
            consecutiveClear++;
        }

        // Send event only on state transition (avoid queue flooding)
        if (consecutiveObs >= 3 && !lastObsState) {
            evt = EVT_OBSTACLE_DETECTED;
            xQueueSend(xEventQueue, &evt, 0);
            lastObsState = true;
        } else if (consecutiveClear >= 3 && lastObsState) {
            evt = EVT_AREA_CLEAR;
            xQueueSend(xEventQueue, &evt, 0);
            lastObsState = false;
        }
    }
}

// ---- Enable/Disable scan (called from FSM) ----
void TriggerCrossingAreaScan(bool enable) {
    // Simple flag — scanner task reads this each cycle
    // In production: use task notification (xTaskNotify) for < 1 ms response
    extern volatile bool gScanEnabled;
    gScanEnabled = enable;
}
