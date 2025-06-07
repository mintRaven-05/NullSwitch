#include <Arduino.h>
#include <../include/netscan.h>
#include <../include/shell.h>
#include "../include/oui.h"
#include "../include/utils.h"
#include "../include/globals.h"

void clearLines(int count) {
  for (int i = 0; i < count; i++) {
    Serial.print("\033[A");
    Serial.print("\033[K");
  }
}

bool compareMac(const uint8_t *mac1, const uint8_t *mac2) {
  return memcmp(mac1, mac2, 6) == 0;
}

bool isBroadcastMac(const uint8_t *mac) { return (mac[0] & 0x01) != 0; }

bool isValidClientMac(const uint8_t *mac) {
  bool allZero = true;
  for (int i = 0; i < 6; i++) {
    if (mac[i] != 0) {
      allZero = false;
      break;
    }
  }
  if (allZero)
    return false;
  if (isBroadcastMac(mac))
    return false;
  return true;
}

void printMac(const uint8_t *mac) {
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16)
      Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
}

void parseBSSID(String bssidStr, uint8_t *bssidBytes) {
  for (int i = 0; i < 6; i++) {
    String byteStr = bssidStr.substring(i * 3, i * 3 + 2);
    bssidBytes[i] = (uint8_t)strtol(byteStr.c_str(), NULL, 16);
  }
}