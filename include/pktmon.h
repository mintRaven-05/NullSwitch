#ifndef PKTMON_H
#define PKTMON_H

#include <EEPROM.h>
#include <ESP8266WiFi.h>
extern "C" {
    #include "user_interface.h"
}
#define maxCh 14
#define packetRate 5
#define channelSwitchTime 3000
#define graphWidth 30


void sniffer(uint8_t *buf, uint16_t len);
String readLineWithEcho();
void getUserChoice();
void clearEntireScreen();
void clearPKTLines(int count);
void updateDisplay();
void switchChannel();

#endif