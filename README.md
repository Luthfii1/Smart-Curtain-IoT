# SMARTLOOM CURTAIN MANAGER

## Introduction

Welcome to the Smartloom Curtain Manager project! In this readme, we'll provide an overview of the problem we aimed to solve and the innovative solution we developed. This IoT-enabled system transforms the way you interact with your home's curtains, offering both automation and user-controlled customization.

## Problem Statement

The challenge was to create an Internet of Things (IoT) enabled Smartloom Curtain Manager system that optimizes curtain positions for energy efficiency, convenience, and sustainability. The system needed to consider factors such as time of day, weather conditions, and user preferences. Additionally, a secure and user-friendly remote control functionality was crucial for managing curtains through a dedicated mobile application.

## Proposed Solution

The Smartloom Curtain Manager addresses these challenges with a combination of hardware and software components. The system is equipped with an ESP32 microcontroller, a Light Dependent Resistor (LDR) module for light detection, and an L298N Motor Driver for precise curtain movement. The Blynk platform is utilized for seamless communication between the user interface and the IoT device. The system supports both automatic and manual modes, allowing users to customize their curtain preferences.

## Hardware Design and Implementation Details

### Hardware Components

- **ESP32 Microcontroller:** Processes data, communicates with the Blynk app, and controls the L298N motor drivers.
- **LDR Module:** Detects ambient light intensity for automated curtain operation.
- **L298N Motor Driver Module:** Controls the DC motor for smooth and precise curtain movement.
- **ESP32 Expansion Board:** Facilitates additional component integration.
- **Cardboard Safety Package:** Provides eco-friendly protection for hardware components.

### Implementation Steps

1. **Hardware Development:** Research and select compatible hardware tools.
2. **Hardware Design Schematic:** Create a detailed schematic design for hardware components.
3. **Testing Each Tool:** Individually test each hardware tool for functionality.
4. **Create Code for Each Tool:** Develop code snippets for each hardware tool.
5. **Implement Schematic to Real Hardware:** Implement finalized schematic design into a physical circuit.

## Network Infrastructure

The system utilizes the Blynk platform for remote control. The WiFi Manager library enables dynamic configuration of WiFi credentials, allowing users to connect the Smart Curtain to a WiFi network seamlessly.

## Software Implementation Details

The software development focuses on configuring the Blynk platform for efficient communication. The custom Blynk template, named "FinPro," ensures an intuitive user interface. The code is organized into FreeRTOS tasks for parallel execution and scalability.

## Test Results and Performance Evaluation

### Testing

- **LDR Sensor:** Tested for accurate readings in both dark and bright conditions.
- **DC Motor:** Tested for reliable and responsive curtain movement.
- **Blynk Integration:** Tested for proper communication between the user interface and the embedded system.
- **ESP32 WiFi Connection:** Tested for successful connection to WiFi networks.

### Results

- **LDR Sensor:** Accurately detects light intensity.
- **DC Motor:** Successfully opens and closes the curtain.
- **Blynk Integration:** Allows for seamless remote control and monitoring.
- **ESP32 WiFi Connection:** Connects to WiFi networks efficiently.

## Conclusion and Future Work

In conclusion, the Smartloom Curtain Manager project provides an advanced solution for automated and manual curtain control. The integration of hardware components, such as the ESP32 microcontroller and LDR sensor, with the Blynk platform ensures a user-friendly and efficient system.

### Future Work

- **Enhanced Sensor Capabilities:** Explore additional sensors for more nuanced environmental data.
- **Voice Control Integration:** Implement voice-activated controls for added convenience.
- **Security Enhancements:** Further strengthen communication security protocols.

Thank you for exploring the Smartloom Curtain Manager project readme. Feel free to contribute, provide feedback, or explore further improvements!
