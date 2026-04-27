# Autonomous Food Delivery Robot

An embedded robot system designed to deliver food autonomously along a predefined path, with an additional Wi-Fi remote-control mode for manual operation.

This project combines line-following navigation, obstacle detection, LCD status display, buzzer feedback, motor control, and ESP-01 wireless communication into a single food delivery robot prototype.

---

## Overview

The robot is designed to support simple food delivery in a controlled environment. In autonomous mode, it follows a line route, reacts to turns using IR sensors, detects nearby obstacles with an ultrasonic sensor, and updates its movement status through an LCD and buzzer. In remote mode, it can be manually controlled over Wi-Fi through commands relayed by an ESP-01 module.

The project also includes CAD part files for the robot body and component holders.

---

## Key Features

- Autonomous line-following using dual IR sensors
- Obstacle detection using an ultrasonic sensor
- LCD status updates for robot state and delivery flow
- Passive buzzer feedback for movement and robot events
- DC motor control for forward, reverse, and turning actions
- Wi-Fi remote control mode using ESP-01
- Physical CAD part designs for the robot structure and holders

---

## Operating Modes

### 1. Autonomous Delivery Mode
In autonomous mode, the robot:

- waits for the start trigger / target-board interrupt
- follows a black line using two IR sensors
- turns based on line position
- detects nearby obstacles using the ultrasonic sensor
- stops when blocked or when the route is completed
- shows status updates on the LCD
- plays different buzzer patterns for movement states

This mode is implemented in the Arduino autonomous sketch.

### 2. Wi-Fi Remote Control Mode
In remote mode, the robot:

- connects to Wi-Fi through the ESP-01
- receives movement commands over TCP
- supports commands such as:
  - `forward`
  - `reverse`
  - `turnleft`
  - `turnright`
- relays messages between the ESP-01 and the Arduino Uno
- displays debug or received messages on the LCD

This mode is split between the Arduino-side control sketch and the ESP-01 communication sketch.

---

## Hardware Used

- Arduino Uno
- ESP-01 / ESP8266 Wi-Fi module
- 16x2 I2C LCD
- Dual IR sensors
- Ultrasonic sensor
- PIR / trigger-related sensing components
- Passive buzzer
- Motor driver and DC motors
- Robot chassis / custom structural parts

---

## Software and Libraries

### Arduino-side libraries
- `RichShieldPassiveBuzzer`
- `PinChangeInterrupt`
- `Wire`
- `LiquidCrystal_I2C`
- `SoftwareSerial`

### ESP-01 libraries
- `ESP8266WiFi`
- `WiFiClient`
- `WiFiManager`

---

## Repository Structure

```text
autonomous-food-delivery-robot/
├─ arduino/        # Arduino Uno sketches for autonomous and remote robot control
├─ esp-01/         # ESP-01 / ESP8266 Wi-Fi communication code
├─ cad/            # CAD part files for robot body and holders
├─ autonomous robot movement.mp4
├─ robot movement VIA wifi.mp4
├─ foodDeliveryRobot_presentation.pdf
└─ README.md
