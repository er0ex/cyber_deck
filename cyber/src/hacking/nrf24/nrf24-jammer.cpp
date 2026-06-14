#include "RF24.h"
#include <SPI.h>
#include "esp_bt.h"
#include "esp_wifi.h"
#include "../../general.h"


// === Channel Definitions ===
byte bluetooth_even_channels[] = {
    2,  4,  6,  8,  10, 12, 14, 16, 18, 20,
    22, 24, 26, 28, 30, 32, 34, 36, 38, 40,
    42, 44, 46, 48, 50, 52, 54, 56, 58, 60,
    62, 64, 66, 68, 70, 72, 74, 76, 78, 80
};
byte bluetooth_odd_channels[] = {
    1,  3,  5,  7,  9,  11, 13, 15, 17, 19,
    21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
    41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
    61, 63, 65, 67, 69, 71, 73, 75, 77, 79
};
byte wifi_channels[] = {
    6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
    22, 24, 26, 28,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
    46, 48, 50, 52,
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68
};
byte ble_channels[] = {1, 2, 3, 25, 26, 27, 79, 80, 81};
byte usb_channels[] = {40, 50, 60};
byte video_channels[] = {70, 75, 80};
byte rc_channels[] = {1, 3, 5, 7};
byte full_channels[] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
    85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
};

const int num_bluetooth_even = sizeof(bluetooth_even_channels) / sizeof(bluetooth_even_channels[0]);
const int num_bluetooth_odd = sizeof(bluetooth_odd_channels) / sizeof(bluetooth_odd_channels[0]);
const int num_wifi = sizeof(wifi_channels) / sizeof(wifi_channels[0]);
const int num_ble = sizeof(ble_channels) / sizeof(ble_channels[0]);
const int num_usb = sizeof(usb_channels) / sizeof(usb_channels[0]);
const int num_video = sizeof(video_channels) / sizeof(video_channels[0]);
const int num_rc = sizeof(rc_channels) / sizeof(rc_channels[0]);
const int num_full = sizeof(full_channels) / sizeof(full_channels[0]);

// === State Variables ===
byte ch[2] = {45, 45}; // Current channel for each module
char mode = 'b';               // Default mode: random Bluetooth hopping
bool jammersActive = false;    // Flag for running state

bool jammerInit1 = false;
bool jammerInit2 = false;

RF24 radio_hspi1(NRF1_CE, NRF1_CSN); 
RF24 radio_hspi2(NRF2_CE, NRF2_CSN);


void configureRadio(RF24& radio, byte channel) {
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS); //RF24_2MBPS   RF24_250KBPS
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.startConstCarrier(RF24_PA_MAX, channel);
}


void initNRF() {
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();

  SPI.begin();
  delay(150);
  if (radio_hspi1.begin()) {
    Serial.println("HSPI Module 1 Jammer Initialized!");
    configureRadio(radio_hspi1, ch[0]);
    jammerInit1 = true;
  } else {
    Serial.println("HSPI Module 1 initialization failed!");
    delay(150);
    if (radio_hspi1.begin()) {
      Serial.println("HSPI Module 1 Jammer Initialized!");
      configureRadio(radio_hspi1, ch[0]);
      jammerInit1 = true;
    } else {
      Serial.println("HSPI Module 1 initialization failed!");
      delay(150);
      jammerInit1 = false;
    }
  }
  if (radio_hspi2.begin()) {
    Serial.println("HSPI Module 2 Jammer Initialized!");
    configureRadio(radio_hspi2, ch[1]);
    jammerInit2 = true;
  } else {
    Serial.println("HSPI Module 2 initialization failed!");
    delay(150);
    if (radio_hspi1.begin()) {
      Serial.println("HSPI Module 1 Jammer Initialized!");
      configureRadio(radio_hspi1, ch[0]);
      jammerInit1 = true;
    } else {
      Serial.println("HSPI Module 1 initialization failed!");
      delay(150);
      jammerInit1 = false;
    }
  }
}

void updateChannels(byte newCh[4]) {
  if (newCh[0] != ch[0]) { radio_hspi1.setChannel(newCh[0]); ch[0] = newCh[0]; }
  if (newCh[1] != ch[1]) { radio_hspi2.setChannel(newCh[1]); ch[1] = newCh[1]; }
}


void sendJammerNrf() {
  byte newCh[4];
  switch (mode) {
    case 'r': // Random Bluetooth hopping (odd/even)
      newCh[0] = bluetooth_odd_channels[random(num_bluetooth_odd)];
      newCh[1] = bluetooth_even_channels[random(num_bluetooth_even)];
      newCh[2] = bluetooth_odd_channels[random(num_bluetooth_odd)];
      newCh[3] = bluetooth_even_channels[random(num_bluetooth_even)];
      break;
    case 'w': // WiFi channels
      for (int i = 0; i < 2; i++) newCh[i] = wifi_channels[random(num_wifi)];
      break;
    case 'b': // BLE advertising channels
      for (int i = 0; i < 2; i++) newCh[i] = ble_channels[random(num_ble)];
      break;
    case 'u': // USB wireless channels
      for (int i = 0; i < 2; i++) newCh[i] = usb_channels[random(num_usb)];
      break;
    case 'v': // Video streaming channels
      for (int i = 0; i < 2; i++) newCh[i] = video_channels[random(num_video)];
      break;
    case 'c': // RC toys/drones channels
      for (int i = 0; i < 2; i++) newCh[i] = rc_channels[random(num_rc)];
      break;
    case 'f': // Full spectrum
      for (int i = 0; i < 2; i++) newCh[i] = full_channels[random(num_full)];
      break;
    default:
      for (int i = 0; i < 2; i++) newCh[i] = ch[i]; // Maintain current channels
      break;
  }
  updateChannels(newCh);
  delayMicroseconds(15 + random(35));
}


void deactivateJammers() {
  radio_hspi1.stopConstCarrier();
  radio_hspi2.stopConstCarrier();
  jammerInit1 = false;
  jammerInit2 = false;
  Serial.println("All jammers stopped transmission");
}