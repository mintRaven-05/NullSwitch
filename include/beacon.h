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

extern uint8_t wifi_channels[];
extern bool enable_wpa2_mode;
extern bool pad_ssid_names;

extern char blank_ssid[MAX_SSID_LENGTH];
extern uint8_t current_channel_idx;
extern uint8_t device_mac[MAC_ADDRESS_LENGTH];
extern uint8_t active_channel;
extern uint32_t time_now;
extern uint32_t frame_size;
extern uint32_t sent_packets;
extern uint32_t last_attack;
extern uint32_t rate_check_time;

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