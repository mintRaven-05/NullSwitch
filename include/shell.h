#ifndef SHELL_H
#define SHELL_H

#include <Arduino.h>

// Shell constants
#define MAX_COMMAND_LENGTH 128
#define HISTORY_SIZE 10

// Function declarations
void addToHistory(String command);
void showPrompt();
void clearScreen();
void executeCommand(String command);
void handleTabCompletion();
void handleShellInput();
bool checkForAbort();

#endif
