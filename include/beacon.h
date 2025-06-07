#ifndef WIFI_BEACON_H
#define WIFI_BEACON_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

extern "C" {
#include "user_interface.h"
typedef void (*packet_send_cb_t)(uint8 status);
int wifi_register_send_pkt_freedom_cb(packet_send_cb_t cb);
void wifi_unregister_send_pkt_freedom_cb(void);
int wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

#endif // WIFI_BEACON_H