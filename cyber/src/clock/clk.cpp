#include "includes/clk.h"
#include "../general.h"
#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(DAT_CLK, CLK_CLK, RST_CLK);   // DAT, CLK, RST
RtcDS1302<ThreeWire> rtc(myWire);


void initRTC() {
    rtc.Begin();
    if (!rtc.IsDateTimeValid()) {
        rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
    }
    if (rtc.GetIsWriteProtected()) {
        rtc.SetIsWriteProtected(false);
    }
    if (!rtc.GetIsRunning()) {
        rtc.SetIsRunning(true);
    }
    Serial.println("[RTC] OK");
}

String getTimeStr() {
    RtcDateTime now = rtc.GetDateTime();
    char buf[9];
    snprintf(buf, sizeof(buf), "%02d:%02d", now.Hour(), now.Minute());
    return String(buf);
}

String getDateStr() {
    RtcDateTime now = rtc.GetDateTime();
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d-%02d", now.Month(), now.Day());
    return String(buf);
}