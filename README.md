<div align=center>

![NullSwitch](https://github.com/user-attachments/assets/49ce4e72-9adf-48dd-9204-850c559b8136)
# NullSwitch
[![Status](https://img.shields.io/badge/status-active-brightgreen.svg)](https://github.com/mintRaven-05/nullswitch)
[![Platform](https://img.shields.io/badge/platform-ESP8266-blue.svg?style=social&logo=github)](https://github.com/mintRaven-05/nullswitch)
[![stars](https://img.shields.io/github/stars/mintRaven-05/nullswitch?style=social)](https://github.com/mintRaven-05/nullswitch/stargazers)
[![forks](https://img.shields.io/github/forks/mintRaven-05/nullswitch?style=social)](https://github.com/serene-brew/kaizen/network/members)
[![Issues](https://img.shields.io/github/issues/mintRaven-05/nullswitch.svg?style=social&logo=github)](https://github.com/mintRaven-05/nullswitch/issues)
</div>

## What is NullSwitch?

**NullSwitch** is a lightweight, command-line-based wireless network auditing toolkit designed for the ESP8266 microcontroller. It provides a suite of tools for scanning, monitoring, and simulating wireless activity, with the goal of supporting educational, research, and authorized penetration testing scenarios.


![netscan](https://github.com/user-attachments/assets/3163a577-a2ad-4e51-aafb-a75098ba967d)
<div align=center>

![pktmon](https://github.com/user-attachments/assets/5b64d28a-4827-4f10-b0d7-947946729aa0)
</div>

## Available commands
<div align=center>
  
![help](https://github.com/user-attachments/assets/3df7e51a-3f3e-4ac5-8b1d-fd0c7ecde588)
</div>

## Current Features

#### Command-Line Interface (CLI)
- Command history traversal
- Auto tab completion
- Cursor control and navigation

#### Network Scanning and Monitoring
- Scan for nearby Wi-Fi networks
- Monitor active devices connected to discovered networks

#### Packet Monitoring
- Run in auto (channel-hopping) or locked channel mode
- Real-time detection and notification of deauthentication attacks

#### Beacon Attack Module
- Generate and broadcast fake access points
- Intended to confuse network scanners

## Legal Notice

>[!IMPORTANT]
> This software is intended **solely for educational, research, and authorized security auditing purposes**.
>
> - The authors do **not** endorse or condone any unauthorized or malicious use.
> - Use of this tool **without proper authorization** may violate local, national, or international laws.
> - You are fully responsible for any misuse. Only use this software in environments where you have **explicit permission** to operate.
>
> **By using this software, you agree to:**
> - Use it only in lawful and authorized contexts.
> - Accept full responsibility for your actions.
> - Release the developers from any liability or legal consequences.

## Flashing the ESP8266

To flash the firmware onto your ESP8266:

```bash
chmod +x flash.sh
./flash.sh
```
Make sure your board is connected to the port you will enter while flashing.

## Installation of dependencies and serial monitor

To install the required software components on your laptop:

```bash
chmod +x install.sh
./install.sh
```
This will install all dependencies and system tools needed to interact with the ESP8266 and run the command-line interface.

## Usage
To open serial monitor to the connected ESP8266, run the following command
```bash
nullswitch
```
OR
```bash
nullswitch <PORT>
```

The first way will ask for the port to the ESP8266

## Uninstallation

To uninstall the software from your laptop:

```bash
nullswitch --uninstall
```
This will remove installed components and clean up the system environment.


<p align="center">Copyright &copy; 2025 <a href="https://github.com/mintRaven-05" target="_blank">Debjeet Banerjee</a>
<p align="center"><a href="https://github.com/mintRaven-05/NullSwitch/blob/main/LICENSE"><img src="https://img.shields.io/static/v1.svg?style=for-the-badge&label=License&message=MIT&logoColor=d9e0ee&colorA=363a4f&colorB=b7bdf8"/></a></p>


