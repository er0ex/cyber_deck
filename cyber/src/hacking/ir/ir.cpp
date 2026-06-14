#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <ir_Mitsubishi.h>
#include <ir_LG.h>
#include <ir_Samsung.h>
#include <ir_Panasonic.h>
#include <ir_Tcl.h>
#include <ir_Gree.h>

#include "../../general.h"
#include "../../input/includes/input.h"

// ================= НАСТРОЙКИ =================
#define IR_LED_1    IR_1     // Первый передатчик

// Основные отправщики
IRsend irsend(IR_LED_1);

IRDaikinESP acDaikin(IR_LED_1);
IRMitsubishiAC acMitsubishi(IR_LED_1);
IRLgAc acLG(IR_LED_1);
IRSamsungAc acSamsung(IR_LED_1);
IRPanasonicAc acPanasonic(IR_LED_1);
IRTcl112Ac acTcl(IR_LED_1);
IRGreeAC acGree(IR_LED_1);

void initIR() {
    irsend.begin();
    
    acDaikin.begin();
    acMitsubishi.begin();
    acLG.begin();
    acSamsung.begin();
    acPanasonic.begin();
    acTcl.begin();
    acGree.begin();
}

bool sendIR() {                   
    for (uint32_t code = 0x00000000; code < 0xFFFFFFFF; code += 0x11111111) {
        
        irsend.sendNEC(code, 32);
        irsend.sendNEC(code, 32);
        irsend.sendSamsung36(code, 32);
        irsend.sendSamsung36(code, 32);

        if (code % 50000 == 0) {      
            if (getClickJoyButton()) {
                Serial.println("[IR] Attack stopped by user");
                return true;           
            }
        }
        delay(35);
    }

    irsend.sendSony(0x12345, 20);
    irsend.sendSony(0x12345, 20);
    irsend.sendLG(0x20DF10EF, 32);
    irsend.sendLG(0x20DF10EF, 32);
    delay(50);

    // ===================== КОНДИЦИОНЕРЫ =====================
    acDaikin.setPower(true);
    acDaikin.setMode(kDaikinCool);
    acDaikin.setTemp(24);
    acDaikin.send();
    if (getClickJoyButton()) return true;
    delay(150);

    acDaikin.setPower(false);
    acDaikin.send();
    if (getClickJoyButton()) return true;
    delay(100);

    acLG.setPower(true);
    acLG.setTemp(23);
    acLG.send();
    if (getClickJoyButton()) return true;
    delay(120);

    uint16_t rawData[67] = {9000, 4500, 600, 1650 /*...*/};
    irsend.sendRaw(rawData, 67, 38);
    irsend.sendRaw(rawData, 67, 38);

    if (getClickJoyButton()) return true;

    Serial.println("→ Цикл завершён, повторяем...");
    return false;
}