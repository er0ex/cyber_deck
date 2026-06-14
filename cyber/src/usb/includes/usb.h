#include <EspUsbHost.h>
#include <SD.h>
#include <ff.h>

static FATFS usbFatFs;
static bool  usbFsMounted = false;

class MyKeyboard : public EspUsbHost {
private:
    bool deviceDetected = false;

    char hidToAscii(uint8_t key, bool shift) {
        if (key >= 0x04 && key <= 0x1D) return (shift ? 'A' : 'a') + (key - 0x04);
        if (key >= 0x1E && key <= 0x26) {
            const char n[] = "123456789";
            const char s[] = "!@#$%^&*(";
            return shift ? s[key-0x1E] : n[key-0x1E];
        }
        if (key == 0x27) return shift ? ')' : '0';
        switch (key) {
            case 0x2C: return ' ';
            case 0x2D: return shift ? '_' : '-';
            case 0x2E: return shift ? '+' : '=';
            case 0x2F: return shift ? '{' : '[';
            case 0x30: return shift ? '}' : ']';
            case 0x31: return shift ? '|' : '\\';
            case 0x33: return shift ? ':' : ';';
            case 0x34: return shift ? '"' : '\'';
            case 0x35: return shift ? '~' : '`';
            case 0x36: return shift ? '<' : ',';
            case 0x37: return shift ? '>' : '.';
            case 0x38: return shift ? '?' : '/';
            default:   return 0;
        }
    }

public:
    String lastKey        = "";
    bool   isKeyboard     = false;
    bool   isMassStorage  = false;

    void onReceive(const usb_transfer_t *transfer) override {
        if (!deviceDetected) {
            deviceDetected = true;
            if (_bInterfaceClass == 0x03) {
                isKeyboard    = true;
                isMassStorage = false;
                Serial.println("[USB] Keyboard");
            } else if (_bInterfaceClass == 0x08) {
                isMassStorage = true;
                isKeyboard    = false;
                Serial.println("[USB] Flash");
                if (!usbFsMounted) {
                    if (f_mount(&usbFatFs, "1:", 1) == FR_OK) {
                        usbFsMounted = true;
                        Serial.println("[USB] Mounted 1:");
                    }
                }
            }
        }
    }

    void onGone(const usb_host_client_event_msg_t *eventMsg) override {
        if (usbFsMounted) {
            f_unmount("1:");
            usbFsMounted = false;
        }
        isKeyboard     = false;
        isMassStorage  = false;
        deviceDetected = false;
        EspUsbHost::onGone(eventMsg);
    }

    void onKeyboard(hid_keyboard_report_t report,
                    hid_keyboard_report_t last_report) override {
        if (!isKeyboard) return;
        bool shift = (report.modifier & 0x02) || (report.modifier & 0x20);
        for (int i = 0; i < 6; i++) {
            uint8_t key = report.keycode[i];
            if (key == 0 || key == last_report.keycode[i]) continue;
            switch (key) {
                case 0x28: lastKey = "\n";      break;
                case 0x29: lastKey = "[ESC]";   break;
                case 0x2A: lastKey = "[BS]";    break;
                case 0x2B: lastKey = "\t";      break;
                case 0x4F: lastKey = "[RIGHT]"; break;
                case 0x50: lastKey = "[LEFT]";  break;
                case 0x51: lastKey = "[DOWN]";  break;
                case 0x52: lastKey = "[UP]";    break;
                default: {
                    char c = hidToAscii(key, shift);
                    lastKey = c ? String(c) : String("[0x") + String(key, HEX) + "]";
                }
            }
        }
    }
};

void initUSB();
void updateUSB();


extern MyKeyboard usbHost;