// camera_pins.h
// Freenove FNK0060 (ESP32-WROVER + OV2640)
// Pinout verificado contra esquemático FNK0060 v2.0
// NO incluir ningún header de Arduino ni de ESP-IDF aquí
#pragma once

// Power / Reset
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1

// XCLK
#define XCLK_GPIO_NUM  21

// SCCB / I2C
#define SIOD_GPIO_NUM  26   // SDA
#define SIOC_GPIO_NUM  27   // SCL

// Data bus (D0–D7 del OV2640, Y2–Y9 en nomenclatura Espressif)
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    19
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    5
#define Y2_GPIO_NUM    4

// Sync
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22