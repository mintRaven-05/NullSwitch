#include <Arduino.h>
#include "../include/utils.h"
#include "../include/auth.h"
#include "../include/shell.h"
#include "../include/wifi.h"
#include "../include/netscan.h"
#include "../include/pktmon.h"
#include "../include/globals.h"
#include "../include/beacon.h"


void setup() {
  Serial.begin(115200);
  Serial.println();

  delay(2000);

  for (int i = 0; i < 3; i++) {
    while (Serial.available()) {
      Serial.read();
    }
    delay(100);
  }

  Serial.println();
  Serial.println();
  Serial.println("\033[1;32m=== SERIAL COMMUNICATION ESTABLISHED ===\033[0m");
  Serial.println("\033[33mSystem initializing...\033[0m");
  delay(500);
}
void loop() {
  yield();

  if (!authenticated) {
    handleAuthentication();
    if (settingPassword) {
      handlePasswordSetting();
    }
    return;
  }

  else if (awaitingPasswordRemoval) {
    handlePasswordRemoval();
    return;
  }

  else if (awaitingWiFiCredentials) {
    handleWiFiCredentials();
    return;
  }

  else if (networkScanActive) {
    if (checkForAbort()) {
      Serial.println();
      Serial.println("\033[33m[!] Scan aborted.\033[0m");
      networkScanActive = false;
      wifi_promiscuous_enable(0);
      WiFi.scanDelete();
      showPrompt();
      return;
    }

    switch (currentScanState) {
    case WAITING_FOR_INPUT:
      waitForUserInput();
      break;
    case SCANNING_CLIENTS:
      clientScanningLoop();
      break;
    default:
      break;
    }
    return;
  }

  else if (packetMonitoringActive) {
    if (checkForAbort()) {
      if (autoScan) {
        clearPKTLines(8);
      } else {
        clearPKTLines(9);
      }

      Serial.print("\033[J");
      Serial.println("\033[33m[!] Monitoring aborted.\033[0m");
      packetMonitoringActive = false;
      wifi_set_promiscuous_rx_cb(NULL);
      wifi_set_opmode(STATION_MODE);
      wifi_promiscuous_enable(0);
      WiFi.scanDelete();
      Serial.println("\033[?25h");
      showPrompt();
      return;
    }
    curTime = millis();

    if (autoScan && curTime - channelSwitchTimer >= channelSwitchTime) {
      channelSwitchTimer = curTime;
      switchChannel();
    }

    if (curTime - displayUpdateTimer >= 1000) {
      displayUpdateTimer = curTime;
      updateDisplay();
    }

    if (curTime - prevTime >= 1000) {
      prevTime = curTime;

      for (int i = 0; i < maxRow - 1; i++) {
        val[i] = val[i + 1];
      }
      val[maxRow - 1] = pkts;

      if (pkts == 0)
        pkts = deauths;
      no_deauths = pkts - deauths;


      deauths = 0;
      pkts = 0;
    }
  } else if (beaconAttackActive){
    if (checkForAbort()) {
      Serial.println();
      Serial.println("\033[33m[!] Beacon attack aborted.\033[0m");
      beaconAttackActive = false;
      stopBeaconAttack();
      showPrompt();
      return;
    }
    
    time_now = millis();

    transmit_beacon_frames();
    display_transmission_rate();
  
  } else {
    handleShellInput();
  }
  delay(10);
}