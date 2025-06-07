#include <Arduino.h>
#include "../include/wifi.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/globals.h"

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

void connectToWiFi(String ssid, String password) {
  Serial.println("\n\033[1;36mConnecting to WiFi...\033[0m");
  Serial.print("\033[33mSSID: \033[0m");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  Serial.print("\033[33mConnecting");
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\033[32mWiFi connected successfully!\033[0m");
    Serial.print("\033[33mIP Address: \033[0m");
    Serial.println(WiFi.localIP());
    Serial.print("\033[33mSignal Strength: \033[0m");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\033[31mFailed to connect to WiFi!\033[0m");
    Serial.print("\033[31mStatus: \033[0m");
    Serial.println(WiFi.status());
  }

void disconnectFromWiFi() {
  Serial.println("\n\033[1;33mDisconnecting from WiFi...\033[0m");
  WiFi.disconnect();
  Serial.println("\033[32mWiFi disconnected.\033[0m");
}

void showWiFiStatus() {
  Serial.println("\n\033[1;36mWiFi STATUS\033[0m");
  Serial.println(
      "\033[36m================================================\033[0m");
  Serial.print("\033[33mStatus: \033[0m");

  switch (WiFi.status()) {
  case WL_CONNECTED:
    Serial.println("\033[32mConnected\033[0m");
    Serial.print("\033[33mSSID: \033[0m");
    Serial.println(WiFi.SSID());
    Serial.print("\033[33mIP Address: \033[0m");
    Serial.println(WiFi.localIP());
    Serial.print("\033[33mGateway: \033[0m");
    Serial.println(WiFi.gatewayIP());
    Serial.print("\033[33mSubnet Mask: \033[0m");
    Serial.println(WiFi.subnetMask());
    Serial.print("\033[33mDNS: \033[0m");
    Serial.println(WiFi.dnsIP());
    Serial.print("\033[33mMAC Address: \033[0m");
    Serial.println(WiFi.macAddress());
    Serial.print("\033[33mSignal Strength: \033[0m");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("\033[33mChannel: \033[0m");
    Serial.println(WiFi.channel());
    break;
  case WL_NO_SSID_AVAIL:
    Serial.println("\033[31mNetwork not found\033[0m");
    break;
  case WL_CONNECT_FAILED:
    Serial.println("\033[31mConnection failed\033[0m");
    break;
  case WL_WRONG_PASSWORD:
    Serial.println("\033[31mWrong password\033[0m");
    break;
  case WL_DISCONNECTED:
    Serial.println("\033[33mDisconnected\033[0m");
    break;
  default:
    Serial.print("\033[31mUnknown status: \033[0m");
    Serial.println(WiFi.status());
    break;
  }

  Serial.print("\033[33mWiFi Mode: \033[0m");
  switch (WiFi.getMode()) {
  case WIFI_OFF:
    Serial.println("OFF");
    break;
  case WIFI_STA:
    Serial.println("Station");
    break;
  case WIFI_AP:
    Serial.println("Access Point");
    break;
  case WIFI_AP_STA:
    Serial.println("AP + Station");
    break;
  default:
    Serial.println("Unknown");
    break;
  }

  Serial.println(
      "\033[36m================================================\033[0m");
}

void handleWiFiCredentials() {
  if (!awaitingWiFiCredentials)
    return;

  if (checkForAbort()) {
    Serial.println();
    Serial.println("\033[33mWiFi connection setup aborted.\033[0m");
    awaitingWiFiCredentials = false;
    awaitingSSID = true;
    wifiSSID = "";
    wifiPassword = "";
    commandBuffer = "";
    cursorPosition = 0;
    showPrompt();
    return;
  }

  while (Serial.available()) {
    char c = Serial.read();

    if (c == 3) {
      Serial.println();
      Serial.println("\033[33mWiFi connection setup aborted.\033[0m");
      awaitingWiFiCredentials = false;
      awaitingSSID = true;
      wifiSSID = "";
      wifiPassword = "";
      commandBuffer = "";
      cursorPosition = 0;
      showPrompt();
      return;
    }

    if (c == '\n' || c == '\r') {
      if (commandBuffer.length() > 0) {
        if (awaitingSSID) {
          wifiSSID = commandBuffer;
          Serial.println();
          Serial.print(
              "\033[33mEnter WiFi password (Ctrl+C to abort): \033[0m");
          awaitingSSID = false;
        } else {
          wifiPassword = commandBuffer;
          Serial.println();
          awaitingWiFiCredentials = false;
          connectToWiFi(wifiSSID, wifiPassword);
          wifiSSID = "";
          wifiPassword = "";
          awaitingSSID = true;
          showPrompt();
        }
        commandBuffer = "";
        cursorPosition = 0;
      }
    } else if (c == 8 || c == 127) {
      if (commandBuffer.length() > 0 && cursorPosition > 0) {
        commandBuffer.remove(cursorPosition - 1, 1);
        cursorPosition--;
        Serial.print("\b \b");
      }
    } else if (c >= 32 && c <= 126) {
      commandBuffer += c;
      cursorPosition++;
      if (awaitingSSID) {
        Serial.print(c);
      } else {
        Serial.print("*");
      }
    }
  }
}