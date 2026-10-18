# Smart Railway Level Crossing & Shared Track System
### 010123119 Embedded System Design — KMUTNB

ระบบทางข้ามรถไฟอัจฉริยะสำหรับรางเดี่ยวร่วมกัน (Shared Single Track)  
ควบคุมด้วย **9-State FSM**, **FreeRTOS** บน **ESP32-S3**, รองรับ Gravity Fail-Safe

---

## 👥 Project Team

| Student ID | Name | Role |
|---|---|---|
| 6701012610040 | นายธนกร ตะบอง | Project Leader / System Architect |
| 6701012610121 | นายฐิติ ศรีประจง | Hardware Specialist |
| 6701012610279 | นายภานุกร สุวรรณศรี | Software Lead |
| 6701012610287 | นายชนันธร วงศ์สุคนธนิตย์ | Verification Engineer |

---

## ✨ Key Features

| Feature | Detail |
|---|---|
| FSM States | **9 States** (S0_IDLE → S8_SAFE_STATE) |
| RTOS | FreeRTOS on ESP32-S3 Dual-Core |
| Fail-Safe | Gravity Fail-Safe + Backup LiFePO4 24V 30Ah |
| Sensor Distance | **D_sensor = 460 m** @ 60 km/h |
| Obstacle Detection | mmWave Radar + AI Camera (Sensor Fusion) |
| Track Arbitration | Mutex Semaphore + FCFS Queue (Deadlock-Free) |
| Emergency Alert | LoRa P2P Sub-GHz (< 50 ms latency) |
| Standards | EN 50126, EN 50128, IEC 61508 |

---

## 📁 Repository Structure

```
Code/
├── firmware/
│   ├── include/
│   │   ├── config.h          # System timing & distance constants
│   │   └── fsm_types.h       # FSM enums & event definitions
│   └── src/
│       ├── main.cpp           # FreeRTOS init, Queue, Mutex, Tasks
│       ├── fsm_controller.cpp # 9-State FSM logic
│       └── crossing_scanner.cpp # Sensor Fusion scanner task
├── simulation/
│   └── braking_distance_sim.py # D_sensor calculation script
├── docs/
│   ├── Pinout_Assignment_Table.md
│   └── README_DOCS.md
└── README.md
```

---

## 🚀 Quick Start (Firmware)

```bash
# PlatformIO
pio run -e esp32s3
pio upload
pio device monitor --baud 115200
```

## 📄 Report
See `docs/README_DOCS.md` for link to Final Design Document.
