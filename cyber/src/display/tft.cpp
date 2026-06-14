#include <WiFi.h>
#include <SD.h>
#include "includes/tft.h"
#include "../input/includes/input.h"
#include "../clock/includes/clk.h"
#include "../general.h"
#include "../sd/includes/sd.h"
#include "../usb/includes/usb.h"
#include "../ai/includes/ai.h"
#include "../dht/includes/dht.h"
#include "../hacking/includes/nrf.h"


Adafruit_ST7735 tft = Adafruit_ST7735(&SPI, TFT_CS, TFT_DC, TFT_RST);

void initTFTMini() {
    tft.initR(INITR_144GREENTAB);
    tft.setRotation(2);
    tft.fillScreen(C_BG);
}

void printTextTFT(int x, int y, String text) {
    tft.setTextColor(C_TEXT);
    tft.setCursor(x, y);
    tft.println(text);
}

void drawWifiIcon(int x, int y, bool connected) {
    if (connected) {
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_WIFI_ON[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, C_TERM_ON);
    } else {
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_WIFI_BASE[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, C_TERM_DIM);

        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_WIFI_X[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, 0xF800);
    }
}

void drawSdIcon(int x, int y, bool mounted) {
    if (mounted) {
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_SD_BASE[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, C_TERM_ON);
    } else {
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_SD_BASE[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, C_TERM_DIM);

        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_SD_X[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, 0xF800);
    }
}

void drawNrfIcon(int x, int y, bool connected) {
    if (connected) {
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_NRF_ON[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, C_TERM_ON);
    } else {
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_NRF_BASE[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, C_TERM_DIM);

        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                if (ICON_NRF_X[row] & (1 << (7 - col)))
                    tft.drawPixel(x + col, y + row, 0xF800);
    }
}

void updateWifiStatus(){
    static uint32_t lastCheck = 0;
    static bool lastState = false;

    if (millis() - lastCheck < 2000)
        return;

    lastCheck = millis();

    bool currentState =
        (WiFi.status() == WL_CONNECTED);

    if (currentState != lastState)
    {
        lastState = currentState;

        tft.fillRect(80, 0, 18, 13, C_TERM_BG);
        tft.fillRect(90, 0, 18, 13, C_TERM_BG);
        tft.fillRect(100, 0, 18, 13, C_TERM_BG);
        tft.fillRect(110, 0, 18, 13, C_TERM_BG);

        drawNrfIcon(82, 2, jammerInit1);
        drawNrfIcon(92, 2, jammerInit2);
        drawWifiIcon(112, 2, currentState);
        drawSdIcon(102, 2, stateSD);
    }
}

void updateSDStatus() {
    static uint32_t lastSDCheck = 0;
    if (millis() - lastSDCheck < 2000) return;
    lastSDCheck = millis();

    bool nowMounted = SD.open("/") ? true : false;

    if (nowMounted != stateSD) {
        if (!nowMounted) {
            SD.end();
            stateSD = false;
        } else {
            SD.end();
            delay(500);
            resetSDCard();
            stateSD = true;
        }

        tft.fillRect(80, 0, 18, 13, C_TERM_BG);
        tft.fillRect(90, 0, 18, 13, C_TERM_BG);
        tft.fillRect(100, 0, 18, 13, C_TERM_BG);
        tft.fillRect(110, 0, 18, 13, C_TERM_BG);
        drawNrfIcon(82, 2, jammerInit1);
        drawNrfIcon(92, 2, jammerInit2);
        drawWifiIcon(112, 2, connect);
        drawSdIcon(102, 2, stateSD);
    }
}

void updateNRFStatus(){
    static uint32_t lastCheck = 0;
    static bool lastState1 = false;
    static bool lastState2 = false;

    if (millis() - lastCheck < 2000)
        return;

    lastCheck = millis();

    bool currentState1 = jammerInit1;
    bool currentState2 = jammerInit2;

    if (currentState1 != lastState1 || currentState2 != lastState2){
        lastState1 = currentState1;

        tft.fillRect(80, 0, 18, 13, C_TERM_BG);
        tft.fillRect(90, 0, 18, 13, C_TERM_BG);
        tft.fillRect(100, 0, 18, 13, C_TERM_BG);
        tft.fillRect(110, 0, 18, 13, C_TERM_BG);

        drawNrfIcon(82, 2, currentState1);
        drawNrfIcon(92, 2, currentState2);
        drawWifiIcon(112, 2, connect);
        drawSdIcon(102, 2, stateSD);
    }
}

