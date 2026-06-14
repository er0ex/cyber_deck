#include "includes/input.h"
#include "../general.h"


int x = 64;
int y = 64;
int oldX = 64;
int oldY = 64;

void initJoy() {
    pinMode(JOY_X, INPUT_PULLUP);      //
    pinMode(JOY_Y, INPUT_PULLUP);      //
    pinMode(JOY_B, INPUT_PULLUP);      //
    pinMode(JOY_BUTTON, INPUT_PULLUP); // |>

    delay(10);
}

bool getJoyX(){
    if (!digitalRead(JOY_X)) {
        return true;
    }
    return false;
}
bool getJoyY(){
    if (!digitalRead(JOY_BUTTON)) {
        return true;
    }
    return false;
}
bool getJoyYUp(){
    if (!digitalRead(JOY_B)) {
        return true;
    }
    return false;
}
bool getClickJoyButton() {
    if (!digitalRead(JOY_Y)) {
        return true;
    }
    return false;
}

bool setMapJoyXGameMode(){
    if (!digitalRead(JOY_X)) {
        return true;
    }
    return false;
}

bool setMapJoyYGameMode(){
    if (!digitalRead(JOY_Y)) {
        return true;
    }
    return false;
}


int setMapJoyYMenuModeSimple() {
    if (getJoyY() == true) return +1;   // НИЗ
    if (getJoyYUp() == true) return -1;   // ВВЕРХ
    return 0;                    // центр — не двигаем
}
int setMapJoyXMenuModeSimple() {
    if (getJoyX() == true) return +1;   // вверх
    return 0;                    // центр — не двигаем
}
