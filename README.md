# Hybrid-Energy-Using-Smart-Street-Light-for-Smart-Cities

Welcome to the Hybrid Energy Harvesting System repository! This project develops an innovative smart street lighting solution that integrates solar and piezoelectric energy harvesting technologies to power adaptive lighting with real-time data monitoring. The system optimizes energy efficiency, enhances operational performance, and promotes renewable energy awareness through user engagement features.
Table of Contents
Project Overview (#project-overview)

System Architecture (#system-architecture)

Features (#features)

Installation and Setup (#installation-and-setup)

Usage (#usage)

Performance Metrics (#performance-metrics)

Contributing (#contributing)

License (#license)

Contact (#contact)

Project Overview
This project implements a hybrid energy harvesting system for smart street lighting, combining solar panels and piezoelectric transducers to capture renewable energy from sunlight and mechanical vibrations (e.g., pedestrian or vehicular movement). The system includes intelligent lighting control, environmental sensing, energy storage, and a web-based data monitoring interface to ensure efficient operation and user engagement.
Key objectives:
Maximize energy efficiency through hybrid energy harvesting.

Provide adaptive lighting based on environmental conditions and motion detection.

Enable real-time system performance monitoring via a web interface.

Promote renewable energy awareness through interactive user feedback.

System Architecture
The system comprises four primary subsystems:
Energy Harvesting:
Solar: Photovoltaic panels (5V average, 7V max) capture sunlight energy.

Piezoelectric: 10 transducers (3V max each) harvest energy from vibrations.

Conditioning: Schottky diodes ensure unidirectional energy flow and protect components.

Energy Storage:
Uses 18650 Li-ion batteries (3.7V nominal).

Voltage divider (100kΩ resistors) and sensor monitor battery status.

Intelligent Lighting Control:
Environmental Sensing: LDR measures ambient light for adaptive lighting.

Motion Detection: PIR sensors activate lighting in low-light conditions.

Adaptive Brightness: Context-aware brightness control for roadside and flyover areas.

Data Monitoring and Visualization:
Data Acquisition: Current and voltage sensors track system performance.

Transmission: ESP32 with Wi-Fi sends data to a Flask-based Python web server.

Visualization: Web interface displays real-time metrics, historical trends, and data export options.

Features
Hybrid Energy Harvesting: Combines solar and piezoelectric energy for reliable power.

Adaptive Lighting: Adjusts brightness based on ambient light and motion detection.

Real-Time Monitoring: Web interface displays battery status, energy efficiency, and sensor data.

User Engagement: OLED display shows battery charging and energy production animations.

Data Management: SQL database stores historical data; supports Excel export and database reset.

Energy Efficiency: Optimized charging and storage reduce energy waste.

Installation and Setup
Prerequisites
Hardware:
Solar panels (5V-7V output)

Piezoelectric transducers (10 units, 3V max each)

18650 Li-ion batteries

ESP32 microcontroller with Wi-Fi

LDR, PIR, voltage, and current sensors

OLED display

Schottky diodes, resistors (100kΩ), and other circuit components

Software:
Python 3.x

Flask for web server

SQL database (e.g., SQLite or MySQL)

Arduino IDE or PlatformIO for ESP32 programming

Web browser for visualization interface

