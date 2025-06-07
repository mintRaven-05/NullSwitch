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


