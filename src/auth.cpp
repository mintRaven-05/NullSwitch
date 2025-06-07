#include <Arduino.h>
#include "../include/auth.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/globals.h"

void _generate_hash(String password, uint8_t *output, uint8_t *salt) {
  SHA256 hasher;
  uint8_t tempHash[32];

  uint8_t *passwordBytes = (uint8_t *)password.c_str();
  int passwordLen = password.length();

  hasher.reset();
  hasher.update(passwordBytes, passwordLen);
  hasher.update(DEVICE_SALT, 16);
  hasher.update(salt, 16);
  hasher.finalize(tempHash, 32);

  for (int i = 0; i < 1000; i++) {
    hasher.reset();
    hasher.update(tempHash, 32);
    hasher.update(salt, 16);
    hasher.update((uint8_t *)&i, sizeof(i));
    hasher.finalize(tempHash, 32);
  }

  hasher.reset();
  hasher.update(tempHash, 32);
  hasher.update(DEVICE_SALT, 16);
  hasher.finalize(output, 32);
}

void generateSalt(uint8_t *salt) {
    for (int i = 0; i < 16; i++) {
      salt[i] = (uint8_t)RANDOM_REG32;
    }

    uint32_t chipId = ESP.getChipId();
    uint32_t time = millis();
  
    for (int i = 0; i < 4; i++) {
      salt[i] ^= (chipId >> (i * 8)) & 0xFF;
      salt[i + 4] ^= (time >> (i * 8)) & 0xFF;
    }
  }