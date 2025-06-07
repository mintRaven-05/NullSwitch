#include <Arduino.h>
#include "../include/utils.h"
#include "../include/shell.h"
#include "../include/wifi.h"
#include "../include/auth.h"
#include "../include/globals.h"

bool checkForAbort() {

  while (Serial.available()) {
    int c = Serial.peek();
    if (c == 3) {
      Serial.read();
      return true;
    }
    break;
  }
  return false;
}

void refreshLine() {
  Serial.print("\r\033[K");
  Serial.print("\033[32mnull@switch\033[0m:\033[34m~\033[0m$ ");
  Serial.print(commandBuffer);

  int moveBack = commandBuffer.length() - cursorPosition;
  for (int i = 0; i < moveBack; i++) {
    Serial.print("\b");
  }
}

void showHelp() {
  Serial.println(
      "\n\033[1;36m================================================\033[0m");
  Serial.println(
      "\033[1;36m                 AVAILABLE COMMANDS             \033[0m");
  Serial.println(
      "\033[1;36m================================================\033[0m");
  Serial.println("\033[1;35mTerminal Commands:\033[0m");
  Serial.println("\033[33mhelp\033[0m       - Display this help message");
  Serial.println("\033[33mmeminfo\033[0m    - Display memory information");
  Serial.println("\033[33mrestart\033[0m    - Restart the system");
  Serial.println("\033[33mpasswd\033[0m     - Remove the current password");
  Serial.println("\033[33mclear\033[0m      - Clear the terminal screen");
  Serial.println("");
  Serial.println("\033[1;35mWiFi Commands:\033[0m");
  Serial.println("\033[33mconnect\033[0m    - Connect to WiFi network");
  Serial.println("\033[33mdisconnect\033[0m - Disconnect from WiFi");
  Serial.println(
      "\033[33mlist\033[0m       - Scan for available WiFi networks");
  Serial.println("\033[33mstatus\033[0m     - Check WiFi connection status");
  Serial.println("");
  Serial.println("\033[1;35mController Commands:\033[0m");
  Serial.println(
      "\033[33mnetscan\033[0m    - Scan for networks and monitor clients");
  Serial.println(
      "\033[33mpktmon\033[0m     - Monitor packets accress all channels");
  Serial.println(
      "\033[33mbeacon\033[0m     - Start beacon attack on networks");
  Serial.println("");
  Serial.println("\033[1;32mShell Features:\033[0m");
  Serial.println("• Use \033[33m↑/↓\033[0m arrow keys for command history");
  Serial.println("• Use \033[33m←/→\033[0m arrow keys for cursor navigation");
  Serial.println("• Use \033[33mTab\033[0m for command completion (basic)");
  Serial.println(
      "\033[1;36m================================================\033[0m");
}
void displayMemInfo(){
  Serial.print("\033[36m   Chip ID:       \033[0m0x");
  Serial.println(ESP.getChipId(), HEX);
  Serial.print("\033[36m   Core Version:  \033[0m");
  Serial.println(ESP.getCoreVersion());
  Serial.print("\033[36m   SDK Version:   \033[0m");
  Serial.println(ESP.getSdkVersion());
  Serial.print("\033[36m   CPU Frequency: \033[0m");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("\033[36m   Flash Chip:    \033[0m0x");
  Serial.println(ESP.getFlashChipId(), HEX);
  Serial.print("\033[36m   Flash Size:    \033[0m");
  Serial.print(ESP.getFlashChipRealSize());
  Serial.print(" bytes (");
  Serial.print(ESP.getFlashChipRealSize() / 1024.0 / 1024.0);
  Serial.println(" MB)");
  Serial.print("\033[36m   Flash Speed:   \033[0m");
  Serial.print(ESP.getFlashChipSpeed());
  Serial.println(" Hz");
  Serial.println();
  delay(1000);
  uint32_t sketchSize = ESP.getSketchSize();
  uint32_t freeSketchSpace = ESP.getFreeSketchSpace();

  if (SPIFFS.begin()) {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    Serial.print("\033[36m   SPIFFS Total:  \033[0m");
    Serial.print(fs_info.totalBytes);
    Serial.print(" bytes (");
    Serial.print(fs_info.totalBytes / 1024.0);
    Serial.println(" KB)");
    Serial.print("\033[36m   SPIFFS Used:   \033[0m");
    Serial.print(fs_info.usedBytes);
    Serial.print(" bytes (");
    Serial.print(fs_info.usedBytes / 1024.0);
    Serial.println(" KB)");
    Serial.print("\033[36m   SPIFFS Free:   \033[0m");
    Serial.print(fs_info.totalBytes - fs_info.usedBytes);
    Serial.print(" bytes (");
    Serial.print((fs_info.totalBytes - fs_info.usedBytes) / 1024.0);
    Serial.println(" KB)");
    Serial.print("\033[36m   Usage:         \033[0m");
    Serial.print((fs_info.usedBytes * 100.0) / fs_info.totalBytes);
    Serial.println("%");
    Serial.print("\033[36m   Block Size:    \033[0m");
    Serial.print(fs_info.blockSize);
    Serial.println(" bytes");
    Serial.print("\033[36m   Page Size:     \033[0m");
    Serial.print(fs_info.pageSize);
    Serial.println(" bytes");
  } else {
    Serial.println("\033[31m   SPIFFS not available\033[0m");
  }
  Serial.print("\033[36m   MAC Address:   \033[0m");
  Serial.println(WiFi.macAddress());
  Serial.println();
}

