#ifndef NETSCAN_H
#define NETSCAN_H

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <user_interface.h>
#include "config.h"

struct NetworkInfo {
    String ssid;
    String bssid;
    uint8_t bssidBytes[6];
    int32_t rssi;
    String security;
    int channel;
  };
  
  struct ClientInfo {
    uint8_t mac[6];
    int8_t rssi;
    uint32_t lastSeen;
    uint32_t packetCount;
    bool active;
    String networkSSID;
    String deviceType;
    String macOUI;
  };
  
  typedef struct {
    signed rssi : 8;
    unsigned rate : 4;
    unsigned is_group : 1;
    unsigned : 1;
    unsigned sig_mode : 2;
    unsigned legacy_length : 12;
    unsigned damatch0 : 1;
    unsigned damatch1 : 1;
    unsigned bssidmatch0 : 1;
    unsigned bssidmatch1 : 1;
    unsigned MCS : 7;
    unsigned CWB : 1;
    unsigned HT_length : 16;
    unsigned Smoothing : 1;
    unsigned Not_Sounding : 1;
    unsigned : 1;
    unsigned Aggregation : 1;
    unsigned STBC : 2;
    unsigned FEC_CODING : 1;
    unsigned SGI : 1;
    unsigned rxend_state : 8;
    unsigned ampdu_cnt : 8;
    unsigned channel : 4;
    unsigned : 12;
  } wifi_pkt_rx_ctrl_t;
  
  typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8_t buf[0];
  } wifi_pkt_t;
  
  struct ieee80211_hdr {
    uint16_t frame_ctrl;
    uint16_t duration_id;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t seq_ctrl;
  } __attribute__((packed));

#define WIFI_PKT_MGMT 0x00
#define WIFI_PKT_CTRL 0x01
#define WIFI_PKT_DATA 0x02

void scanNetworks();
void displayNetworks();
void waitForUserInput();
void startClientScanning();
void clientScanningLoop();
bool compareMac(const uint8_t *mac1, const uint8_t *mac2);
bool isBroadcastMac(const uint8_t *mac);
bool isValidClientMac(const uint8_t *mac);
void printMac(const uint8_t *mac);
void addOrUpdateClient(const uint8_t *clientMac, int8_t rssi);
void promiscuousCallback(uint8_t *buf, uint16_t len);
String getSecurityType(int encType);
String getSignalStrength(int rssi);
void parseBSSID(String bssidStr, uint8_t *bssidBytes);
String getMacOUI(const uint8_t *mac);
String guessDeviceType(const uint8_t *mac);
String generateDeviceID(const uint8_t *mac);
void clearLines(int count);
void updateClientDisplay();
void handleSerialInput();

#endif