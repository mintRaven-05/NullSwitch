#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>

void scanWiFiNetworks();
void connectToWiFi(String ssid, String password);
void disconnectFromWiFi();
void showWiFiStatus();
void handleWiFiCredentials();

#endif