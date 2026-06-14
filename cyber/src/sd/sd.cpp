#include "includes/sd.h"
#include "../general.h"
#include "../display/includes/tft.h"
#include "../usb/includes/usb.h"


std::vector<String> dirItems;
std::vector<MenuItem> dirMenuItems;
std::vector<FileType> dirMenuTypes;

std::vector<String> dirMenuPaths;
String currentPath = "/";

bool stateSD = false;

void initSD() {
    if (!SD.begin(SD_CS)) {
        SD.end();
        delay(500);
        for (int i = 0; i < 3; i++) {
            delay(1000);
            if (SD.begin(SD_CS, SPI, 4000000)) break;
            Serial.printf("Retry %d failed\n", i + 1);
        }
    }
}

void resetSDCard() {
    SD.end();
    delay(300);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin();
    for (int i = 0; i < 10; i++) SPI.transfer(0xFF);
    delay(300);
    SD.begin(SD_CS, SPI, 4000000);
}

String formatSize(size_t bytes) {
    if (bytes < 1024)
        return String(bytes) + "B";
    else if (bytes < 1024 * 1024)
        return String(bytes / 1024) + "K";
    else if (bytes < 1024 * 1024 * 1024)
        return String(bytes / (1024 * 1024)) + "M";
    else
        return String(bytes / (1024 * 1024 * 1024)) + "G";
}

// ── readCurrentDir ──
bool readCurrentDir(const char* path) {
    dirItems.clear();
    dirMenuItems.clear();
    dirMenuTypes.clear();
    dirMenuPaths.clear();

    String strPath = String(path);
    bool fromUsb = strPath.startsWith("USB:");
    String realPath = fromUsb ? strPath.substring(4) : 
                      strPath.startsWith("SD:") ? strPath.substring(3) : strPath;

    // ── SD ──
    if (!fromUsb) {
        File dir = SD.open(realPath.c_str());
        if (dir && dir.isDirectory()) {
            File file = dir.openNextFile();
            while (file) {
                String fullName = String(file.name());
                int slash = fullName.lastIndexOf('/');
                String name = (slash >= 0) ? fullName.substring(slash + 1) : fullName;

                if (name == "System Volume Information" || name == "LOST.DIR") {
                    file = dir.openNextFile(); continue;
                }

                String itemPath = (realPath == "/") ? "/" + name : realPath + "/" + name;
                dirMenuPaths.push_back("SD:" + itemPath);

                if      (file.isDirectory())                                                  dirMenuTypes.push_back(FT_DIR);
                else if (name.endsWith(".txt") || name.endsWith(".TXT"))                      dirMenuTypes.push_back(FT_TEXT);
                else if (name.endsWith(".jpg") || name.endsWith(".png") || name.endsWith(".bmp")) dirMenuTypes.push_back(FT_PHOTO);
                else if (name.endsWith(".mp4") || name.endsWith(".avi"))                      dirMenuTypes.push_back(FT_VIDEO);
                else                                                                           dirMenuTypes.push_back(FT_OTHER);

                String sz = formatSize(file.size());
                dirItems.push_back("[SD] " + name + " " + sz);
                file = dir.openNextFile();
            }
            dir.close();
        }
    }

    // ── USB Flash через FatFs ──
    if (fromUsb || usbHost.isMassStorage) {
        if (usbFsMounted) {
            String usbPath = "1:" + (realPath == "/" ? String("") : realPath);
            FF_DIR  dir;
            FILINFO fno;

            if (f_opendir(&dir, usbPath.c_str()) == FR_OK) {
                while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0) {
                    String name = String(fno.fname);
                    if (name == "." || name == ".." || 
                        name == "System Volume Information") continue;

                    String itemPath = (realPath == "/") ? "/" + name : realPath + "/" + name;
                    dirMenuPaths.push_back("USB:" + itemPath);

                    if (fno.fattrib & AM_DIR) {
                        dirMenuTypes.push_back(FT_DIR);
                        dirItems.push_back("[USB] " + name);
                    } else {
                        if      (name.endsWith(".txt") || name.endsWith(".TXT")) dirMenuTypes.push_back(FT_TEXT);
                        else if (name.endsWith(".jpg") || name.endsWith(".png")) dirMenuTypes.push_back(FT_PHOTO);
                        else                                                      dirMenuTypes.push_back(FT_OTHER);
                        dirItems.push_back("[USB] " + name + " " + formatSize(fno.fsize));
                    }
                }
                f_closedir(&dir);
            }
        }
    }

    if (dirItems.empty()) {
        Serial.println("DIR EMPTY");
        return false;
    }

    // BACK / EXIT / DELETE
    for (int i = 0; i < 3; i++) dirMenuPaths.insert(dirMenuPaths.begin(), "");
    dirMenuItems.push_back({"BACK",   "[BCK]"}); dirMenuTypes.insert(dirMenuTypes.begin(),     FT_BACK);
    dirMenuItems.push_back({"EXIT",   "[EXT]"}); dirMenuTypes.insert(dirMenuTypes.begin() + 1, FT_EXIT);
    dirMenuItems.push_back({"DELETE", "[DLT]"}); dirMenuTypes.insert(dirMenuTypes.begin() + 2, FT_DELETE);

    for (const String& name : dirItems)
        dirMenuItems.push_back({name.c_str(), ""});

    return true;
}
