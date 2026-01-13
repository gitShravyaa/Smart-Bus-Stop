# Smart-Bus-Stop

## Overview
A simple IoT-based dashboard created to display real-time information from a Smart Bus Stop prototype making traditional bus stops more helpful and intelligent.


## Tech Used
- Arduino IDE 
- HTML, CSS, JavaScript

## Folder Structure
- `ESP32/` → sensor code
- `Dashboard/` → Web dashboard

## How to Run Website
1. Open `file:///D:/VSCode/Projects/smartBusStop/index.html` in browser

## Hardware Used
- ESP32
- Ultrasonic sensor: Used to measure distance and detect if a bus is arriving.
- IR sensor: Used to detect movement of people entering/exiting the bus stop area, which can be used to estimate passenger count.
- LDR sensor: Used to detect light intensity and decide whether the bus stop lights should turn ON automatically when it’s dark.

## Working
- IoT device connects to WiFi and receives a local IP (e.g., 10.239.29.173).
- It reads values from LDR, IR, and ultrasonic sensors.
- It hosts an endpoint. script.js fetches this endpoint repeatedly.
- Dashboard updates sensor values on the screen.
