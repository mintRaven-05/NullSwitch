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

bool passwordHashExists() {
  EEPROM.begin(EEPROM_SIZE);
  uint32_t magic;
  EEPROM.get(MAGIC_ADDRESS, magic);
  EEPROM.end();
  return (magic == MAGIC_BYTES);
}

void storePasswordHash(String password) {
  uint8_t hash[32];
  uint8_t salt[16];
  uint32_t magic = MAGIC_BYTES;

  generateSalt(salt);
  _generate_hash(password, hash, salt);

  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; i++) {
    EEPROM.write(HASH_ADDRESS + i, hash[i]);
  }
  for (int i = 0; i < 16; i++) {
    EEPROM.write(SALT_ADDRESS + i, salt[i]);
  }

  EEPROM.put(MAGIC_ADDRESS, magic);
  EEPROM.commit();
  EEPROM.end();

  Serial.println("\033[32m[+] Password hash stored successfully!\033[0m");
}

bool verifyPassword(String password) {
  if (!passwordHashExists()) {
    return false;
  }

  uint8_t storedHash[32];
  uint8_t storedSalt[16];
  uint8_t computedHash[32];
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; i++) {
    storedHash[i] = EEPROM.read(HASH_ADDRESS + i);
  }
  for (int i = 0; i < 16; i++) {
    storedSalt[i] = EEPROM.read(SALT_ADDRESS + i);
  }
  EEPROM.end();
  _generate_hash(password, computedHash, storedSalt);
  bool match = true;
  for (int i = 0; i < 32; i++) {
    if (storedHash[i] != computedHash[i]) {
      match = false;
    }
  }

  return match;
}

void removePasswordHash() {
  EEPROM.begin(EEPROM_SIZE);

  for (int i = HASH_ADDRESS; i < HASH_ADDRESS + HASH_SIZE; i++) {
    EEPROM.write(i, 0xFF);
  }
  for (int i = SALT_ADDRESS; i < SALT_ADDRESS + SALT_SIZE; i++) {
    EEPROM.write(i, 0xFF);
  }

  uint32_t clearMagic = 0x00000000;
  EEPROM.put(MAGIC_ADDRESS, clearMagic);

  EEPROM.commit();
  EEPROM.end();

  Serial.println("\033[32m[+] Password removed successfully!\033[0m");
  Serial.println("\033[32m[+] System will restart...\033[0m");
  delay(2000);
  ESP.restart();
}

void handlePasswordSetting() {
  if (!settingPassword)
    return;

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (commandBuffer.length() > 0) {
        Serial.println();
        Serial.println("\033[32m[+] Password set successfully!\033[0m");
        storePasswordHash(commandBuffer);
        commandBuffer = "";
        cursorPosition = 0;
        authenticated = true;
        settingPassword = false;
        Serial.println("\033[32m[+] Authentication completed!\033[0m");
        displayBootInfo();
        return;
      }
    } else if (c == 8 || c == 127) {
      if (commandBuffer.length() > 0 && cursorPosition > 0) {
        commandBuffer.remove(cursorPosition - 1, 1);
        cursorPosition--;
        Serial.print("\b \b");
      }
    } else if (c >= 32 && c <= 126) {
      commandBuffer += c;
      cursorPosition++;
      Serial.print("*");
    }
  }
}

void handlePasswordRemoval() {
  if (!awaitingPasswordRemoval)
    return;

  if (checkForAbort()) {
    Serial.println();
    Serial.println("\033[33m[!] Password removal aborted.\033[0m");
    awaitingPasswordRemoval = false;
    commandBuffer = "";
    cursorPosition = 0;
    showPrompt();
    return;
  }

  while (Serial.available()) {
    char c = Serial.read();

    if (c == 3) {
      Serial.println();
      Serial.println("\033[33m[!] Password removal aborted.\033[0m");
      awaitingPasswordRemoval = false;
      commandBuffer = "";
      cursorPosition = 0;
      showPrompt();
      return;
    }

    if (c == '\n' || c == '\r') {
      if (commandBuffer.length() > 0) {
        Serial.println();
        if (verifyPassword(commandBuffer)) {
          Serial.println("\033[32m[+] Password verified!\033[0m");
          removePasswordHash();
        } else {
          Serial.println("\033[31m[!] Incorrect password! Password removal "
                         "cancelled.\033[0m");
          awaitingPasswordRemoval = false;
          showPrompt();
        }
        commandBuffer = "";
        cursorPosition = 0;
      }
    } else if (c == 8 || c == 127) {
      if (commandBuffer.length() > 0 && cursorPosition > 0) {
        commandBuffer.remove(cursorPosition - 1, 1);
        cursorPosition--;
        Serial.print("\b \b");
      }
    } else if (c >= 32 && c <= 126) {
      commandBuffer += c;
      cursorPosition++;
      Serial.print("*");
    }
  }
}

void handleAuthentication() {
    if (authenticated)
      return;
  
    if (!passwordHashExists()) {
      if (!promptShown) {
        Serial.println("\n\033[1;33mAUTHENTICATION SETUP\033[0m");
        Serial.println(
            "\033[33m================================================\033[0m");
        Serial.println(
            "\033[32m[!] No password found. Please set a new password.\033[0m");
        Serial.print("\033[33mEnter new password: \033[0m");
        promptShown = true;
        settingPassword = true;
      }
      return;
    } else {
      if (!promptShown) {
        Serial.println("\n\033[1;31mAUTHENTICATION REQUIRED\033[0m");
        Serial.println(
            "\033[31m================================================\033[0m");
        Serial.print("\033[33mEnter password: \033[0m");
        promptShown = true;
      }
  
      while (Serial.available()) {
        char c = Serial.read();

        if (c == 27) {
          delay(10);
          while (Serial.available()) {
            Serial.read();
          }
          continue;
        }
  
        if (c == '\n' || c == '\r') {
          if (commandBuffer.length() > 0) {
            Serial.println();
            if (verifyPassword(commandBuffer)) {
              Serial.println("\033[32m[+] Authentication completed!\033[0m");
              authenticated = true;
              displayBootInfo();
            } else {
              Serial.println("\033[31m[!] Invalid password! Try again: \033[0m");
              Serial.print("\033[33mEnter password: \033[0m");
            }
            commandBuffer = "";
            cursorPosition = 0;
          }
        } else if (c == 8 || c == 127) {
          if (commandBuffer.length() > 0 && cursorPosition > 0) {
            commandBuffer.remove(cursorPosition - 1, 1);
            cursorPosition--;
            Serial.print("\b \b");
          }
        } else if (c >= 32 && c <= 126) {
          commandBuffer += c;
          cursorPosition++;
          Serial.print("*");
        }
      }
    }
  }
  