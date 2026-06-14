#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define C_BG        0x1129
#define C_TOPBAR    0x0C62
#define C_ACCENT    0xE8AC
#define C_TEXT      0xEF7D
#define C_MUTED     0x528A
#define C_DIVIDER   0x2965
#define C_CURSOR    0x07FF
#define C_CURSOR_SH 0x0410

#define C_TERM_BG   0x0000   // чёрный
#define C_TERM_SEL  0x0200   // тёмно-зелёный фон выбранного
#define C_TERM_LINE 0x0460   // разделители
#define C_TERM_DIM  0x02A0   // приглушённый текст
#define C_TERM_ON   0x07E0   // ярко-зелёный активный
#define C_TERM_NUM  0x0340   // номера строк

struct MenuItem {
    const char* label;
    const char* tag;
};

const uint8_t ICON_SD_BASE[8] = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00111100,
};

const uint8_t ICON_SD_X[8] = {
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b10000001,
};

const uint8_t ICON_WIFI_ON[8] = {
    0b00000000,
    0b00111100,
    0b01100110,
    0b00011000,
    0b00100100,
    0b00000000,
    0b00011000,
    0b00011000,
};

const uint8_t ICON_WIFI_BASE[8] = {
    0b00000000,
    0b00111100,
    0b01100110,
    0b00011000,
    0b00100100,
    0b00000000,
    0b00011000,
    0b00011000,
};

const uint8_t ICON_WIFI_X[8] = {
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b10000001,
};

const uint8_t ICON_NRF_ON[8] = {
    0b00010000,
    0b00111000,
    0b01010100,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00111000,
    0b01111100,
};

const uint8_t ICON_NRF_BASE[8] = {
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00111000,
    0b01111100,
};

const uint8_t ICON_NRF_X[8] = {
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b10000001,
};

extern bool connect;
extern bool wifiMonitor;
extern volatile bool wifiActuallyConnected;


void initTFTMini();
void printTextTFT(int x, int y, String text);
int drawMenuGeneric(const char* title, MenuItem* items, int count, bool reset = false);
void printDelayTFT();
void printAttackTFT();
void tftClear();
void updateWifiStatus();
void updateSDStatus();
void updateNRFStatus();
void deleteSD();
void showTextFile(const char* path);
void showBootScreen();
void showTerminal();
String inputPassword(const String& title = "Enter password");
void showAIChat();
void drawTetrisMatrix(int x, int y, int w, int h, uint16_t color);
void drawTetrisFigure(int x, int y, int w, int h, uint16_t color);