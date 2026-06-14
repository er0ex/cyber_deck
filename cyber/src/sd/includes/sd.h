#pragma once
#include <SD.h>
#include <vector>
#include "../../display/includes/tft.h"


struct FileStruct{
    String name;
    String size;
};

enum FileType { FT_BACK, FT_DIR, FT_PHOTO, FT_VIDEO, FT_TEXT, FT_OTHER, FT_EXIT, FT_DELETE };

void initSD();
bool readCurrentDir(const char* path);
void resetSDCard();

extern std::vector<String> dirItems;
extern std::vector<MenuItem> dirMenuItems;
extern std::vector<FileType> dirMenuTypes;

extern std::vector<String> dirMenuPaths;
extern String currentPath;

extern bool stateSD;
