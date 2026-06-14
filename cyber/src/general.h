#pragma once

// TFT
#define TFT_CS    10
#define TFT_RST   9
#define TFT_DC    8

// Shared SPI
#define SPI_MOSI  11
#define SPI_SCK   12
#define SPI_MISO  13

// SD
#define SD_CS     5

// joystick
#define JOY_X 41
#define JOY_B 18
#define JOY_Y 2
#define JOY_BUTTON 3


// CLOCK
#define DAT_CLK 7
#define RST_CLK 6
#define CLK_CLK 4


#define USB_DM 19
#define USB_DP 20


// NRF24L01 #1 — shared SPI (MOSI=11, SCK=12, MISO=13)
#define NRF1_CE     14
#define NRF1_CSN    15

// NRF24L01 #2 — та же шина, другие CE/CSN
#define NRF2_CE     16
#define NRF2_CSN    17

// ИК диоды
#define IR_1        21

// DHT11
#define DHT_PIN     42
#define DHT_TYPE DHT11

#define MAX_NETWORKS 25

#define HIGHT_W 128
#define WEIGHT_W 128