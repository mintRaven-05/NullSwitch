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

//======================NETSCAN==========================
bool networkScanActive = false;
ScanState currentScanState = WAITING_FOR_INPUT;
int networkCount = 0;
int clientCount = 0;
uint8_t targetBSSID[6] = {0};
int targetChannel = 1;
String targetSSID = "";

bool headerPrinted = false;
int lastActiveCount = 0;

//======================PKTMON==========================
bool packetMonitoringActive = false;
unsigned long prevTime = 0;
unsigned long channelSwitchTimer = 0;
unsigned long displayUpdateTimer = 0;
unsigned long curTime = 0;
unsigned long pkts = 0;
unsigned long no_deauths = 0;
unsigned long deauths = 0;
int curChannel = 1;
unsigned long maxVal = 0;
unsigned long totalPackets = 0;
unsigned long totalDeauths = 0;
bool autoScan = true;
int PKTtargetChannel = 1;
bool monitoringStarted = false;

unsigned int val[MAX_ROW] = {0};