int drawMenuGeneric(const char* title, MenuItem* items, int count, bool reset) {
    static const int VISIBLE = 5;
    static const int ITEM_H  = 20;

    static unsigned long lastFooterUpdate = 0;
    static String        footerCache      = getDateStr() + " | " + getTimeStr() + " | " + getDHTData();

    static MenuItem* lastItems = nullptr;
    static int       selected  = 0;
    static int       offset    = 0;
    static int       prevSel   = -1;
    static int       prevOff   = -1;
    static bool      firstDraw = true;

    if (lastItems != items || reset) {
        lastItems = items;
        selected  = 0;
        offset    = 0;
        prevSel   = -1;
        prevOff   = -1;
        firstDraw = true;
    }

    int joy = setMapJoyYMenuModeSimple();
    if (joy > 0 && selected < count - 1) {
        selected++;
        if (selected >= offset + VISIBLE) offset++;
    }
    if (joy < 0 && selected > 0) {
        selected--;
        if (selected < offset) offset--;
    }

    if (firstDraw) {
        firstDraw = false;
        tft.fillScreen(C_TERM_BG);

        tft.setTextSize(1);
        tft.setTextColor(C_TERM_ON);
        tft.setCursor(4, 4);
        tft.print("// ");
        tft.print(title);
        tft.print("");
        drawWifiIcon(112, 2, connect);
        drawSdIcon(102, 2, stateSD);
        drawNrfIcon(82, 2, jammerInit1);
        drawNrfIcon(92, 2, jammerInit2);
        tft.drawLine(0, 13, 128, 13, C_TERM_LINE);
        tft.drawLine(0, 116, 128, 116, C_TERM_LINE);
        tft.setTextColor(C_TERM_DIM);
        tft.setCursor(4, 119);
        tft.print(footerCache);
    }

    if (millis() - lastFooterUpdate > 30000) {
        lastFooterUpdate = millis();
        footerCache = getDateStr() + " | " + getTimeStr() + " | " + getDHTData();
        tft.fillRect(0, 116, 128, 12, C_TERM_BG);
        tft.setTextColor(C_TERM_DIM);
        tft.setCursor(4, 119);
        tft.print(footerCache);
    }

    if (selected == prevSel && offset == prevOff) {
        if (getClickJoyButton()) { delay(200); return selected; }
        return -1;
    }

    auto drawItem = [&](int row, int idx) {
        if (row < 0 || row >= VISIBLE || idx >= count) return;
        bool sel = (idx == selected);
        int  y   = 16 + row * ITEM_H;

        tft.fillRect(0, y, 125, ITEM_H - 1, sel ? C_TERM_SEL : C_TERM_BG);
        if (sel) tft.fillRect(0, y, 2, ITEM_H - 1, C_TERM_ON);

        tft.setTextColor(C_TERM_NUM);
        tft.setCursor(4, y + 6);
        if (idx < 9) tft.print("0");
        tft.print(idx + 1);

        tft.setTextColor(sel ? C_TERM_ON : C_TERM_LINE);
        tft.setCursor(18, y + 6);
        tft.print(sel ? ">" : ".");

        tft.setTextColor(sel ? C_TERM_ON : C_TERM_DIM);
        tft.setCursor(27, y + 6);
        tft.print(items[idx].label);

        tft.setTextColor(sel ? C_TERM_DIM : C_TERM_LINE);
        tft.setCursor(92, y + 6);
        tft.print(items[idx].tag);
    };

    if (offset != prevOff) {
        for (int row = 0; row < VISIBLE; row++)
            drawItem(row, offset + row);
    } else {
        if (prevSel >= 0) drawItem(prevSel - offset, prevSel);
        drawItem(selected - offset, selected);
    }

    tft.fillRect(126, 14, 2, 102, C_TERM_LINE);
    int barH = max(8, 102 / count);
    int barY = 14 + (102 - barH) * selected / max(1, count - 1);
    tft.fillRect(126, barY, 2, barH, C_TERM_ON);

    prevSel = selected;
    prevOff = offset;

    if (getClickJoyButton()) { delay(200); return selected; }
    return -1;
}

