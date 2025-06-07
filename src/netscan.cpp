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

void displayNetworks() {
  Serial.println("AVAILABLE NETWORKS:");
  Serial.println("============================================================="
                 "================================");

  Serial.print("Sl.  SSID               BSSID              Channel   RSSI    "
               "Signal          Security");
  Serial.println();
  Serial.println("------------------------------------------------------"
                 "---------------------------------------");

  for (int i = 0; i < networkCount; i++) {
    if (checkForAbort()) {
      Serial.println();
      Serial.println("\033[33mNetwork scan aborted.\033[0m");
      networkScanActive = false;
      showPrompt();
      return;
    }

    String signalStrength = getSignalStrength(networks[i].rssi);
    String securityIcon = (networks[i].security == "Open") ? "[O]" : "[L]";

    Serial.print(i + 1);
    if (i + 1 < 10)
      Serial.print("    ");
    else if (i + 1 < 100)
      Serial.print("   ");
    else
      Serial.print("  ");

    String ssid = networks[i].ssid;
    if (ssid.length() > 14)
      ssid = ssid.substring(0, 17) + "...";
    Serial.print(ssid);
    for (int j = ssid.length(); j < 14; j++)
      Serial.print(" ");

    Serial.print(networks[i].bssid);
    Serial.print("         ");

    Serial.print(networks[i].channel);
    if (networks[i].channel < 10)
      Serial.print("       ");
    else
      Serial.print("      ");

    Serial.print(networks[i].rssi);
    if (networks[i].rssi > -10)
      Serial.print("       ");
    else if (networks[i].rssi > -100)
      Serial.print("      ");
    else
      Serial.print("     ");

    Serial.print(signalStrength);
    for (int j = signalStrength.length(); j < 12; j++)
      Serial.print(" ");

    Serial.print(securityIcon);
    Serial.print(" ");
    Serial.println(networks[i].security);
  }
  Serial.println("------------------------------------------------------"
                 "---------------------------------------");
  Serial.println("\n\033[32m[+]\033[0m Enter the serial number of the network "
                 "to scan for active devices");
  Serial.println("\033[32m[+]\033[0m Press Ctrl+C to return to shell:");
  Serial.print("\033[33m[?]\033[0m Selection (1-");
  Serial.print(networkCount);
  Serial.print("): ");

  currentScanState = WAITING_FOR_INPUT;
}

void startClientScanning() {
  Serial.println();
  Serial.println("\033[32m[+] Starting client discovery...\033[0m");
  Serial.println("================================================");
  Serial.print("\033[32m[INFO] Target BSSID: \033[0m");
  printMac(targetBSSID);
  Serial.println();
  Serial.println("\033[32m[INFO] Target Channel: \033[0m" +
                 String(targetChannel));
  Serial.println("\033[32m[INFO] Target SSID: \033[0m" + targetSSID);

  clientCount = 0;

  headerPrinted = false;
  lastActiveCount = 0;

  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(targetChannel);

  wifi_set_promiscuous_rx_cb(promiscuousCallback);
  wifi_promiscuous_enable(1);

  Serial.println("\n\033[32m[+]\033[0m Packet capture started...");
  Serial.println("\033[32m[+]\033[0m Press Ctrl+C to stop");
  Serial.println("\033[32m[+]\033[0m Looking for devices connected to: " +
                 targetSSID);

  currentScanState = SCANNING_CLIENTS;
}

void waitForUserInput() {
  while (Serial.available()) {
    char c = Serial.read();

    // Handle Ctrl+C
    if (c == 3) {
      Serial.println();
      Serial.println("\033[33m[!] Network scan aborted.\033[0m");
      networkScanActive = false;
      showPrompt();
      return;
    }

    if (c == '\n' || c == '\r') {
      if (commandBuffer.length() > 0) {
        int selection = commandBuffer.toInt();
        if (selection >= 1 && selection <= networkCount) {
          int index = selection - 1;
          memcpy(targetBSSID, networks[index].bssidBytes, 6);
          targetChannel = networks[index].channel;
          targetSSID = networks[index].ssid;
          startClientScanning();
        } else {
          Serial.println("\n\033[33m[!] Invalid selection. Please enter a "
                         "number between 1 and " +
                         String(networkCount));
          Serial.print("[?]\033[0m Selection (1-");
          Serial.print(networkCount);
          Serial.print("): ");
        }
        commandBuffer = "";
      }
    } else if (c >= '0' && c <= '9') {
      commandBuffer += c;
      Serial.print(c);
    } else if (c == 8 || c == 127) { // Backspace
      if (commandBuffer.length() > 0) {
        commandBuffer.remove(commandBuffer.length() - 1);
        Serial.print("\b \b");
      }
    }
  }
}