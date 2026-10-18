#pragma once
// ============================================================
// config.h — System-wide constants for Smart Railway Level Crossing
// Project: 010123119 Embedded System Design, KMUTNB
// ============================================================

// ---- Timing Constraints (Real-Time Requirements) ----
#define ROAD_YELLOW_TIME_MS       3000   // Yellow light warning duration (3.0 s)
#define ALL_RED_DELAY_MS          2000   // All-red clearance delay (2.0 s)
#define GATE_LOWER_TIMEOUT_MS    12000   // Watchdog: max time for gate to close (12.0 s)
#define GATE_RAISE_TIME_MS        7000   // Expected barrier raise duration (5–8 s)
#define SCAN_PERIOD_MS              50   // Obstacle scan interval = 50 ms (20 Hz)
#define EMERGENCY_LATENCY_MS       100   // Max emergency response latency

// ---- Safety Distances ----
#define D_SENSOR_DISTANCE_M        460   // Min sensor placement distance from crossing (m)
#define D_MARGIN_M                  50   // Safety margin distance (m)

// ---- Train Speed ----
#define TRAIN_MAX_SPEED_KMH         60   // Maximum train speed (km/h)
#define TRAIN_MAX_SPEED_MS       16.67f  // 60 km/h in m/s

// ---- FreeRTOS Task Priorities ----
#define PRIORITY_FSM_CONTROLLER      3   // Highest: FSM core logic
#define PRIORITY_SCANNER             4   // Highest+: time-critical scan
#define PRIORITY_ACTUATOR_CONTROL    2   // Medium: barrier/light control
#define PRIORITY_TELEMETRY_COMM      1   // Lowest: LoRa/logging

// ---- FreeRTOS Queue & Stack ----
#define EVENT_QUEUE_LENGTH          20
#define TASK_STACK_FSM            4096
#define TASK_STACK_SCANNER        4096
#define TASK_STACK_ACTUATOR       2048
#define TASK_STACK_TELEMETRY      2048

// ---- GPIO Pin Definitions (ESP32-S3) ----
// See docs/Pinout_Assignment_Table.md for full mapping
#define PIN_SENSOR_EAST_WHEEL      4
#define PIN_SENSOR_EAST_LIMIT_UP   5
#define PIN_SENSOR_EAST_LIMIT_DN   6
#define PIN_SENSOR_WEST_WHEEL      7
#define PIN_SENSOR_WEST_LIMIT_UP   15
#define PIN_SENSOR_WEST_LIMIT_DN   16
#define PIN_BARRIER_MOTOR_FWD      10
#define PIN_BARRIER_MOTOR_REV      11
#define PIN_ROAD_LIGHT_RED         12
#define PIN_ROAD_LIGHT_YELLOW      13
#define PIN_ROAD_LIGHT_GREEN       14
#define PIN_TRAIN_LIGHT_RED        17
#define PIN_TRAIN_LIGHT_GREEN      18
#define PIN_SIREN                  19
#define PIN_LORA_CS                 8
#define PIN_LORA_RST                9
#define PIN_RADAR_UART_TX          20
#define PIN_RADAR_UART_RX          21
