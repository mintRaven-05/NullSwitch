#ifndef GLOBALS_H
#define GLOBALS_H

#include "config.h"
#include "netscan.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

// #define HISTORY_SIZE 10
// #define MAX_NETWORKS 64
// #define MAX_CLIENTS 64
#define maxRow 64

//======================AUTH==========================
extern bool authenticated;
extern bool awaitingPasswordRemoval;
extern bool settingPassword;

extern const uint8_t DEVICE_SALT[16];

//======================SHELL==========================
extern bool promptShown;
extern bool awaitingWiFiCredentials;
extern bool awaitingSSID;
extern bool abortRequested;
extern bool bootComplete;
extern String commandBuffer;
extern String commandHistory[HISTORY_SIZE];
extern int historyCount;
extern int currentHistoryIndex;
extern int cursorPosition;
extern String wifiSSID;
extern String wifiPassword;

//======================NETSCAN==========================
extern bool networkScanActive;
enum ScanState { SCANNING_NETWORKS, WAITING_FOR_INPUT, SCANNING_CLIENTS };
extern ScanState currentScanState;
extern int networkCount;
extern int clientCount;

extern uint8_t targetBSSID[6];
extern int targetChannel;
extern String targetSSID;

extern bool headerPrinted;
extern int lastActiveCount;

//=================NETWORKS & CLIENTS===================
extern NetworkInfo networks[MAX_NETWORKS];
extern ClientInfo clients[MAX_CLIENTS];

//======================PKTMON==========================
extern bool packetMonitoringActive;
extern unsigned long prevTime;
extern unsigned long channelSwitchTimer;
extern unsigned long displayUpdateTimer;
extern unsigned long curTime;
extern unsigned long pkts;
extern unsigned long no_deauths;
extern unsigned long deauths;
extern int curChannel;
extern unsigned long maxVal;
extern unsigned long totalPackets;
extern unsigned long totalDeauths;
extern bool autoScan;
extern int PKTtargetChannel;
extern bool monitoringStarted;

extern unsigned int val[MAX_ROW];

//======================BEACON==========================
extern char network_list[650] PROGMEM;
extern bool beaconAttackActive;

#endif // GLOBALS_H