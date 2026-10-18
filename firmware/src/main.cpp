// ============================================================
// main.cpp — FreeRTOS Initialization: Queue, Mutex, Tasks
// Project: Smart Railway Level Crossing, KMUTNB 010123119
// ============================================================

#include "Arduino.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "config.h"
#include "fsm_types.h"

// ---- Global RTOS Objects ----
QueueHandle_t     xEventQueue   = NULL;
SemaphoreHandle_t xTrackMutex   = NULL;
TimerHandle_t     xGateWatchdog = NULL;

// ---- Forward declarations ----
void vTaskFSMController(void *pvParameters);
void vTaskCrossingScanner(void *pvParameters);
void vTaskActuatorControl(void *pvParameters);
void vTaskTelemetryComm(void *pvParameters);
void vGateWatchdogCallback(TimerHandle_t xTimer);

// ---- Gate Watchdog Callback ----
void vGateWatchdogCallback(TimerHandle_t xTimer) {
    // Watchdog expired: gate did not close in GATE_LOWER_TIMEOUT_MS
    SystemEvent_t evt = EVT_GATE_TIMEOUT_FAULT;
    xQueueSendToFront(xEventQueue, &evt, 0); // High-priority: send to front
}

// ---- Arduino setup() ----
void setup() {
    Serial.begin(115200);
    Serial.println("[MAIN] Smart Railway Level Crossing booting...");

    // 1. Create event queue (FIFO, depth 20)
    xEventQueue = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(SystemEvent_t));
    configASSERT(xEventQueue != NULL);

    // 2. Create track mutex with Priority Inheritance
    xTrackMutex = xSemaphoreCreateMutex();
    configASSERT(xTrackMutex != NULL);

    // 3. Create gate watchdog timer (one-shot, 12 s)
    xGateWatchdog = xTimerCreate(
        "GateWDT",
        pdMS_TO_TICKS(GATE_LOWER_TIMEOUT_MS),
        pdFALSE,               // One-shot
        NULL,
        vGateWatchdogCallback
    );
    configASSERT(xGateWatchdog != NULL);

    // 4. Create FreeRTOS tasks
    // FSM Controller — Core 1 (Hard Real-Time)
    xTaskCreatePinnedToCore(
        vTaskFSMController, "FSM_Ctrl",
        TASK_STACK_FSM, NULL,
        PRIORITY_FSM_CONTROLLER, NULL, 1
    );

    // Crossing Scanner — Core 0 (Vision/Radar)
    xTaskCreatePinnedToCore(
        vTaskCrossingScanner, "Scanner",
        TASK_STACK_SCANNER, NULL,
        PRIORITY_SCANNER, NULL, 0
    );

    // Actuator Control — Core 1
    xTaskCreatePinnedToCore(
        vTaskActuatorControl, "Actuator",
        TASK_STACK_ACTUATOR, NULL,
        PRIORITY_ACTUATOR_CONTROL, NULL, 1
    );

    // Telemetry / LoRa — Core 0
    xTaskCreatePinnedToCore(
        vTaskTelemetryComm, "Telemetry",
        TASK_STACK_TELEMETRY, NULL,
        PRIORITY_TELEMETRY_COMM, NULL, 0
    );

    Serial.println("[MAIN] All tasks created. Scheduler starting.");
    // FreeRTOS scheduler starts automatically via Arduino-ESP32
}

void loop() {
    // Intentionally empty — all logic is in FreeRTOS tasks
    vTaskDelay(portMAX_DELAY);
}
