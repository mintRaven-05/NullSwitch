#include <Arduino.h>
#include "../include/wifi.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/globals.h"

// WiFi management functions
void scanWiFiNetworks() {
  Serial.println("\n\033[1;36mScanning for WiFi networks...\033[0m");
  Serial.println(
      "\033[36m------------------------------------------------\033[0m");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int networkCount = WiFi.scanNetworks();

  if (networkCount == 0) {
    Serial.println("\033[31mNo networks found.\033[0m");
  } else {
    Serial.print("\033[32mFound ");
    Serial.print(networkCount);
    Serial.println(" networks:\033[0m");
    Serial.println();

    for (int i = 0; i < networkCount; i++) {
      Serial.print("\033[33m");
      Serial.print(i + 1);
      Serial.print(": \033[0m");
      Serial.print(WiFi.SSID(i));
      Serial.print(" \033[90m(");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm) ");

      switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_WEP:
        Serial.print("\033[31m[WEP]\033[0m");
        break;
      case ENC_TYPE_TKIP:
        Serial.print("\033[33m[WPA]\033[0m");
        break;
      case ENC_TYPE_CCMP:
        Serial.print("\033[32m[WPA2]\033[0m");
        break;
      case ENC_TYPE_NONE:
        Serial.print("\033[92m[Open]\033[0m");
        break;
      case ENC_TYPE_AUTO:
        Serial.print("\033[32m[WPA/WPA2]\033[0m");
        break;
      default:
        Serial.print("\033[90m[Unknown]\033[0m");
        break;
      }
      Serial.println();
    }
  }
  Serial.println(
      "\033[36m------------------------------------------------\033[0m");
}