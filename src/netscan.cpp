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
    } else if (c == 8 || c == 127) {
      if (commandBuffer.length() > 0) {
        commandBuffer.remove(commandBuffer.length() - 1);
        Serial.print("\b \b");
      }
    }
  }
}

void updateClientDisplay() {
  uint32_t currentTime = millis();
  int activeCount = 0;
  for (int i = 0; i < clientCount; i++) {
    if (clients[i].active) {
      activeCount++;
    }
  }

  if (!headerPrinted) {
    Serial.println(
        "================\033[33m ACTIVE CLIENTS \033[0m================");
    Serial.println("Network: " + targetSSID);
    Serial.print("BSSID: ");
    printMac(targetBSSID);
    Serial.println();
    Serial.println("Channel: " + String(targetChannel));
    Serial.println("\033[32m[+] Switching scan modes\033[0m");
    Serial.println("\033[32m[+] Started Capturing Packets....\033[0m");
    Serial.println();
    Serial.println(
        "======================================================================"
        "===========================");
    headerPrinted = true;
  } else {
    int linesToClear = 0;
    if (lastActiveCount > 0) {
      linesToClear = lastActiveCount +
                     4;
    } else {
      linesToClear = 5;
    }
    clearLines(linesToClear);
  }

  if (activeCount == 0) {
    Serial.println("\033[90m[~] Refreshing to detect active users");
    Serial.println("Troubleshooting tips:");
    Serial.println("1. Verify the selected network is correct");
    Serial.println("2. Try generating traffic on connected devices");
    Serial.println("3. Check if devices are in power-saving mode\033[0m");
  } else {
    Serial.println(
        "No.  MAC Address              Manufacturer        "
        "RSSI       Signal       Packets      Last Seen");
    Serial.println(
        "----------------------------------------------------------------------"
        "---------------------------");

    int displayedCount = 0;
    for (int i = 0; i < clientCount; i++) {
      if (clients[i].active) {
        displayedCount++;
        Serial.print(displayedCount);
        if (displayedCount < 10)
          Serial.print("    ");
        else
          Serial.print("   ");

        printMac(clients[i].mac);
        Serial.print("        ");

        String manufacturer = clients[i].macOUI;
        if (manufacturer.length() > 22)
          manufacturer = manufacturer.substring(0, 19) + "...";
        Serial.print(manufacturer);
        for (int j = manufacturer.length(); j < 20; j++)
          Serial.print(" ");
        Serial.print(clients[i].rssi);
        if (clients[i].rssi > -10)
          Serial.print("          ");
        else if (clients[i].rssi > -100)
          Serial.print("         ");
        else
          Serial.print("        ");

        String signalStrength = getSignalStrength(clients[i].rssi);
        Serial.print(signalStrength);
        for (int j = signalStrength.length(); j < 13; j++)
          Serial.print(" ");

        Serial.print(clients[i].packetCount);
        if (clients[i].packetCount < 10)
          Serial.print("           ");
        else if (clients[i].packetCount < 100)
          Serial.print("          ");
        else if (clients[i].packetCount < 1000)
          Serial.print("         ");
        else if (clients[i].packetCount < 10000)
          Serial.print("        ");
        else
          Serial.print("       ");

        Serial.print(String((currentTime - clients[i].lastSeen) / 1000));
        Serial.println("s ago");
      }
    }
    Serial.println(
        "----------------------------------------------------------------------"
        "---------------------------");
    Serial.println("\033[90m[+] Total active clients: " + String(activeCount) +
                   "\033[0m");
  }
  lastActiveCount = activeCount;
}

void clientScanningLoop() {
  uint32_t currentTime = millis();

  for (int i = 0; i < clientCount; i++) {
    if (currentTime - clients[i].lastSeen > 60000) {
      clients[i].active = false;
    }
  }

  static unsigned long lastUpdate = 0;
  if (currentTime - lastUpdate > 5000) {
    updateClientDisplay();
    lastUpdate = currentTime;
  }
}

void addOrUpdateClient(const uint8_t *clientMac, int8_t rssi) {
  if (!isValidClientMac(clientMac))
    return;
  if (compareMac(clientMac, targetBSSID))
    return;

  for (int i = 0; i < clientCount; i++) {
    if (compareMac(clients[i].mac, clientMac)) {
      clients[i].rssi = rssi;
      clients[i].lastSeen = millis();
      clients[i].packetCount++;
      clients[i].active = true;
      return;
    }
  }

  if (clientCount < 50) {
    memcpy(clients[clientCount].mac, clientMac, 6);
    clients[clientCount].rssi = rssi;
    clients[clientCount].lastSeen = millis();
    clients[clientCount].packetCount = 1;
    clients[clientCount].active = true;
    clients[clientCount].networkSSID = targetSSID;
    clients[clientCount].macOUI = getMacOUI(clientMac);
    clientCount++;
  }
}

void ICACHE_RAM_ATTR promiscuousCallback(uint8_t *buf, uint16_t len) {
  if (len < sizeof(wifi_pkt_rx_ctrl_t))
    return;

  wifi_pkt_t *pkt = (wifi_pkt_t *)buf;
  wifi_pkt_rx_ctrl_t *ctrl = &pkt->rx_ctrl;

  if (len < sizeof(wifi_pkt_rx_ctrl_t) + sizeof(ieee80211_hdr))
    return;

  ieee80211_hdr *hdr = (ieee80211_hdr *)(buf + sizeof(wifi_pkt_rx_ctrl_t));
  uint8_t frameType = (hdr->frame_ctrl & 0x0C) >> 2;
  uint8_t frameSubType = (hdr->frame_ctrl & 0xF0) >> 4;

  // totalPackets++;
  bool isRelevant = false;
  const uint8_t *clientMac = nullptr;

  switch (frameType) {
  case WIFI_PKT_DATA:
    if (compareMac(hdr->addr1, targetBSSID)) {
      clientMac = hdr->addr2;
      isRelevant = true;
    } else if (compareMac(hdr->addr2, targetBSSID)) {
      clientMac = hdr->addr1;
      isRelevant = true;
    } else if (compareMac(hdr->addr3, targetBSSID)) {
      if (!compareMac(hdr->addr1, targetBSSID)) {
        clientMac = hdr->addr1;
        isRelevant = true;
      } else if (!compareMac(hdr->addr2, targetBSSID)) {
        clientMac = hdr->addr2;
        isRelevant = true;
      }
    }
    break;

  case WIFI_PKT_MGMT:
    if (frameSubType == 0 || frameSubType == 1 || 
        frameSubType == 2 || frameSubType == 3 || 
        frameSubType == 4 || frameSubType == 5 || 
        frameSubType == 11) {                     
      if (compareMac(hdr->addr3, targetBSSID) ||
          compareMac(hdr->addr1, targetBSSID) ||
          compareMac(hdr->addr2, targetBSSID)) {
        if (!compareMac(hdr->addr1, targetBSSID)) {
          clientMac = hdr->addr1;
          isRelevant = true;
        } else if (!compareMac(hdr->addr2, targetBSSID)) {
          clientMac = hdr->addr2;
          isRelevant = true;
        }
      }
    }
    break;
  }

  if (isRelevant && clientMac) {
    // relevantPackets++;
    addOrUpdateClient(clientMac, ctrl->rssi);
  }
}