void displayBanner(){
  Serial.println("\033[90m⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠤⠠⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠴⣧⣿⣿⣿⣿⣽⣶⡤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣳⣟⣿⣟⣿⣻⣿⣿⣿⣿⣾⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣻⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣷⢸⣛⠟⠻⣛⡟⢛⣟⠟⠻⣛⡇⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠘⣺⠀⠀⠀⠀⢦⠼⠁⠀⠀⠀⢳⠁⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠸⣀⠗⠒⢒⠀⠀⠀⠀⢰⣟⣄⣀⣠⣼⠋⠙⣗⣠⣀⣠⣸⡆⠀⠀⠀⠀⡖⠒⠺⣀⠇⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠈⠛⠛⠻⡯⢯⣀⣄⣻⢹⠟⢛⠋⠁⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠁⠀⢞⡀⡿⣯⣾⢻⢿⢸⠀⡿⠀⠈⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠉⠉⠉⠉⠁⠀⢸⣦⣒⡤⣭⢨⢨⢰⣴⡇⠀⠈⠉⠉⠉⠉⡅⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⢰⠉⡦⠤⠬⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿⡽⠋⠀⠀⠀⠀⠀⠀⠀⠧⠤⠤⡎⢱⠀⠀⠀⠀⠀⠀");
  Serial.println("⠀⠀⠀⠀⠀⠀⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠁⠀⠀⠀⠀⠀⠀\033[0m");
  Serial.println("⣶⡀⠀⣶⠀⣶⠀⢰⡆⣶⡆⠀⠀⣶⡆⠀⠀⢠⣶⣶⣦⠀⣶⠀⠀⢰⡆⣶⡆⣶⣶⣶⣶⡆⣠⣶⣶⣄⢰⡆⠀⢰⡆");
  Serial.println("⣿⣷⠀⣿⠀⣿⠀⢸⡇⣿⡇⠀⠀⣿⡇⠀⠀⢸⣇⣀⠉⠀⣿⠀⡀⢸⡇⣿⡇⠀⢸⣿⠀⠀⣿⠃⠘⠛⢸⣇⣀⣸⡇");
  Serial.println("⣿⠹⣧⣿⠀⣿⠀⢸⡇⣿⡇⠀⠀⣿⡇⠀⠀⢈⡉⠻⣷⠀⣿⣼⢿⣼⡇⣿⡇⠀⢸⣿⠀⠀⣿⡀⠀⠀⢸⡏⠉⢹⡇");  
  Serial.println("⣿⠀⠙⣿⠀⢿⣀⣼⠇⣿⣷⣶⡆⣿⣷⣶⣶⠸⣷⣶⣿⠀⡿⠁⠀⠙⡇⣿⡇⠀⢸⣿⠀⠀⠻⣿⣾⠟⢸⡇⠀⢸⡇");
  
  Serial.println();
  Serial.println("\033[1;36mVersion 1.0.0\033[0m");
  Serial.println("\033[1;33m~mintRaven Copyright (c) 2025\033[0m");
  Serial.println();
}

void displayBootInfo() {
  Serial.println();
  Serial.println(
      "\033[1;32m================================================\033[0m");
  Serial.println(
      "\033[1;32m              Booting NullSwitch               \033[0m");
  Serial.println(
      "\033[1;32m================================================\033[0m");
  displayMemInfo();
  delay(1000);
  Serial.println();
  displayBanner();
  bootComplete = true;
  showPrompt();
}