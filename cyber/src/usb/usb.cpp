#include <Arduino.h>
#include <EspUsbHost.h>
#include "includes/usb.h"

MyKeyboard usbHost;

void initUSB()
{
    usbHost.begin();
    Serial.println("[USB] Host started");
}

void updateUSB()
{
    usbHost.task();

}

