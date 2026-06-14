#include "../includes/wifi.h"
#include <Arduino.h>
#include <WiFi.h>

String wifiPassword;
volatile bool wifiActuallyConnected = false;


void onWifiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            wifiActuallyConnected = true;
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            wifiActuallyConnected = false;
            break;
        default:
            break;
    }
}

void initWiFiBase(){
    WiFi.onEvent(onWifiEvent);
}

bool connectToNetwork(const WiFiNetwork &net, const String &password) {
    WiFi.disconnect(true);
    delay(200);
    
    WiFi.mode(WIFI_STA);
    
    if (net.isOpen || password.length() == 0) {
        // Открытая сеть
        WiFi.begin(net.ssid.c_str());
    } else {
        // Защищённая сеть
        WiFi.begin(net.ssid.c_str(), password.c_str());
    }

    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 25) {  // ~12.5 секунд
        delay(500);
        timeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        return true;
    } else {
        return false;
    }
}

bool wifiIsConnected(){
    if (WiFi.status() == WL_CONNECTED) {
        return true;
    }
    return false;
}