#include <Arduino.h>
#include <SPI.h>

#include "display/includes/tft.h"
#include "sd/includes/sd.h"
#include "input/includes/input.h"
#include "clock/includes/clk.h"
#include "hacking/includes/wifi.h"
#include "hacking/includes/bluetooth.h"
#include "usb/includes/usb.h"
#include "hacking/includes/nrf.h"
#include "hacking/includes/ir.h"
#include "ai/includes/ai.h"
#include "dht/includes/dht.h"
#include "game/includes/games.h"
#include "general.h"


void setup() {
    Serial.begin(115200);

    pinMode(TFT_CS,   OUTPUT); digitalWrite(TFT_CS,   HIGH);
    pinMode(SD_CS,    OUTPUT); digitalWrite(SD_CS,    HIGH);
    pinMode(NRF1_CSN, OUTPUT); digitalWrite(NRF1_CSN, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    initTFTMini();
    showBootScreen();
    initSD();
    initDHT();
    initJoy();
    initRTC();
    initUSB();
    randomSeed(analogRead(A0));
    initNRF(); 
}


enum Screen { SCR_MAIN, SCR_GAME, SCR_SETTINGS, SCR_HACKING, SCR_FILES, SCR_POWEROFF, 
  SCR_SLEEP, SCR_JARVIS, SCR_AIONLINE, SCR_WIFI, SCR_WIFI_ACTION, SCR_BLUETOOTH, SCR_NRF, SCR_SERVERS};
Screen currentScreen = SCR_MAIN;

MenuItem mainItems[] = {
    { "GAME",      "[RUN]" },
    { "SETTINGS",  "[CFG]" },
    { "OPTIONS",   "[OPT]" },
    { "FILES",     "[DAT]" },
    { "POWER OFF", "[PWR]" },
    { "SLEEP",     "[SLP]" },
    { "JARVIS",    "[AIJ]" },
    { "AI_ONLINE", "[AIO]" },
};

MenuItem hackItems[] = {
    { "WIFI",      "[WIF]" },
    { "BLUETOOTH", "[BLE]" },
    { "NRF24L01",  "[NRF]" },
    { "KEYBOARD",  "[HID]" },
    { "IR-LEDs",   "[IFR]" },
    { "SERVER C",  "[CLI]" },
    { "SERVER A",  "[ADM]" },
    { "TERMINAL",   "[CLV]" },
    { "BACK",      "[BCK]" },
};

MenuItem hackWifiItems[] = {
    { "SCAN",      "[SCN]" },
    { "BACK",      "[BCK]" },
};

MenuItem gameItems[] = {
    { "TETRIS",    "[JEU]" },
    { "BACK",      "[BCK]" },
};

MenuItem settingsItems[] = {
    { "BACK",      "[BCK]" },
};

MenuItem filesItems[] = {
    { "BACK",      "[BCK]" },
};

MenuItem poweroffItems[] = {
    { "BACK",      "[BCK]" },
};

MenuItem sleepItems[] = {
    { "BACK",      "[BCK]" },
};

MenuItem jarvisItems[] = {
    { "BACK",      "[BCK]" },
};

MenuItem aionlineItems[] = {
    { "CHAT",      "[CHT]" },
    { "BACK",      "[BCK]" },
};

MenuItem wifiBSSIDItems[] = {
    { "CONNECT",   "[CNC]" },
    { "INFO",      "[SCN]" },
    { "DEAUTH",    "[DAU]" },
    { "BACK",      "[BCK]" },
};

MenuItem wifiInfoBSSIDItems[] = {
    { "SSID",     "[CNC]" },
    { "BSSID",    "[SCN]" },
    { "RSSI",     "[SCN]" },
    { "CHANNEL",  "[DAU]" },
    { "OPEN",     "[TWN]" },
    { "BACK",     "[BCK]" },
};

MenuItem bleMenuItems[] = {
    { "APPLE",     "[DOS]" },
    { "SAMSUNG",   "[DOS]" },
    { "SWIFT P",   "[DOS]" },
    { "SOUR A",    "[DOS]" },
    { "JUICE A",   "[DOS]" },
    { "GOOGLE FP", "[DOS]" },
    { "BACK",      "[BKC]" },
};

MenuItem nrfMenuItems[] = {
    { "WIFI",       "[DOS]" },
    { "BLE",        "[DOS]" },
    { "USB",        "[DOS]" },
    { "VIDEO",      "[DOS]" },
    { "RC",         "[DOS]" },
    { "ALL",        "[DOS]" },
    { "BLUETOOTH",  "[DOS]" },
    { "BACK",       "[BKC]" },
};


bool premiereFois = true;
bool connect = false;

void loop() {
    updateWifiStatus();
    updateSDStatus();
    
    switch (currentScreen) {
        case SCR_MAIN: {
            int sel = drawMenuGeneric("MENU", mainItems, 8);
            if (sel == 0) currentScreen = SCR_GAME;  // GAME
            if (sel == 1) currentScreen = SCR_SETTINGS;  // GAME
            if (sel == 2) currentScreen = SCR_HACKING;  // HACKING
            if (sel == 3) {
                printDelayTFT();
                initSD();
                delay(2500);
                readCurrentDir("/");
                delay(500);
                currentScreen = SCR_FILES;
            }
            if (sel == 4) currentScreen = SCR_POWEROFF;  // GAME
            if (sel == 5) currentScreen = SCR_SLEEP;  // GAME
            if (sel == 6) currentScreen = SCR_JARVIS;  // GAME
            if (sel == 7) currentScreen = SCR_AIONLINE;  // GAME
            break;
        }

        case SCR_HACKING: {
            int sel = drawMenuGeneric("HACKING", hackItems, 9);
            if (sel == 0) {currentScreen = SCR_WIFI;}
            if (sel == 1) {currentScreen = SCR_BLUETOOTH;}
            if (sel == 4) {
                printAttackTFT();
                while (true) {
                    bool stop = sendIR();   
                    
                    if (stop || getClickJoyButton()) {
                        delay(150);
                        drawMenuGeneric("HACKING", hackItems, 9, true);
                        delay(300);
                        break;
                    }
                    delay(50);
                }
                drawMenuGeneric("HACKING", hackItems, 9, true);
            }
            if (sel == 2) {
                currentScreen = SCR_NRF;
            }
            if (sel == 5) {
                printDelayTFT();
                initAP();
                initUserServer();
                loopGetButtonServer();
                closeServer();
                drawMenuGeneric("HACKING", hackItems, 9, true);
            }
            if (sel == 6) {
                printDelayTFT();
                initAP();
                initServer();
                while (true) {
                    serverAdmins();
                    if (getClickJoyButton()) break;
                }
                closeServer();
                drawMenuGeneric("HACKING", hackItems, 9, true);
            }
            if (sel == 7) {                              
                showTerminal();
                drawMenuGeneric("HACKING", hackItems, 9, true);
            }
            if (sel == 8) {                              // BACK
                currentScreen = SCR_MAIN;
            }
            break;
        }

        case SCR_GAME: {
            int sel = drawMenuGeneric("GAME", gameItems, 2);
            if (sel == 0) {                              // BACK
                tftClear();
                tetris();
                drawMenuGeneric("GAME", gameItems, 2, true);
            }
            if (sel == 1) {                              // BACK
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 7, true); // reset
            }
            break;
        }
        
        case SCR_SETTINGS: {
            int sel = drawMenuGeneric("SETTINGS", settingsItems, 1);
            if (sel == 0) {                              // BACK
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 7, true); // reset
            }
            break;
        }

        case SCR_FILES: {
            if (dirMenuItems.empty()) {
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 8, true);
                break;
            }

            int sel = drawMenuGeneric("FILES", dirMenuItems.data(), dirMenuItems.size());

            if (sel >= 0) {
                FileType   type         = dirMenuTypes[sel];
                String     selectedPath = dirMenuPaths[sel];

                switch (type) {
                    case FT_BACK: {
                        // Определяем источник из текущего пути
                        bool isUsb = currentPath.startsWith("USB:");
                        String raw = isUsb ? currentPath.substring(4) :
                                    currentPath.startsWith("SD:") ? currentPath.substring(3) : currentPath;
                        int last = raw.lastIndexOf('/');
                        raw = (last <= 0) ? "/" : raw.substring(0, last);
                        currentPath = (isUsb ? "USB:" : "SD:") + raw;
                        readCurrentDir(currentPath.c_str());
                        drawMenuGeneric("FILES", dirMenuItems.data(), dirMenuItems.size(), true);
                        break;
                    }
                    case FT_EXIT:
                        currentScreen = SCR_MAIN;
                        drawMenuGeneric("MENU", mainItems, 8, true);
                        break;

                    case FT_DELETE:
                        deleteSD();
                        currentScreen = SCR_MAIN;
                        drawMenuGeneric("MENU", mainItems, 8, true);
                        break;

                    case FT_DIR:
                        currentPath = selectedPath;
                        if (readCurrentDir(currentPath.c_str()))
                            drawMenuGeneric("FILES", dirMenuItems.data(), dirMenuItems.size(), true);
                        break;

                    case FT_TEXT: {
                        printDelayTFT();
                        if (selectedPath.startsWith("USB:")) {
                            // Читаем с USB через FatFs
                            String usbPath = "1:" + selectedPath.substring(4);
                            FIL    fil;
                            String content = "";
                            if (f_open(&fil, usbPath.c_str(), FA_READ) == FR_OK) {
                                char buf[64]; UINT br;
                                while (f_read(&fil, buf, sizeof(buf)-1, &br) == FR_OK && br > 0) {
                                    buf[br] = '\0';
                                    content += String(buf);
                                    if (content.length() > 4096) break;
                                }
                                f_close(&fil);
                                showTextFile(content.c_str()); // передаём текст напрямую
                            }
                        } else {
                            String p = selectedPath.startsWith("SD:") ? selectedPath.substring(3) : selectedPath;
                            showTextFile(p.c_str());
                        }
                        drawMenuGeneric("FILES", dirMenuItems.data(), dirMenuItems.size(), true);
                        break;
                    }

                    default: break;
                }
            }
            break;
        }

        case SCR_POWEROFF: {
            int sel = drawMenuGeneric("POWER", poweroffItems, 1);
            if (sel == 0) {                              // BACK
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 7, true); // reset
            }
            break;
        }

        case SCR_SLEEP: {
            int sel = drawMenuGeneric("SLEEP", sleepItems, 1);
            if (sel == 0) {                              // BACK
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 7, true); // reset
            }
            break;
        }

        case SCR_JARVIS: {
            int sel = drawMenuGeneric("JARVIS", jarvisItems, 1);
            if (sel == 0) {                              // BACK
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 7, true); // reset
            }
            break;
        }

        case SCR_AIONLINE: {
            int sel = drawMenuGeneric("AI", aionlineItems, 2);
            if (sel == 0) {                              
                showAIChat();
                drawMenuGeneric("AI", aionlineItems, 2, true);
            }
            if (sel == 1) {                              // BACK
                currentScreen = SCR_MAIN;
                drawMenuGeneric("MENU", mainItems, 7, true); // reset
            }
            break;
        }

        case SCR_WIFI: {
            if (premiereFois){
                printDelayTFT();
                initWiFiScanner();
                premiereFois = false;
            }
            static bool scanned = false;

            if (!scanned) {
                buildWiFiMenu();
                tftClear();
                drawMenuGeneric( "WIFI_SCAN", wifiItems, networkCount + 1, !scanned);
                scanned = true;
            }

            int sel = drawMenuGeneric("WIFI_SCAN", wifiItems, networkCount + 1);
            if (sel >= 2) {
                WiFiNetwork net = getNetwork(sel);
                selectedNetIndex = sel - 2;                   
                selectedNet = getNetwork(selectedNetIndex);    
                currentScreen = SCR_WIFI_ACTION;              
                premiereFois = true;
            }
            if (sel == 0) { scanned = false; currentScreen = SCR_HACKING; premiereFois = true; }
            if (sel == 1) { scanned = false; premiereFois = true; }
            break;
        }

        case SCR_WIFI_ACTION: {
            if (premiereFois) {
                printDelayTFT();
                premiereFois = false;
                tftClear();
                drawMenuGeneric(selectedNet.ssid.c_str(), wifiBSSIDItems, 4, true);
            }

            int sel = drawMenuGeneric(selectedNet.ssid.c_str(), wifiBSSIDItems, 4, false);

            if (sel == 0) {
                String pass = "";
                connect = false;

                printDelayTFT();

                if (selectedNet.isOpen)
                {
                    connect = connectToNetwork(selectedNet);
                }
                else
                {
                    if (selectedNet.ssid == "RN11"){
                        pass = "88888888";
                    }
                    else{
                        pass = inputPassword("WiFi Password");

                        if (pass == "")
                        {
                            tftClear();
                            drawMenuGeneric(selectedNet.ssid.c_str(), wifiBSSIDItems, 9, true);
                            return; 
                        }
                    }
                    printDelayTFT();
                    connect = connectToNetwork(selectedNet, pass);
                }
                tftClear();
                drawMenuGeneric(selectedNet.ssid.c_str(), wifiBSSIDItems, 9, true);
            }
            else if (sel == 1) {    //INFO
                tftClear();
                String ssidStr = selectedNet.ssid.length() > 0 ? selectedNet.ssid : "<Hidden>";
                if (ssidStr.length() > 14) ssidStr = ssidStr.substring(0, 14) + "..";
                String bssidStr = selectedNet.bssid;
                bssidStr.replace(":", "");        // ← вот это главное
                String rssiStr = String(selectedNet.rssi) + " dBm";
                String channelStr = String(selectedNet.channel);
                String openStr = selectedNet.isOpen ? "Open" : "Secured";
                wifiInfoBSSIDItems[0] = (MenuItem){strdup(ssidStr.c_str()),     ""};
                wifiInfoBSSIDItems[1] = (MenuItem){strdup(bssidStr.c_str()),    ""};
                wifiInfoBSSIDItems[2] = (MenuItem){strdup(rssiStr.c_str()),     ""};
                wifiInfoBSSIDItems[3] = (MenuItem){strdup(channelStr.c_str()),  ""};
                wifiInfoBSSIDItems[4] = (MenuItem){strdup(openStr.c_str()),     ""};
                while (true) {
                    int infoSel = drawMenuGeneric(selectedNet.ssid.c_str(), wifiInfoBSSIDItems, 6, false);
                    
                    if (infoSel == 5) {        // Back
                        break;                 // выходим из INFO обратно в ACTION
                    }
                    
                    delay(50);
                }
                premiereFois = true;
            }
            else if (sel == 2) {   
                printAttackTFT();
                initDeauth();
                delay(1000);
                startDeauthAttack();
                unsigned long startTime = millis();
                while (deauthRunning) {
                    sendDeauthAttack();       
                    delay(10);                    // очень важно - маленькая задержка
                    
                    if (getClickJoyButton()) {
                        stopDeauthAttack();
                        break;
                    }
                }
                premiereFois = true;
            }
            else if (sel == 3) {    //EVIL TWIN
                currentScreen = SCR_WIFI;
                premiereFois = true;
            }
            break;
        }
    
        case SCR_BLUETOOTH: {
            if (premiereFois) {
                printDelayTFT();
                initBLE();
                premiereFois = false;
                tftClear();
                drawMenuGeneric("BLUETOOTH", bleMenuItems, 7, true);
            }
            int sel = drawMenuGeneric("BLUETOOTH", bleMenuItems, 7, false);

            if (sel == 0) {  
                printAttackTFT();
                while (true) {
                    sendRandomApple();
                    delay(20);        
        
                    if (getClickJoyButton()) {  
                        break;
                    }
                }
                premiereFois = true;
                delay(500);
            }
            else if (sel == 1) {   
                printAttackTFT();
                while (true) {
                    sendSamsung();
                    delay(20);        
        
                    if (getClickJoyButton()) {  
                        break;
                    }
                }
                premiereFois = true;
                delay(500);
            }
            else if (sel == 2) {   
                printAttackTFT();
                while (true) {
                    sendSwiftPair();
                    delay(20);        
        
                    if (getClickJoyButton()) {  
                        break;
                    }
                }
                premiereFois = true;
                delay(500);
            }
            else if (sel == 3) {   
                printAttackTFT();
                while (true) {
                    sendSourApple();
                    delay(20);        
        
                    if (getClickJoyButton()) {  
                        break;
                    }
                }
                premiereFois = true;
                delay(500);
            }
            else if (sel == 4) {   
                printAttackTFT();
                while (true) {
                    sendAppleJuice();
                    delay(20);        
        
                    if (getClickJoyButton()) {  
                        break;
                    }
                }
                premiereFois = true;
                delay(500);
            }
            else if (sel == 5) {   
                printAttackTFT();
                while (true) {
                    sendGoogleFastPair();
                    delay(20);        
        
                    if (getClickJoyButton()) {  
                        break;
                    }
                }
                premiereFois = true;
                delay(500);
            }
            else if (sel == 6) {   
                currentScreen = SCR_HACKING;
                premiereFois = true;
            }
            break;
        }

        case SCR_NRF: {
            if (premiereFois) {
                printDelayTFT();
                initNRF();
                premiereFois = false;
                tftClear();
                drawMenuGeneric("NRF24L01", nrfMenuItems, 8, true);
            }
            updateNRFStatus();
            int sel = drawMenuGeneric("NRF24L01", nrfMenuItems, 8, false);

            switch (sel) {
                case 0: mode = 'w'; break;
                case 1: mode = 'b'; break;
                case 2: mode = 'u'; break;
                case 3: mode = 'v'; break;
                case 4: mode = 'c'; break;
                case 5: mode = 'f'; break;
                case 6: mode = 'r'; break; 
                case 7: {
                    deactivateJammers();
                    premiereFois = true;
                    currentScreen = SCR_MAIN;
                    break;
                }
                default: break;
            }
            if (sel >= 0 && sel <= 6) {
                printAttackTFT();
                while (true) {
                    sendJammerNrf();
                    if (getClickJoyButton()) {
                        delay(100);
                        drawMenuGeneric("NRF24L01", nrfMenuItems, 8, true);
                        break;
                    }
                }
            }
        }
    }
    delay(100);
}

                                     