void printDelayTFT() {
    const int boxW = 90;
    const int boxH = 36;
    int x = (128 - boxW) / 2;
    int y = (128 - boxH) / 2;
    tft.fillRoundRect(x, y, boxW, boxH, 6, C_TERM_SEL);
    tft.drawRoundRect(x, y, boxW, boxH, 6, C_TERM_ON);
    tft.setTextColor(C_TERM_ON);
    tft.setTextSize(1);
    int textX = x + 20;
    int textY = y + 15;
    tft.setCursor(textX, textY);
    tft.setTextSize(1);
    tft.print("Wait...");
}

void printAttackTFT() {
    const int boxW = 90;
    const int boxH = 36;
    int x = (128 - boxW) / 2;
    int y = (128 - boxH) / 2;
    tft.fillRoundRect(x, y, boxW, boxH, 6, C_TERM_SEL);
    tft.drawRoundRect(x, y, boxW, boxH, 6, C_TERM_ON);
    tft.setTextColor(C_TERM_ON);
    tft.setTextSize(1);
    int textX = x + 20;
    int textY = y + 15;
    tft.setCursor(textX, textY);
    tft.setTextSize(1);
    tft.print("Attack...");
}

void tftClear(){
    tft.fillScreen(C_TERM_BG);
}

void deleteSD() {
    SD.end();
    stateSD = false;
    tft.fillRect(100, 0, 18, 13, C_TERM_BG);
    tft.fillRect(110, 0, 18, 13, C_TERM_BG);
    drawWifiIcon(112, 2, connect);
    drawSdIcon(102, 2, stateSD);
}

void showTextFile(const char* path) {
    File f = SD.open(path);
    if (!f) return;

    static unsigned long lastFooterUpdate = 0;
    static String        footerCache      = getDateStr() + " | " + getTimeStr() + " | " + getDHTData();

    std::vector<String> lines;
    String line = "";
    while (f.available()) {
        char c = f.read();
        if (c == '\n' || c == '\r') {
            if (line.length() > 0) {
                while (line.length() > 20) {
                    lines.push_back(line.substring(0, 20));
                    line = line.substring(20);
                }
                lines.push_back(line);
                line = "";
            }
        } else {
            line += c;
        }
    }
    if (line.length() > 0) lines.push_back(line);
    f.close();

    const int VISIBLE = 5;
    const int ITEM_H  = 20;
    int offset  = 0;
    int prevOff = -1;
    bool redraw = true;

    while (true) {
        delay(100);
        int joy = setMapJoyYMenuModeSimple();
        if (joy > 0 && offset < (int)lines.size() - VISIBLE) offset++;
        if (joy < 0 && offset > 0) offset--;

        if (millis() - lastFooterUpdate > 30000) {
            lastFooterUpdate = millis();
            footerCache = getDateStr() + " | " + getTimeStr() + " | " + getDHTData();
            tft.fillRect(0, 116, 128, 12, C_TERM_BG);
            tft.setTextColor(C_TERM_DIM);
            tft.setCursor(4, 119);
            tft.print(footerCache);
        }

        if (redraw || offset != prevOff) {
            if (redraw) {
                tft.fillScreen(C_TERM_BG);
                tft.setTextSize(1);
                tft.setTextColor(C_TERM_ON);
                tft.setCursor(4, 4);
                tft.print("// TXT");
                drawWifiIcon(112, 2, connect);
                drawSdIcon(102, 2, stateSD);
                tft.drawLine(0, 13, 128, 13, C_TERM_LINE);
                tft.drawLine(0, 116, 128, 116, C_TERM_LINE);
                tft.setTextColor(C_TERM_DIM);
                tft.setCursor(4, 119);
                tft.print(footerCache);
                redraw = false;
            }

            tft.fillRect(0, 14, 125, 102, C_TERM_BG);
            for (int row = 0; row < VISIBLE; row++) {
                int idx = offset + row;
                if (idx >= (int)lines.size()) break;
                int y = 16 + row * ITEM_H;
                tft.setTextColor(C_TERM_DIM);
                tft.setCursor(4, y + 6);
                tft.print(lines[idx]);
            }

            tft.fillRect(126, 14, 2, 102, C_TERM_LINE);
            int barH = max(8, 102 / max(1, (int)lines.size()));
            int barY = 14 + (102 - barH) * offset / max(1, (int)lines.size() - 1);
            tft.fillRect(126, barY, 2, barH, C_TERM_ON);

            prevOff = offset;
        }

        if (getClickJoyButton()) { delay(200); break; }
    }
}

