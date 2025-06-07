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

#define MAX_SSID_LENGTH 32
#define BEACON_FRAME_SIZE 109
#define MAC_ADDRESS_LENGTH 6
#define ATTACK_INTERVAL 100
#define RATE_CHECK_INTERVAL 1000
#define TRANSMISSION_REPEATS 3

void switch_channel();
void generate_random_mac();
void initialize_system();
void transmit_beacon_frames();
void display_transmission_rate();
void setup_beacon_frame();
void process_network_list();
void send_single_beacon(const char *ssid_name, uint8_t ssid_length);
void stopBeaconAttack();

#endif // WIFI_BEACON_H