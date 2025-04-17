# Smart Temperature & Humidity Monitoring System with BLE & LCD

**Group Project | Course: Computer Interface and Data Acquisition | Sep 2024 - Dec 2024**

This project implements an embedded system using a Silicon Labs EFR32 Blue Gecko board to monitor environmental conditions (temperature and humidity), display the data on an LCD, and broadcast it via Bluetooth Low Energy (BLE).

## Features

*   **Sensor Integration:** Interfaces with a DHT11 sensor via GPIO to acquire real-time temperature and humidity readings.
*   **BLE Communication:** Implements custom BLE advertising packets to broadcast the collected sensor data wirelessly. Remote devices can scan for these packets to receive the environmental information.
*   **LCD Display:** Utilizes the Silicon Labs Graphics Library (GLIB) and Display Management Driver (DMD) to show current temperature, humidity, and system status (sensor sampling interval, BLE advertising interval) on a connected Sharp Memory LCD.
*   **UART Configuration:** Enables users to configure the sensor sampling interval and the BLE advertising interval via a UART serial connection.
*   **Hardware Platform:** Developed for the Silicon Labs EFR32 Blue Gecko series (specifically tested on EFR32BG13).
*   **Development Environment:** Built using Simplicity Studio IDE and programmed in C.

## Hardware Requirements

*   Silicon Labs EFR32 Blue Gecko Wireless Starter Kit (or a custom board with an EFR32BG13)
*   DHT11 Temperature and Humidity Sensor
*   Sharp Memory LCD (compatible with the GLIB/DMD driver, e.g., LS013B7DH03)
*   Connecting wires

## Software Requirements

*   [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio) v5 or later
*   [Gecko SDK Suite](https://www.silabs.com/developers/gecko-software-development-kit)

## Getting Started

1.  **Clone the Repository:**
    ```bash
    # If using Git
    git clone <repository-url>
    ```
2.  **Import Project:** Open Simplicity Studio and import the project (`.slcp` file).
3.  **Build Project:** Build the project within Simplicity Studio. Ensure the correct board/part is selected.
4.  **Flash Bootloader (if needed):** As this is an SoC project, it likely requires a bootloader for OTA DFU capabilities or proper execution.
    *   Flash a suitable bootloader (e.g., *Bluetooth Apploader OTA DFU*) for your EFR32 device first. You can create one using Simplicity Studio or flash a pre-compiled demo that includes a bootloader (like *Bluetooth - SoC Thermometer*).
    *   **Important:** When flashing the application *after* the bootloader, use the `.hex` or `.s37` file, not `.bin`, to avoid overwriting the bootloader. Refer to [UG103.6: Bootloader Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-06-fundamentals-bootloading.pdf) and [UG489: Silicon Labs Gecko Bootloader User's Guide](https://cn.silabs.com/documents/public/user-guides/ug489-gecko-bootloader-user-guide-gsdk-4.pdf) for more details.
5.  **Flash Application:** Flash the built application (`.hex` or `.s37` file) to the EFR32 board using Simplicity Studio's debugger/programmer.
6.  **Connect Hardware:** Connect the DHT11 sensor and the Sharp Memory LCD to the appropriate GPIO pins as defined in `config/pin_config.h` and the respective driver files (`DHT.c`, `dmd_memlcd.c`, etc.). Connect a USB cable for power and UART communication.
7.  **Run:** Reset the board. The system should start, read sensor data, display it on the LCD, and begin BLE advertising.

## Usage

*   **BLE Advertising:** Use a BLE scanner app (like EFR Connect) on a smartphone or another BLE-capable device to scan for the device. The advertised data will contain the temperature and humidity readings.
*   **LCD Display:** The Sharp Memory LCD will show:
    *   Group Name ("Nhom DDCH")
    *   Current Temperature (e.g., "Nhiet do: 25 C")
    *   Current Humidity (e.g., "Do am: 60 %")
    *   Sensor Sampling Interval (e.g., "Chu Ki Sensor: 1 s")
*   **UART Configuration:**
    1.  Connect to the board's virtual COM port using a terminal emulator (e.g., Tera Term, PuTTY) with settings 115200 baud, 8N1.
    2.  Press 'i' to initiate configuration.
    3.  Follow the prompts:
        *   Enter '1' to set the DHT11 sampling interval (in seconds, 01-99).
        *   Enter '2' to set the BLE advertising interval (in seconds, 01-99).
    4.  Enter the desired two-digit interval value. The system will update its timing accordingly.

## Code Structure Overview

*   `main.c`: Entry point, system initialization, main loop handling UART input.
*   `app.c`/`app.h`: Core application logic, Bluetooth event handling (`sl_bt_on_event`), timer callbacks, LCD updates.
*   `DHT.c`/`DHT.h`: Driver for interfacing with the DHT11 sensor.
*   `custom_adv.c`/`custom_adv.h`: Functions for creating and updating custom BLE advertisement packets.
*   `app_lcd.c`/`app_lcd.h`: Application-specific LCD functions using GLIB.
*   `glib/`: Silicon Labs Graphics Library files (drawing primitives, fonts).
*   `dmd*.c`/`dmd.h`: Display Management Driver for the memory LCD.
*   `lcd*.c`/`lcd*.h`: Low-level LCD interface drivers.
*   `config/`: Project configuration files, including pin mappings (`pin_config.h`).
*   `autogen/`: Files automatically generated by Simplicity Studio.

## Troubleshooting

*   **No Display/Incorrect Data:** Check wiring for the LCD and DHT11 sensor. Verify pin configurations in `config/pin_config.h` match the hardware connections.
*   **Not Advertising/Connecting:** Ensure a bootloader is flashed correctly. Verify BLE settings and intervals. Use EFR Connect or another BLE tool to debug advertising packets.
*   **Build Errors:** Ensure the correct Gecko SDK version is installed and selected in Simplicity Studio. Check for missing components.

## Resources

*   [Silicon Labs Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)
*   [UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)
*   [GLIB Documentation (Example for EFR32BG13)](https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg13/html/dir_4e5d6edff79a205450b6fcf6e037f6d7.html)

## Report Bugs & Get Support

Note: Use UART or Log, don't use both at the same time