void showBootScreen() {
    tft.fillScreen(C_TERM_BG);

    // сетка
    for (int x = 0; x < 128; x += 8)
        tft.drawFastVLine(x, 0, 128, C_TERM_LINE);

    for (int y = 0; y < 128; y += 8)
        tft.drawFastHLine(0, y, 128, C_TERM_LINE);

    int cx = 64;
    int cy = 64;

    // внешний ромб
    tft.drawLine(cx, 18, 110, cy, C_TERM_ON);
    tft.drawLine(110, cy, cx, 110, C_TERM_ON);
    tft.drawLine(cx, 110, 18, cy, C_TERM_ON);
    tft.drawLine(18, cy, cx, 18, C_TERM_ON);

    // внутренний ромб
    tft.drawLine(cx, 34, 94, cy, C_TERM_ON);
    tft.drawLine(94, cy, cx, 94, C_TERM_ON);
    tft.drawLine(cx, 94, 34, cy, C_TERM_ON);
    tft.drawLine(34, cy, cx, 34, C_TERM_ON);

    // круги
    tft.drawCircle(cx, cy, 28, C_TERM_ON);
    tft.drawCircle(cx, cy, 20, C_TERM_DIM);
    tft.drawCircle(cx, cy, 12, C_TERM_ON);

    // центр
    tft.fillCircle(cx, cy, 4, C_TERM_ON);

    // лучи
    tft.drawFastHLine(0, cy, 128, C_TERM_DIM);
    tft.drawFastVLine(cx, 0, 128, C_TERM_DIM);

    tft.drawLine(20, 20, 108, 108, C_TERM_DIM);
    tft.drawLine(108, 20, 20, 108, C_TERM_DIM);

    delay(3000);
}

