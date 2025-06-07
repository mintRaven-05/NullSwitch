#include <Arduino.h>
#include "../include/shell.h"
#include "../include/auth.h"
#include "../include/wifi.h"
#include "../include/netscan.h"
#include "../include/pktmon.h"
#include "../include/utils.h"
#include "../include/globals.h"
#include "../include/beacon.h"

void addToHistory(String command) {
  if (command.length() == 0)
    return;
  for (int i = HISTORY_SIZE - 1; i > 0; i--) {
    commandHistory[i] = commandHistory[i - 1];
  }

  commandHistory[0] = command;
  if (historyCount < HISTORY_SIZE) {
    historyCount++;
  }
  currentHistoryIndex = -1;
}
  
void showPrompt() {Serial.print("\n\033[32mnull@switch\033[0m:\033[34m~\033[0m$ ");}
void clearScreen() { Serial.print("\033[2J\033[H"); }

void executeCommand(String command) {
    command.trim();
    command.toLowerCase();
  
    if (command.length() == 0) {
      return;
    }
  
    addToHistory(command);
  
    if (command == "help") {
      showHelp();
    } else if (command == "meminfo") {
      Serial.println("\n\033[1;36mMemory Info\033[0m");
      Serial.println("\033[36m=======================================\033[0m");
      displayMemInfo();
      showPrompt();
      return;
    } else if (command == "restart") {
      Serial.println("\n\033[33m[!] Restarting system...\033[0m");
      delay(1000);
      ESP.restart();
    } else if (command == "passwd") {
      Serial.println("\n\033[1;31mINITIATING PASSWORD REMOVAL\033[0m");
      Serial.println("\033[31m================================================\033[0m");
      Serial.println("\033[33m[!] WARNING: This will remove the current password!\033[0m");
      Serial.println("\033[33m[!] You will need to set a new password on next boot.\033[0m");
      Serial.print("\033[33mEnter current password to confirm: \033[0m");
      awaitingPasswordRemoval = true;
      return;
    } else if (command == "clear") {
      clearScreen();
    } else if (command == "list") {
      scanWiFiNetworks();
    } else if (command == "connect") {
      Serial.println("\n\033[1;36mWiFi CONNECTION SETUP\033[0m");
      Serial.println("\033[36m================================================\033[0m");
      Serial.print("\033[33mEnter WiFi SSID: \033[0m");
      awaitingWiFiCredentials = true;
      awaitingSSID = true;
      return;
    } else if (command == "disconnect") {
      disconnectFromWiFi();
    } else if (command == "status") {
      showWiFiStatus();
    } else if (command == "netscan") {
      Serial.println("\n\033[1;36mStarting Network Scanner\033[0m");
      Serial.println(
          "\033[36m================================================\033[0m");
      networkScanActive = true;
      currentScanState = SCANNING_NETWORKS;
      networkCount = 0;
      clientCount = 0;
      scanNetworks();
      return;
    } else if (command == "pktmon") {
      Serial.println("\n\033[1;36mStarting Packet Monitor\033[0m");
      Serial.println(
          "\033[36m================================================\033[0m");
      packetMonitoringActive = true;
      getUserChoice();
      EEPROM.begin(4096);
      if (autoScan) {
        curChannel = EEPROM.read(2000);
        if (curChannel < 1 || curChannel > maxCh) {
          curChannel = 1;
          EEPROM.write(2000, curChannel);
          EEPROM.commit();
        }
      }
      wifi_set_opmode(STATION_MODE);
      wifi_promiscuous_enable(0);
      WiFi.disconnect();
      wifi_set_promiscuous_rx_cb(sniffer);
      wifi_set_channel(curChannel);
      wifi_promiscuous_enable(1);
  
      channelSwitchTimer = millis();
      displayUpdateTimer = millis();
      return;
    } else if (command == "beacon"){
        Serial.println("\n\033[1;36mStarting Beacon Attack\033[0m");
        Serial.println(
          "\033[36m================================================\033[0m");
        Serial.println("\033[32m[+]\033[0m Generating MAC addresses.");
        Serial.println("\033[32m[+]\033[0m Generating beacon frames.");
        Serial.println("\033[32m[+]\033[0m Press Ctrl+C to stop the attack.");
        beaconAttackActive = true;
        Serial.println();
        initialize_system();
        Serial.println("\033[90m[+] Preparing to send frames.\033[0m");
        delay(500);
        Serial.println("\033[90m[+] Beacon Networks: \033[0m");
        Serial.println();
        String networks[50];
        int network_count = 0;
        int idx = 0;
        int total_len = sizeof(network_list)/sizeof(network_list[0]);
        String current_network = "";
  
        while (idx < total_len) {
          char c = (char)pgm_read_byte(network_list + idx);
          if (c == '\n') {
            if (current_network.length() > 0) {
              networks[network_count] = current_network;
              network_count++;
              current_network = "";
            }
          } else {
            current_network += c;
          }
          idx++;
        }
      
        int half = (network_count + 1) / 2;
        for (int i = 0; i < half; i++) {
          Serial.print(networks[i]);
  
          for (int j = networks[i].length(); j < 30; j++) {
            Serial.print(" ");
          }
        
          if (i + half < network_count) {
            Serial.print(networks[i + half]);
          }
          Serial.println();
        }
      
        Serial.println();
        Serial.println("\033[32m[+] System initialized successfully!\033[0m");
        Serial.println();
        return;
    } else {
      Serial.println("\n\033[31mUnknown command: \033[0m" + command);
      Serial.println("\033[90mType 'help' for available commands.\033[0m");
    }
  
    showPrompt();
  }
  
