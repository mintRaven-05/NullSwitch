#include "../include/globals.h"
#include "../include/beacon.h"

//======================AUTH==========================
bool authenticated = false;
bool awaitingPasswordRemoval = false;
bool settingPassword = false;

const uint8_t DEVICE_SALT[16] = {
    0x4f, 0x72, 0x69, 0x67, 0x69, 0x6e, 0x61, 0x6c,
    0x20, 0x53, 0x61, 0x6c, 0x74, 0x69, 0x6e, 0x67
};

//======================SHELL==========================
bool promptShown = false;
bool awaitingWiFiCredentials = false;
bool awaitingSSID = false;
bool abortRequested = false;
bool bootComplete = false;
String commandBuffer = "";
String commandHistory[HISTORY_SIZE] = {""};
int historyCount = 0;
int currentHistoryIndex = 0;
int cursorPosition = 0;
String wifiSSID = "";
String wifiPassword = "";



