#ifndef NETSCAN_H
#define NETSCAN_H

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <user_interface.h>
#include "config.h"

#define WIFI_PKT_MGMT 0x00
#define WIFI_PKT_CTRL 0x01
#define WIFI_PKT_DATA 0x02

void scanNetworks();
void displayNetworks();
void waitForUserInput();
void startClientScanning();
void clientScanningLoop();
bool compareMac(const uint8_t *mac1, const uint8_t *mac2);
bool isBroadcastMac(const uint8_t *mac);
bool isValidClientMac(const uint8_t *mac);
void printMac(const uint8_t *mac);
void addOrUpdateClient(const uint8_t *clientMac, int8_t rssi);
void promiscuousCallback(uint8_t *buf, uint16_t len);
String getSecurityType(int encType);
String getSignalStrength(int rssi);
void parseBSSID(String bssidStr, uint8_t *bssidBytes);
String getMacOUI(const uint8_t *mac);
String guessDeviceType(const uint8_t *mac);
String generateDeviceID(const uint8_t *mac);
void clearLines(int count);
void updateClientDisplay();
void handleSerialInput();

#endif