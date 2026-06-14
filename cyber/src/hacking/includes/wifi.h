#pragma once
#include "../../general.h"
#include "../../display/includes/tft.h"


struct WiFiNetwork {
    String  ssid;
    String  bssid;
    int32_t rssi;
    uint8_t channel;
    bool    isOpen;
};

extern WiFiNetwork networks[MAX_NETWORKS];
extern int        networkCount;

extern MenuItem wifiItems[MAX_NETWORKS + 2];
void buildWiFiMenu();

extern WiFiNetwork selectedNet;    
extern int selectedNetIndex;      

extern String wifiPassword; 

extern bool deauthRunning;
extern uint32_t deauthPacketCount;

void initWiFiScanner();
int  scanNetworks();

WiFiNetwork getNetwork(int index);
bool connectToNetwork(const WiFiNetwork &net, const String &password = "");

void stopDeauthAttack();
void startDeauthAttack();
void sendDeauthAttack();
void initDeauth();
bool wifiIsConnected();
void initWiFiBase();

void initAP();
void initServer();
void serverAdmins();
void serverUsers();
void closeServer();
void initUserServer();
void loopGetButtonServer();