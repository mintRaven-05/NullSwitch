#ifndef GLOBALS_H
#define GLOBALS_H

#include "config.h"
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
