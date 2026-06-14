#include "../includes/wifi.h"
#include "../../display/includes/tft.h"
#include <vector>
#include <Arduino.h>
#include <WiFi.h>

WiFiNetwork networks[MAX_NETWORKS];
int         networkCount = 0;
WiFiNetwork selectedNet;     // выбранная сеть
int selectedNetIndex;        // её индекс

void initWiFiScanner() {
    WiFi.mode(WIFI_STA);
    delay(100);
}

int scanNetworks() {
    WiFi.disconnect();
    delay(100);
    int found = WiFi.scanNetworks();

    networkCount = min(found, MAX_NETWORKS);

    for (int i = 0; i < networkCount; i++) {
        networks[i].ssid    = WiFi.SSID(i);
        networks[i].bssid   = WiFi.BSSIDstr(i);
        networks[i].rssi    = WiFi.RSSI(i);
        networks[i].channel = WiFi.channel(i);
        networks[i].isOpen  = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
    }

    // Сортировка по силе сигнала
    for (int i = 0; i < networkCount - 1; i++)
        for (int j = i + 1; j < networkCount; j++)
            if (networks[j].rssi > networks[i].rssi)
                std::swap(networks[i], networks[j]);

    WiFi.scanDelete();
    Serial.printf("[WiFi] Found: %d\n", networkCount);
    return networkCount;
}

WiFiNetwork getNetwork(int index) {
    return networks[index];
}

// -------- Меню --------
MenuItem wifiItems[MAX_NETWORKS + 2];
char     wifiLabels[MAX_NETWORKS][12];
char     wifiTags[MAX_NETWORKS][8];

void buildWiFiMenu() {
    int count = scanNetworks();
    wifiItems[0] = { "BACK", "[BCK]" };
    wifiItems[1] = { "SCAN", "[SCN]" };

    for (int i = 0; i < count; i++) {

        strncpy(wifiLabels[i], networks[i].ssid.c_str(), 10);
        wifiLabels[i][10] = '\0';

        int bars = map(networks[i].rssi, -90, -40, 1, 5);
        bars = constrain(bars, 1, 5);

        const char* sig = bars >= 4 ? "***" :
                        bars >= 2 ? "** " : "*  ";

        snprintf(wifiTags[i], sizeof(wifiTags[i]), "[%s]", sig);

        wifiItems[i + 2] = { wifiLabels[i], wifiTags[i] };
    }
}
