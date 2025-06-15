# 🌞⚡ Hybrid Energy Harvesting System for Smart Street Lighting

### Combining Solar and Piezoelectric Energy with Intelligent Lighting Control and Real-Time Monitoring

---

## 🧠 Project Overview

This project presents a **hybrid energy harvesting system** using **solar panels** and **piezoelectric transducers** to power a **smart street lighting system**. Designed for **sustainable urban infrastructure**, it integrates multiple subsystems—intelligent lighting, adaptive brightness control, energy monitoring, and web-based data visualization—using an **ESP32 microcontroller**, environmental sensors, and a **Python Flask web server**.

---

## 📐 System Architecture

The system comprises 4 core subsystems:

1. **Energy Harvesting Subsystem**  
   - Solar panels (5V avg, 7V max) for daytime energy capture  
   - Piezoelectric transducers (3V max) under walkways for vibration energy harvesting  
   - Energy conditioning with Schottky diodes

2. **Energy Storage Subsystem**  
   - Li-ion 18650 batteries (3.7V nominal)  
   - Voltage divider (2x 100kΩ) and voltage sensor for monitoring  

3. **Intelligent Lighting Control Subsystem**  
   - LDR for ambient light sensing  
   - PIR sensor for motion detection  
   - Adaptive brightness for different zones (roadside vs flyover)

4. **Data Monitoring and Visualization Subsystem**  
   - Real-time data logging (voltage, current, PIR/LDR events)  
   - ESP32 Wi-Fi transmission to Flask-based local server  
   - Dashboard with live metrics and downloadable logs  

---

## 🔧 Features

- 🔋 **Dual Energy Harvesting**: Collect energy from both solar radiation and pedestrian/vehicular pressure  
- 💡 **Adaptive Street Lighting**: Adjust brightness based on light conditions and motion detection  
- 📊 **Live Monitoring Dashboard**: Web interface for real-time energy tracking and system status  
- 📈 **Historical Data Visualization**: Time-series graphs with options to export as Excel  
- 🖥️ **OLED User Feedback**: Displays system status and promotes awareness of energy use  

---

## 🖥️ Hardware Components

- ESP32 WROOM-32 Microcontroller  
- Solar Panel (5–7V output)  
- Piezoelectric Elements (10 transducers, 3V each)  
- 18650 Li-ion Battery  
- LDR (Light Dependent Resistor)  
- PIR Motion Sensor  
- Current and Voltage Sensors  
- OLED Display  
- Schottky Diodes, Resistors (100kΩ), Voltage Divider Circuit

---

## 🧪 Software Stack

| Layer          | Tool/Language        | Purpose                                  |
|----------------|----------------------|------------------------------------------|
| Firmware       | Arduino (C/C++)      | Sensor interfacing, energy control       |
| Server         | Python (Flask)       | Data handling and web serving            |
| Frontend       | HTML/CSS + Chart.js  | Web dashboard for real-time visualization|
| Database       | SQLite (via Flask)   | Sensor data logging and export           |

---

## 🌐 Web Interface Features

- ✅ System component status indicators  
- 🔋 Battery charge level bar  
- 📉 Area graphs showing power usage and voltage over time  
- 📥 Data download in Excel format  
- 🔄 Admin controls for resetting logs  

---


