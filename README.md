# RFID IoT Access Control System with ESP32

---
---

# Description

Wi-Fi connected access control system built with an **ESP32** and **RC522 RFID reader**.  
The system authenticates RFID tags and logs each scan (UID, access status, timestamp) to a **MySQL database** via a **PHP web server**. Every authenticated scan is also locally signalled by a Blue LED and every invalid scan is signalled by a Red LED.

---

# Features
- Real-time RFID authentication using the MFRC522 module
- Wi-Fi enabled logging to a remote PHP/MySQL backend
- API key protection for secure communication
- Access granted/denied indication via LEDs
- Timestamped event logging

---

# Components

**Hardware:**
- 1x ESP32-WROOM-32E
- 1x RC522 RFID reader
- 1x Blue LED
- 1x Red LED
- 2x 220 Ohms Resistor
- 10x Male to Male Jumper Wires
- 1x Breadboard
- 1x Micro-USB Cable

**Software:** Arduino C++, PHP, MySQL, XAMPP  
**Protocols:** SPI, HTTP (POST), Wi-Fi

---

# Files

- **ESP32_RFID_Project.ino :** Main project file uploaded to the ESP32-WROOM-32E
- **access_log.php :** PHP script

---

# Additional References

---

## ESP32-WROOM-32E Pinout Diagram

<img width="1475" height="765" alt="image" src="https://github.com/user-attachments/assets/5fa74c3d-8d44-43b3-a5d6-380efd603935" />

---

## README by Ganojan Pathmasiri | Sept. 25, 2025 ##