void showTerminal() {
    std::vector<String> history;
    String input = "";

    static unsigned long lastFooterUpdate = 0;
    static String        footerCache      = getDateStr() + " | " + getTimeStr() + " | " + getDHTData();

    auto redrawAll = [&]() {
        tft.fillScreen(C_TERM_BG);
        tft.setTextSize(1);
        tft.setTextColor(C_TERM_ON);
        tft.setCursor(4, 4);
        tft.print("// TERMINAL");
        tft.drawLine(0, 13, 128, 13, C_TERM_LINE);
        tft.drawLine(0, 116, 128, 116, C_TERM_LINE);
        tft.setTextColor(C_TERM_DIM);
        tft.setCursor(4, 119);
        tft.print(footerCache);

        // вывод истории
        const int MAX_LINES = 9;
        const int LINE_H    = 10;
        int start = max(0, (int)history.size() - MAX_LINES);
        for (int i = start; i < (int)history.size(); i++) {
            int y = 16 + (i - start) * LINE_H;
            tft.setTextColor(C_TERM_ON);
            tft.setCursor(4, y);
            tft.print(history[i]);
        }

        // строка ввода
        tft.setTextColor(C_TERM_ON);
        tft.setCursor(4, 106);
        tft.print("-> " + input + "_");
    };

    if (millis() - lastFooterUpdate > 30000) {
        lastFooterUpdate = millis();
        footerCache = getDateStr() + " | " + getTimeStr() + " | " + getDHTData();
        tft.fillRect(0, 116, 128, 12, C_TERM_BG);
        tft.setTextColor(C_TERM_DIM);
        tft.setCursor(4, 119);
        tft.print(footerCache);
    }

    auto runCommand = [&](String cmd) -> String {
        cmd.trim();
        if (cmd == "") return "";

        // ls sd
        if (cmd == "ls sd") {
            String result = "";
            File root = SD.open("/");
            if (!root) return "SD error";
            File f = root.openNextFile();
            while (f) {
                result += String(f.name()) + " ";
                f = root.openNextFile();
            }
            return result == "" ? "(empty)" : result;
        }

        // ls /path
        if (cmd.startsWith("ls ")) {
            String path = cmd.substring(3);
            path.trim();
            File root = SD.open(path);
            if (!root) return "not found";
            File f = root.openNextFile();
            String result = "";
            while (f) {
                result += String(f.name()) + " ";
                f = root.openNextFile();
            }
            return result == "" ? "(empty)" : result;
        }

        // cat /file.txt
        if (cmd.startsWith("cat ")) {
            String path = cmd.substring(4);
            path.trim();
            File f = SD.open(path);
            if (!f) return "not found";
            String out = "";
            while (f.available() && out.length() < 80) {
                out += (char)f.read();
            }
            f.close();
            return out;
        }

        // clear
        if (cmd == "clear") {
            history.clear();
            return "";
        }

        // help
        if (cmd == "help") {
            history.push_back("ls sd / ls /path");
            history.push_back("cat /file.txt");
            history.push_back("clear  exit");
            return "";
        }

        // help
        if (cmd == "help") {
            history.push_back("ls sd / ls /path");
            history.push_back("cat /file.txt");
            history.push_back("clear  exit");
            return "";
        }

        if (cmd == "ip") {
            return WiFi.localIP().toString();
        }

        if (cmd == "restart") {
            ESP.restart();
            return "";
        }

        if (cmd == "shutdown") {
            history.push_back("   Shutting down...");
            redrawAll();
            delay(1000);
            esp_sleep_enable_ext0_wakeup(GPIO_NUM_3, LOW);
            esp_deep_sleep_start();
            return "";
        }

        if (cmd == "exit") return "\x01"; // сигнал выхода

        return "unknown: " + cmd;
    };

    redrawAll();

    while (true) {
        delay(100);
        updateUSB();

        if (usbHost.lastKey != "") {
            String k = usbHost.lastKey;
            usbHost.lastKey = "";

            if (k == "[ESC]") break;

            if (k == "[BS]") {
                if (input.length() > 0)
                    input.remove(input.length() - 1);
            }
            else if (k == "\n") {
                String cmd = input;
                history.push_back("-> " + cmd);
                input = "";

                String result = runCommand(cmd);
                if (result == "\x01") break; // exit
                if (result != "") history.push_back("   " + result);
            }
            else if (k.length() == 1) {
                input += k;
            }

            redrawAll();
        }

        if (getClickJoyButton()) { delay(200); break; }
    }
}

String inputPassword(const String& title){
    String password;
    auto redraw = [&]()
    {
        tft.fillScreen(C_TERM_BG);

        tft.setTextColor(C_TERM_ON);
        tft.setTextSize(1);

        tft.setCursor(10, 25);
        tft.print(title);

        tft.drawRoundRect(8, 50, 112, 22, 4, C_TERM_ON);

        tft.setCursor(12, 57);

        for (int i = 0; i < password.length(); i++)
            tft.print('*');

        tft.print('_');
    };

    redraw();

    while (true){
        delay(100);
        updateUSB();

        if (usbHost.lastKey != ""){
            String k = usbHost.lastKey;
            usbHost.lastKey = "";

            if (k == "[ESC]"){
                return "";
            }
            else if (k == "[BS]"){
                if (password.length())
                    password.remove(password.length() - 1);
            }
            else if (k == "\n"){
                return password;
            }
            else if (k.length() == 1){
                password += k;
            }

            redraw();
        }

        if (getClickJoyButton())
        {
            delay(200);
            return "";
        }
    }
}

void drawTetrisMatrix(int x, int y, int w, int h, uint16_t color){
    tft.drawRect(x, y, w, h, color);
}

void drawTetrisFigure(int x, int y, int w, int h, uint16_t color){
    tft.fillRect(x, y, w, h, color);
}

