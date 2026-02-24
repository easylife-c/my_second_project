# my_second_project
second krong ngen
SmartRoom Mini
Motion-Based Light & Fan Controller with Web Dashboard

SmartRoom Mini is a compact room automation system powered by the ESP32-WROOM-32.
It automatically controls lighting and ventilation based on motion detection and provides a clean web interface for monitoring and manual control.
Designed for classrooms, dorm rooms, offices, and smart home prototypes.
Overview

SmartRoom Mini detects human presence using a PIR motion sensor and automatically activates connected devices. A built-in web server allows users to view system status and manually control devices from any browser on the same network.
No external cloud service required.
Key Features
Motion-activated light and fan control
Automatic power-off after inactivity
Real-time web dashboard
Manual override via browser
Local network operation (no internet required)
Mobile-friendly interface
Low power consumption
System Architecture
Input
PIR Motion Sensor
Controller
ESP32-WROOM-32
Outputs

Light (5V DC via relay)
Fan (5V DC via relay)

Interface
Embedded Web Server (HTTP)
Hardware Requirements
ESP32-WROOM-32
PIR Sensor (HC-SR501 or compatible)
2-Channel 5V Relay Module
5V DC Light
5V DC Fan
5V Power Supply

Pin Configuration
Function	GPIO
PIR Sensor	13
Light Relay	26
Fan Relay	27

Relay logic: Active LOW
Operation Logic
Motion detected → Light and Fan turn ON
No motion for 30 seconds → Devices turn OFF
Manual override available via dashboard
Status updates every second
Web Dashboard
Accessible via local IP address:
http://<ESP32_IP>
Dashboard displays:
PIR Status (Motion / No Motion)
Light Status (ON / OFF)
Fan Status (ON / OFF)
Toggle switches for manual control

Designed for:
Fast loading
Mobile compatibility
Real-time updates (AJAX-based)

Installation
Install ESP32 board package in Arduino IDE
Select board: ESP32 Dev Module
Configure WiFi credentials in code
Upload firmware
Open Serial Monitor (115200 baud)
Access displayed IP address in browser
Safety Notice

For DC loads only (recommended).
If adapting for AC appliances, proper electrical isolation and safety measures are required.

Applications
Energy-saving classrooms
Dormitory automation
Smart office prototype
Embedded systems coursework
IoT demonstrations

Version
SmartRoom Mini v1.0
Local Network Web-Controlled Automation System
