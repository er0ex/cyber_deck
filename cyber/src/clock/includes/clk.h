#pragma once
#include <ThreeWire.h>
#include <RtcDS1302.h>

String getTimeStr();
String getDateStr();
void initRTC();
