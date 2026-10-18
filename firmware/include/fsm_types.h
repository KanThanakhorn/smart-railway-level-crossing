#pragma once
// ============================================================
// fsm_types.h — FSM State, Event, and Data Type Definitions
// Project: 010123119 Embedded System Design, KMUTNB
// ============================================================

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

// ---- 9-State FSM ----
typedef enum {
    S0_IDLE          = 0,  // Normal operation: road=GREEN, train=RED, gate=UP
    S1_WARN_ROAD     = 1,  // Train detected: road YELLOW->RED, buzzer ON
    S2_CLOSING_GATE  = 2,  // Gate lowering; watchdog timer active (12 s max)
    S3_SCAN_AREA     = 3,  // Gate closed; radar+camera scanning crossing
    S4_TRAIN_PASSING = 4,  // Track clear, train authorized: train=GREEN
    S5_OPENING_GATE  = 5,  // Train cleared block; gate raising
    S6_FAULT_GATE    = 6,  // Gate timeout/fault: train=RED, alert dispatch
    S7_EMERGENCY_OBS = 7,  // Obstacle on track: train=RED, emergency brake alert
    S8_SAFE_STATE    = 8,  // Power failure: backup battery, all gates closed
} SystemState_t;

// ---- Shared Single Track Ownership ----
typedef enum {
    TRACK_FREE       = 0,
    TRAIN_FROM_EAST  = 1,
    TRAIN_FROM_WEST  = 2,
} TrackDirection_t;

// ---- System Events (sent to xEventQueue) ----
typedef enum {
    // Train detection events
    EVT_TRAIN_APPROACH_EAST     = 0,  // Wheel counter triggered, east side
    EVT_TRAIN_APPROACH_WEST     = 1,  // Wheel counter triggered, west side
    EVT_TRAIN_CLEARED_CROSSING  = 2,  // Axle counter: train fully exited crossing

    // Road signal events
    EVT_ROAD_LIGHT_RED_DONE     = 3,  // Road lights completed RED transition

    // Gate mechanism events
    EVT_GATE_CLOSED_OK          = 4,  // Limit switch: gate fully lowered
    EVT_GATE_OPENED_OK          = 5,  // Limit switch: gate fully raised
    EVT_GATE_TIMEOUT_FAULT      = 6,  // Watchdog expired before gate closed

    // Obstacle scanning events
    EVT_AREA_CLEAR              = 7,  // Scanner: no obstacle detected (3 consecutive)
    EVT_OBSTACLE_DETECTED       = 8,  // Scanner: obstacle on crossing confirmed

    // Power events
    EVT_POWER_FAILURE           = 9,  // Main PSU voltage drop detected
    EVT_POWER_RESTORED          = 10, // Main PSU voltage restored

    // Manual / maintenance
    EVT_MANUAL_RESET            = 11, // Operator manual reset after fault
} SystemEvent_t;

// ---- RTOS Object Handles (declared extern, defined in main.cpp) ----
extern QueueHandle_t     xEventQueue;     // Main FSM event queue (FIFO)
extern SemaphoreHandle_t xTrackMutex;     // Shared single-track mutex
extern TimerHandle_t     xGateWatchdog;   // Software watchdog for gate mechanism

// ---- Inline Helper ----
static inline const char* StateToStr(SystemState_t s) {
    static const char* names[] = {
        "S0_IDLE","S1_WARN_ROAD","S2_CLOSING_GATE","S3_SCAN_AREA",
        "S4_TRAIN_PASSING","S5_OPENING_GATE","S6_FAULT_GATE",
        "S7_EMERGENCY_OBS","S8_SAFE_STATE"
    };
    return (s <= S8_SAFE_STATE) ? names[s] : "UNKNOWN";
}
