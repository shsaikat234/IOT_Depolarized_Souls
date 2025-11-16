# 🚖 AERAS Rickshaw Management System – Full Project Repository

> **⚠️ Important Notice:**  
> **Do NOT update or commit any changes after the final submission!**  
> This repository represents a complete, submitted version of the AERAS Rickshaw Management System project.

---

## 📌 Project Overview

AERAS is an IoT-based Rickshaw Management and Ride-Hailing System designed to connect riders, pullers, and admins through a unified digital ecosystem.  
This repository contains **all components of the complete system**, including:

- ✓ **User-side microcontroller (ESP32/Arduino) code**  
- ✓ **Rickshaw-side Web App**  
- ✓ **Backend server (Node.js / Python / Django / any)**  
- ✓ **Admin dashboard code**  
- ✓ **README with setup instructions**  

The goal of AERAS is to provide real-time ride request handling, puller location tracking, trip monitoring, and centralized administrative control.

---

## 🧩 System Components

### 1️⃣ **User-Side Microcontroller Code (ESP32)**  
Located in `/microcontroller/`

This module handles:
- OLED display updates (SSD1306)
- GPS location acquisition (NEO-6M or compatible)
- WiFi connection + backend communication
- Trip request acceptance flow
- Real-time data transmission

**Hardware Connections:**
- OLED SDA → GPIO 21  
- OLED SCL → GPIO 22  
- GPS TX → ESP32 RX  
- GPS RX → ESP32 TX  

---

### 2️⃣ **Rickshaw-Side Web App**  
Located in `/rickshaw-webapp/`

Features:
- Puller login dashboard  
- Incoming ride request notifications  
- Route map + live user destination  
- Start/End trip interface  
- Syncs with backend via WebSocket/MQTT  

Technologies used:
- HTML, CSS, JavaScript  
- React / Vue / Vanilla JS (based on your implementation)

---

### 3️⃣ **Backend Server**  
Located in `/backend/`

- **Node.js (Express.js)**  

Core functionalities:
- User–Puller ride matching  
- Real-time communication (WebSocket/MQTT/HTTP)  
- Database management (MongoDB / PostgreSQL / MySQL)  
- Trip logging, analytics, status tracking  
- API endpoints for rider, puller, admin  

---

### 4️⃣ **Admin Dashboard**  
Located in `/admin-dashboard/`

Features:
- Global rickshaw tracking  
- Trip logs & history  
- Puller activity statistics  
- User management  
- System analytics  

---

## 📂 Repository Structure

