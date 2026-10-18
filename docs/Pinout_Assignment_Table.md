# Pinout Assignment Table — ESP32-S3
## Smart Railway Level Crossing & Shared Track System
### 010123119 Embedded System Design, KMUTNB

---

## 🔌 GPIO Mapping (ESP32-S3 DevKit-C / Custom PCB)

### Train Detection Sensors

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| 4 | SENSOR_EAST_WHEEL | INPUT | Inductive Proximity (Wheel Counter) | รถไฟเข้าจากทิศตะวันออก |
| 5 | SENSOR_EAST_LIMIT_UP | INPUT | Limit Switch | ยืนยันไม้กั้นขึ้นสุด (East) |
| 6 | SENSOR_EAST_LIMIT_DN | INPUT | Limit Switch | ยืนยันไม้กั้นลงสุด (East) |
| 7 | SENSOR_WEST_WHEEL | INPUT | Inductive Proximity (Wheel Counter) | รถไฟเข้าจากทิศตะวันตก |
| 15 | SENSOR_WEST_LIMIT_UP | INPUT | Limit Switch | ยืนยันไม้กั้นขึ้นสุด (West) |
| 16 | SENSOR_WEST_LIMIT_DN | INPUT | Limit Switch | ยืนยันไม้กั้นลงสุด (West) |

> **Note:** Wheel sensors use Opto-isolated GPIO interrupt (rising edge). Pull-up 10kΩ.

---

### Barrier Gate Motor Control

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| 10 | BARRIER_MOTOR_FWD | OUTPUT | H-Bridge PWM | สั่งให้ไม้กั้นลงมา (Lower) |
| 11 | BARRIER_MOTOR_REV | OUTPUT | H-Bridge PWM | สั่งให้ไม้กั้นขึ้น (Raise) |

> Motor: 24V DC Gear Motor + Electromagnetic Clutch. PWM frequency: 1 kHz.
> Fail-Safe: Clutch releases → gate falls by gravity when power cut.

---

### Road Traffic Signals

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| 12 | ROAD_LIGHT_RED | OUTPUT | High-Brightness LED Module 24V | ไฟแดงสำหรับยานพาหนะ |
| 13 | ROAD_LIGHT_YELLOW | OUTPUT | High-Brightness LED Module 24V | ไฟเหลืองเตือน |
| 14 | ROAD_LIGHT_GREEN | OUTPUT | High-Brightness LED Module 24V | ไฟเขียวอนุญาต |
| 19 | SIREN | OUTPUT | Industrial Siren 110 dB | ไซเรนเตือนภัย |

> Driven via MOSFET / SSR (Solid-State Relay) for galvanic isolation.

---

### Railway Signals

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| 17 | TRAIN_LIGHT_RED | OUTPUT | Rail Signal LED 24V | ห้ามผ่าน (All Directions) |
| 18 | TRAIN_LIGHT_GREEN | OUTPUT | Rail Signal LED 24V | อนุญาตให้รถไฟผ่าน |

---

### Obstacle Scanner (mmWave Radar + Camera)

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| 20 | RADAR_TX | OUTPUT (UART1 TX) | HLK-LD2410 mmWave Radar | ส่งคำสั่ง config ไปยังเรดาร์ |
| 21 | RADAR_RX | INPUT (UART1 RX) | HLK-LD2410 mmWave Radar | รับข้อมูลตรวจจับ |
| — | CAM_DATA (DVP) | INPUT | OV2640 / ESP32-CAM | AI Camera สแกนพื้นที่ทางข้าม |

> Radar: 24GHz, detection range 0–6m, scan period 50ms.
> Camera: Runs TinyML (YOLOv8-Tiny) on ESP32-S3 NPU for object classification.

---

### LoRa Emergency Communication

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| 8 | LORA_CS | OUTPUT (SPI CS) | SX1276/SX1278 LoRa Module | Chip Select |
| 9 | LORA_RST | OUTPUT | SX1276/SX1278 | Reset |
| — | SPI (MOSI/MISO/SCK) | SPI | SX1276/SX1278 | Shared SPI bus |

> Frequency: 433 MHz / 923 MHz (Sub-GHz). SF7, BW 125 kHz.
> Latency: < 50 ms P2P. Heartbeat: 500 ms.

---

### Power Monitoring

| GPIO | Signal Name | Direction | Component | Description |
|------|-------------|-----------|-----------|-------------|
| — | I2C (SDA/SCL) | I2C | INA219 Power Monitor | ตรวจแรงดันและกระแสไฟ 24V |

> Triggers EVT_POWER_FAILURE when V_bus < 20V (below 83% of 24V).

---

## 📐 Power Supply Summary

| Rail | Voltage | Source |
|------|---------|--------|
| MCU + Logic | 3.3V / 5V | Buck Converter from 24V |
| Sensors & Motor | 24V DC | Switching PSU 240W (10A) |
| Backup | 24V 30Ah LiFePO4 | Automatic Transfer Switch (0 ms) |
