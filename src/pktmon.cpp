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
