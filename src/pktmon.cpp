#include <Arduino.h>
#include "../include/pktmon.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/globals.h"
#include "../include/config.h"

void sniffer(uint8_t *buf, uint16_t len) {
  pkts++;
  totalPackets++;

  if (buf[12] == 0xA0 || buf[12] == 0xC0) {
    deauths++;
    totalDeauths++;
  }
}

String readLineWithEcho() {
  String input = "";
  char c;

  while (true) {
    if (Serial.available()) {
      c = Serial.read();
      if (c == 3) {
        Serial.println();
        Serial.println("\033[33m[!] Packet monitoring aborted.\033[0m");
        packetMonitoringActive = false;
        wifi_set_promiscuous_rx_cb(NULL);
        wifi_set_opmode(STATION_MODE);
        wifi_promiscuous_enable(
            0);
        WiFi.scanDelete();
        Serial.println("\033[?25h");
        showPrompt();
        return "";
      }

      if (c == '\n' || c == '\r') {
        Serial.println();
        break;
      } else if (c == 8 || c == 127) {
        if (input.length() > 0) {
          input.remove(input.length() - 1);
          Serial.print("\b \b");
        }
      } else if (c >= 32 && c <= 126) {
        input += c;
        Serial.print(c);
      }
    }
    delay(10);
  }

  return input;
}

void getUserChoice() {
  Serial.println("Choose monitoring mode:");
  Serial.println("1. Auto scan all channels (1-14)");
  Serial.println("2. Lock to specific channel");
  Serial.println();
  Serial.print("\033[33m[?]\033[0m Enter choice \033[90m(1 or 2)\033[0m: ");

  String choice = readLineWithEcho();
  choice.trim();

  if (choice == "1") {
    autoScan = true;
  } else if (choice == "2") {
    autoScan = false;
    Serial.print(
        "\033[33m[?]\033[0m Enter channel number \033[90m(1-14)\033[0m: ");

    String chStr = readLineWithEcho();
    chStr.trim();
    int ch = chStr.toInt();

    if (ch >= 1 && ch <= maxCh) {
      PKTtargetChannel = ch;
      curChannel = ch;
    } else if (ch > 14) {
      Serial.println("\033[33m[!] Invalid channel!\033[0m");
      packetMonitoringActive = false;
      showPrompt();
      return;
    }
  } else {
    packetMonitoringActive = false;
    Serial.println("\033[33m[!] Invalid selection!\033[0m");
    showPrompt();
    return;
  }

  delay(2000);

  monitoringStarted = true;
}

void clearEntireScreen() {
  Serial.print("\033[2J");
  Serial.print("\033[H");
}

void clearPKTLines(int count) {
  Serial.print("\033[?25l");
  Serial.print("\033[" + String(count) + "A");
}

void updateDisplay() {
  if (!monitoringStarted)
    return;

  Serial.println();

  Serial.print("\033[90mChannel: ");
  Serial.print(curChannel < 10 ? " " : "");
  Serial.print(curChannel);
  Serial.print("   │   Normal: ");
  Serial.print(no_deauths < 10 ? "  " : (no_deauths < 100 ? " " : ""));
  Serial.print(no_deauths);
  Serial.print("   │   Deauth: ");
  Serial.print(deauths < 10 ? "  " : (deauths < 100 ? " " : ""));
  Serial.print(deauths);
  Serial.print("   │   Total/s: ");
  Serial.print(pkts < 10 ? "  " : (pkts < 100 ? " " : ""));
  Serial.print(pkts);
  Serial.println("\033[0m          ");
  Serial.println();
  Serial.print("\033[32m[+] Mode: ");
  if (autoScan) {
    Serial.print("Auto Scanning                                                "
                 "        ");
  } else {
    Serial.print("Locked on Channel ");
    Serial.print(PKTtargetChannel);
    Serial.print("\033[0m                                                   ");
  }
  Serial.println();
  // Attack status
  Serial.print("\033[32m[~]\033[0m Status:  ");
  if (deauths > packetRate) {
    Serial.println("\033[33mDEAUTH ATTACK DETECTED!\033[0m                     "
                   "            ");
  } else {
    Serial.println("\033[32m✓ Normal Operation\033[0m                          "
                   "                  ");
  }

  Serial.print("\033[33mSession: ");
  Serial.print(totalPackets);
  Serial.print(" packets, ");
  Serial.print(totalDeauths);
  Serial.print(" deauths\033[0m");
  Serial.println("                                        ");
  Serial.println();
  Serial.print("Activity: ");

  maxVal = 1;
  int startIdx = maxRow - graphWidth;
  if (startIdx < 0)
    startIdx = 0;

  for (int i = startIdx; i < maxRow; i++) {
    if (val[i] > maxVal)
      maxVal = val[i];
  }

  for (int i = startIdx; i < maxRow && (i - startIdx) < graphWidth; i++) {
    int barHeight = (val[i] * 8) / (maxVal > 0 ? maxVal : 1);

    if (val[i] == 0) {
      Serial.print("·");
    } else if (barHeight <= 1) {
      Serial.print("▁");
    } else if (barHeight <= 2) {
      Serial.print("▂");
    } else if (barHeight <= 3) {
      Serial.print("▃");
    } else if (barHeight <= 4) {
      Serial.print("▄");
    } else if (barHeight <= 5) {
      Serial.print("▅");
    } else if (barHeight <= 6) {
      Serial.print("▆");
    } else if (barHeight <= 7) {
      Serial.print("▇");
    } else {
      Serial.print("█");
    }
  }
  Serial.println("                                                ");

  if (autoScan) {
    Serial.println();
    Serial.print("Progress: ");
    for (int i = 1; i <= maxCh; i++) {
      if (i == curChannel) {
        Serial.print("█");
      } else {
        Serial.print("\033[90m▒\033[0m");
      }
    }
    Serial.print(" (");
    Serial.print(curChannel);
    Serial.print("/");
    Serial.print(maxCh);
    Serial.println(")                                                 ");
  } else {
    Serial.println();
    Serial.print("Locked:   ");
    for (int i = 1; i <= maxCh; i++) {
      if (i == PKTtargetChannel) {
        Serial.print("█");
      } else {
        Serial.print("\033[90m▒\033[0m");
      }
    }
    Serial.print(" (Channel ");
    Serial.print(PKTtargetChannel);
    Serial.println(" only)                                          ");
  }

  Serial.println();

  Serial.print("\033[90mLive: ");
  static int spinner = 0;
  char spinChars[] = {'|', '/', '-', '\\'};
  Serial.print(spinChars[spinner % 4]);
  spinner++;

  if (autoScan) {
    Serial.print("  Next channel in: ");
    unsigned long timeLeft =
        channelSwitchTime - (millis() - channelSwitchTimer);
    Serial.print(timeLeft / 1000);
    Serial.println("s                                               ");
  } else {
    Serial.print("  Monitoring channel ");
    Serial.print(PKTtargetChannel);
    Serial.println(" continuously\033[0m                                 ");
  }
  clearPKTLines(13);
  Serial.println();
}

void switchChannel() {
  curChannel++;
  if (curChannel > maxCh)
    curChannel = 1;

  wifi_set_channel(curChannel);

  // Save current channel to EEPROM less frequently to avoid wear
  static int channelSaveCounter = 0;
  if (++channelSaveCounter >= 10) {
    EEPROM.write(2000, curChannel);
    EEPROM.commit();
    channelSaveCounter = 0;
  }
}