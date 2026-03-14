# Secure Firmware Bootloader — STM32

A secure bootloader demonstration for STM32 microcontrollers that validates firmware integrity before launching the application. Built with the STM32 HAL library and STM32CubeIDE, it provides a foundation for implementing secure firmware loading on embedded systems.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Boot Sequence](#boot-sequence)
- [LED Status Indicators](#led-status-indicators)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
  - [Flashing the Firmware](#flashing-the-firmware)
- [Configuration](#configuration)
- [Roadmap](#roadmap)
- [License](#license)

---

## Overview

This project demonstrates a multi-stage boot process on the **STM32F401RETx** microcontroller. On startup, the bootloader computes a checksum of the application firmware and compares it against a stored reference value. If the firmware is valid, the bootloader transfers control to the application. If the firmware is invalid or corrupted, it halts and signals an error via an LED.

---

## Features

- ✅ Firmware integrity verification using checksum validation
- ✅ Multi-stage boot: initialization → validation → application
- ✅ Visual status feedback via GPIO-connected LEDs
- ✅ Graceful error handling — invalid firmware never executes
- ✅ STM32 HAL-based hardware abstraction for portability
- ⚙️ DHT11 temperature/humidity sensor driver (skeleton — in progress)

---

## Hardware Requirements

| Component | Details |
|-----------|---------|
| Microcontroller | STM32F401RETx (ARM Cortex-M4) |
| Package | LQFP64 |
| Flash | 256 KB |
| RAM | 96 KB |
| Clock Source | HSI internal oscillator (16 MHz) |
| Status LEDs | Connected to PA5 and PA6 |
| IDE | STM32CubeIDE |
| Firmware Package | STM32Cube FW_F4 V1.28.3 |

> **Note:** The project targets the NUCLEO-F401RE development board but can be adapted to any STM32F4 board with minimal changes.

---

## Project Structure

```
secure-firmware-bootloader-STM32/
├── main.c                          # System entry point and peripheral initialization
├── bootloader.c                    # Bootloader logic and checksum verification
├── application.c                   # Application code executed after successful boot
└── secure_bootloader_demo.ioc      # STM32CubeIDE project configuration
```

### File Descriptions

| File | Description |
|------|-------------|
| `main.c` | Initializes the HAL, configures the system clock (16 MHz HSI) and GPIO pins (PA5, PA6), then calls `Bootloader_Run()` |
| `bootloader.c` | Computes a firmware checksum and compares it against `STORED_CHECKSUM`. Launches the application on success or blinks an error pattern on failure |
| `application.c` | Contains `Application_Start()` (main app loop with 1 Hz LED blink) and a skeleton `DHT11_Read()` sensor driver |
| `secure_bootloader_demo.ioc` | STM32CubeIDE configuration file describing MCU, clocks, GPIO, memory layout, and toolchain settings |

---

## How It Works

### Checksum Verification (`bootloader.c`)

```c
#define STORED_CHECKSUM 455   // ⚠️ Known issue: should be 45 (see note below)

uint32_t Calculate_Checksum(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += i;
    }
    return sum;   // returns 45 (0+1+2+…+9)
}
```

The bootloader calls `Calculate_Checksum()` and compares the result against `STORED_CHECKSUM`. If they match, the application is launched; otherwise an error loop is entered.

> ⚠️ **Known issue:** `Calculate_Checksum()` returns **45**, but `STORED_CHECKSUM` is defined as **455**. Because 45 ≠ 455 the validation will always fail with the current code, causing the bootloader to enter the error-blink loop indefinitely and never reaching `Application_Start()`. To fix this, change `STORED_CHECKSUM` to `45` in `bootloader.c`.

> **Current implementation** uses a simple arithmetic checksum for demonstration purposes. In a production system this should be replaced with a cryptographic hash (e.g. SHA-256) or digital signature verification over the actual firmware image in Flash memory.

---

## Boot Sequence

```
Power On / Reset
      │
      ▼
 HAL_Init()
 SystemClock_Config()   ← 16 MHz HSI
 MX_GPIO_Init()         ← PA5, PA6 as outputs
      │
      ▼
 Bootloader_Run()
      │
      ├─ Calculate_Checksum()
      │
      ├─[checksum == STORED_CHECKSUM]──► Toggle PA6 × 2 (500 ms each)
      │                                           │
      │                                           ▼
      │                                   Application_Start()
      │                                     └─ Toggle PA6 every 1000 ms
      │
      └─[checksum != STORED_CHECKSUM]──► Toggle PA6 every 200 ms (error loop)
```

---

## LED Status Indicators

| GPIO Pin | Pattern | Meaning |
|----------|---------|---------|
| PA6 | 2 × toggles (500 ms each) | Bootloader validation **passed** |
| PA6 | Rapid blink (200 ms period) | Bootloader validation **failed** — firmware invalid |
| PA6 | Slow blink (1000 ms period) | Application running normally |

---

## Getting Started

### Prerequisites

- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (v1.x or later)
- STM32Cube FW_F4 firmware package (V1.28.3 or compatible)
- ST-LINK/V2 programmer (included on NUCLEO boards)
- STM32F401RETx-based board (e.g., NUCLEO-F401RE)

### Building the Project

1. Clone this repository:
   ```bash
   git clone https://github.com/Bhavin-umatiya/secure-firmware-bootloader-STM32.git
   ```

2. Open **STM32CubeIDE** and select **File → Import → Existing Projects into Workspace**.

3. Browse to the cloned directory and import the project.

4. Build the project using **Project → Build Project** (or press `Ctrl+B`).

### Flashing the Firmware

1. Connect your STM32 board to your PC via USB (ST-LINK interface).

2. In STM32CubeIDE, select **Run → Run** (or press `F11`) to flash and start the debugger.

3. Alternatively, use the ST-LINK Utility or `openocd` to flash the generated `.elf`/`.bin` file:
   ```bash
   openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
           -c "program build/secure_bootloader_demo.elf verify reset exit"
   ```

---

## Configuration

Key parameters in `bootloader.c` that can be modified:

| Parameter | Location | Default | Description |
|-----------|----------|---------|-------------|
| `STORED_CHECKSUM` | `bootloader.c` | `455` | Expected firmware checksum value |
| Blink count (success) | `Bootloader_Run()` | `2` | Number of LED toggles on valid firmware |
| Blink delay (success) | `Bootloader_Run()` | `500 ms` | LED toggle period on successful validation |
| Blink delay (error) | `Bootloader_Run()` | `200 ms` | LED toggle period on failed validation |

Clock and GPIO configuration is managed via the `.ioc` file in STM32CubeIDE's graphical configurator.

---

## Roadmap

- [ ] Replace arithmetic checksum with SHA-256 hash over Flash firmware image
- [ ] Add RSA/ECDSA digital signature verification
- [ ] Implement Flash write protection and readout protection (RDP)
- [ ] Add rollback protection using firmware version tracking
- [ ] Support firmware update over UART/USB DFU
- [ ] Complete DHT11 temperature/humidity sensor integration
- [ ] Add UART logging for boot status messages

---

## License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for the full text.

You are free to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of this software, provided the original copyright notice and this permission notice are included in all copies or substantial portions of the software.

> STM32 HAL library code in `main.c` is Copyright © 2026 STMicroelectronics, licensed under ST's software license terms.