void showAIChat() {
    std::vector<String> history;
    String input = "";

    const int MAX_LINES = 6;
    const int LINE_H = 12;
    const int START_Y = 16;
    const int MAX_CHARS = 20;

    int scrollOffset = 0; // 0 = внизу, >0 = скролл вверх

    auto wrapPush = [&](const String &prefix, const String &text) {
        int prefLen = (int)prefix.length();
        int firstAvail = MAX_CHARS - prefLen;  // сколько текста влезает в первую строку
        int nextAvail  = MAX_CHARS - prefLen;  // то же для остальных (одинаковый отступ)

        if (firstAvail <= 0) firstAvail = 1;
        if (nextAvail  <= 0) nextAvail  = 1;

        String indent = "";
        for (int i = 0; i < prefLen; i++) indent += " ";

        int pos = 0; // позиция в text (не в full!)
        bool first = true;

        while (pos <= (int)text.length()) {
            int avail = first ? firstAvail : nextAvail;
            String chunk = text.substring(pos, pos + avail);

            if (first)
                history.push_back(prefix + chunk);
            else
                history.push_back(indent + chunk);

            first = false;
            pos += avail;

            // если text пустой — один проход и выход
            if (text.length() == 0) break;
        }
    };

    auto drawLine = [&](int y, const String &text, bool isInput = false) {
        tft.fillRect(0, y, 128, LINE_H, C_TERM_BG);
        tft.setCursor(4, y);
        tft.setTextColor(isInput ? C_TERM_ON : C_TERM_DIM);
        tft.print(text);
    };

    auto redraw = [&]() {
        tft.fillScreen(C_TERM_BG);

        // header
        tft.setTextSize(1);
        tft.setTextColor(C_TERM_ON);
        tft.setCursor(4, 4);
        tft.print("// AI CHAT");

        drawWifiIcon(112, 2, connect);
        drawSdIcon(102, 2, stateSD);

        tft.drawLine(0, 13, 128, 13, C_TERM_LINE);
        tft.drawLine(0, 116, 128, 116, C_TERM_LINE);

        int totalLines = (int)history.size();
        int maxScroll = max(0, totalLines - MAX_LINES);
        scrollOffset = constrain(scrollOffset, 0, maxScroll);

        int start = max(0, totalLines - MAX_LINES - scrollOffset);

        for (int i = 0; i < MAX_LINES; i++) {
            int idx = start + i;
            int y = START_Y + i * LINE_H;

            if (idx < totalLines)
                drawLine(y, history[idx]);
            else
                drawLine(y, "");
        }

        // Индикатор скролла — маленький треугольник вверху если есть что листать
        if (scrollOffset > 0) {
            tft.setTextColor(C_TERM_ON);
            tft.setCursor(118, 15);
            tft.print("^");
        }
        if (scrollOffset < maxScroll) {
            tft.setTextColor(C_TERM_DIM);
            tft.setCursor(118, 106);
            tft.print("v");
        }

        // input line
        String shown = "> " + input + "_";
        if ((int)shown.length() > MAX_CHARS)
            shown = shown.substring(shown.length() - MAX_CHARS);

        drawLine(106, shown, true);
    };

    redraw();

    while (true) {
        delay(30);
        updateUSB();

        // ===== Джойстик — листаем историю =====
        int joy = setMapJoyYMenuModeSimple();
        if (joy > 0) {
            scrollOffset++;
            redraw();
            delay(150); // антидребезг
        } else if (joy < 0) {
            if (scrollOffset > 0) {
                scrollOffset--;
                redraw();
                delay(150);
            }
        }

        if (usbHost.lastKey != "") {
            String k = usbHost.lastKey;
            usbHost.lastKey = "";

            if (k == "[ESC]")
                break;

            else if (k == "[BS]") {
                if (input.length())
                    input.remove(input.length() - 1);
            }

            else if (k == "\n") {
                String msg = input;
                input = "";
                scrollOffset = 0; // при отправке — прыгаем вниз

                wrapPush("> ", msg);

                String reply = askDeepSeek(msg);
                if (reply == "") reply = "[no response]";

                wrapPush("AI: ", reply);
            }

            else if (k.length() == 1 && isPrintable(k[0])) {
                input += k;
                scrollOffset = 0; // при вводе — держимся внизу
            }

            redraw();
        }

        if (getClickJoyButton()){
            break;
        }
    }
}