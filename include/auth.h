#ifndef AUTH_H
#define AUTH_H

#include <Crypto.h>
#include <EEPROM.h>
#include <SHA256.h>

#define EEPROM_SIZE 512
#define HASH_ADDRESS 0
#define HASH_SIZE 32
#define SALT_ADDRESS 32
#define SALT_SIZE 16
#define MAGIC_ADDRESS 48
#define MAGIC_BYTES 0xDEADC0DE

void simpleHash(String password, uint8_t *output, uint8_t *salt);
void generateSalt(uint8_t *salt);
bool passwordHashExists();
void storePasswordHash(String password);
bool verifyPassword(String password);
void handlePasswordSetting();
void removePasswordHash();
void handlePasswordRemoval();
void handleAuthentication();

#endif
