#!/bin/bash

echo "[+] Starting NullSwitch firmware flashing process..."

# Step 1: Check for PlatformIO installation
if which pio >/dev/null 2>&1; then
  echo "[+] PlatformIO is already installed."
else
  echo "[!] PlatformIO not found. Installing using pip..."
  pip install platformio --break-system-packages
  if [ $? -ne 0 ]; then
    echo "[!] Failed to install PlatformIO. Aborting."
    exit 1
  fi
fi

# Step 2: Ask user for the /dev port
echo "[+] Please enter the /dev port of your connected ESP8266 (e.g., /dev/ttyUSB0):"
read -r PORT

# Validate port input
if [ ! -e "$PORT" ]; then
  echo "[!] Port $PORT does not exist. Please check the device connection."
  exit 1
fi

# Step 3: Flash the firmware using PlatformIO
echo "[+] Uploading firmware to ESP8266 on port $PORT..."
pio run --target upload --upload-port "$PORT"
if [ $? -eq 0 ]; then
  echo "[+] Firmware upload successful!"
else
  echo "[!] Firmware upload failed. Please check your connection and try again."
  exit 1
fi

echo "[+] Done."
