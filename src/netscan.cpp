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

String getMacOUI(const uint8_t *mac) {
  String ouiStr = "";
  if (mac[0] < 16)
    ouiStr += "0";
  ouiStr += String(mac[0], HEX);
  ouiStr += ":";
  if (mac[1] < 16)
    ouiStr += "0";
  ouiStr += String(mac[1], HEX);
  ouiStr += ":";
  if (mac[2] < 16)
    ouiStr += "0";
  ouiStr += String(mac[2], HEX);
  ouiStr.toUpperCase();
  return "[U](" + ouiStr + ")";
}

String getSecurityType(int encType) {
  switch (encType) {
  case ENC_TYPE_WEP:
    return "WEP";
  case ENC_TYPE_TKIP:
    return "WPA/PSK";
  case ENC_TYPE_CCMP:
    return "WPA2/PSK";
  case ENC_TYPE_NONE:
    return "Open";
  case ENC_TYPE_AUTO:
    return "WPA/WPA2/PSK";
  default:
    return "Unknown";
  }
}

String getSignalStrength(int rssi) {
  if (rssi > -30)
    return "Excellent";
  else if (rssi > -50)
    return "Good";
  else if (rssi > -60)
    return "Fair";
  else if (rssi > -70)
    return "Weak";
  else
    return "Frail";
}