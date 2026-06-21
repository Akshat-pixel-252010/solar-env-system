# Solar Environmental System – Prototype 2

## Goal

Develop and validate a low-cost environmental sensing platform capable of collecting reliable environmental data for future solar energy research.

The long-term objective is to study how environmental conditions affect solar panel performance and use the collected data for optimization, prediction, and autonomous energy management research.

---

## Current Stage

Prototype 2 – Hardware Integration & Architecture Validation

Focus Areas:

* PCB validation
* Sensor integration
* Firmware development
* Communication testing
* Data acquisition pipeline
* Failure analysis and debugging

This prototype is intended to identify architectural weaknesses before transitioning to a more reliable STM32-based platform.

---

## Hardware

### Main Controller

* ESP32-S3

### Sensors

* DHT22 (Temperature & Humidity)
* TSL2591 (Light Intensity)
* BMP280 (Pressure & Altitude)
* GPS Module
* MQ135 (Air Quality)
* INA219 
* DS18B20

---

## System Architecture

Current Architecture:

* ESP32-S3
* Custom PCB
* External sensor board
* Breadboard-assisted connections (temporary)

Future Architecture:

* Dedicated STM32 main board
* Dedicated sensor board
* Dedicated power management board
* CAN-FD communication network
* Fully modular design

---

## Dataset

* Environmental measurements
* Sensor validation data
* Prototype testing logs

Dataset :

---

## Development Timeline

### PCB Assembly

* Received Prototype 2 PCB.
* Soldered ESP32-S3 and supporting hardware.
* Began sensor integration.

### Initial Power Failure

Issue:

* Board became extremely hot during power-up.

Investigation:

* Suspected PCB design failure.
* Performed continuity and wiring inspection.

Root Cause:

* VCC and GND were accidentally connected due to soldering/wiring error.

Resolution:

* Isolated affected connection.
* Cut faulty wire connection.
* Removed short circuit.

Lesson Learned:

* Always perform continuity testing between power rails before powering a new PCB.

---

### Wiring Challenges

Issue:

* Large amount of time spent routing and validating connections.

Challenges:

* Correct pin mapping
* Sensor wiring verification
* GPIO reassignment
* UART remapping
* Breadboard integration

Resolution:

* Reassigned multiple GPIOs.
* Verified firmware pin mappings.
* Created working hybrid PCB + breadboard setup.

Lesson Learned:

* Pin mapping documentation should be maintained during development.
* Hybrid prototypes increase debugging complexity.

---

### Sensor Bring-Up

Successfully Tested:

* DHT22
* MQ135

Partially Tested:

* GPS
* TSL2591
* BMP280

Known Issues:

* DS18B20 pull-up resistor incorrectly connected.
* Multiple sensor communication issues still under investigation.

Lesson Learned:

* Sensor-specific electrical requirements must be verified before PCB design freeze.

---

### Firmware Development

Completed:

* Sensor polling framework
* Serial diagnostics
* Pin remapping
* GPS UART configuration
* Sensor testing framework

In Progress:

* WiFi integration
* Firebase integration
* Local dashboard
* Historical data storage

---

### Environmental Failure Event

Issue:

* Rainwater entered exposed electronics.

Effects:

* Temporary VCC-GND conduction.
* Sensor instability.
* Intermittent communication failures.

Resolution:

* Powered system down.
* Dried affected components.
* Re-tested sensors.

Lesson Learned:

* Outdoor electronics require environmental protection before long-term deployment.

---

## Key Engineering Lessons

### Electrical

* Verify power rails before power-up.
* Validate sensor pull-up requirements.
* Include test points on future PCBs.

### Mechanical

* Breadboards are unsuitable for outdoor deployment.
* Temporary wiring increases failure probability.

### Firmware

* Diagnostic tools are critical.
* Sensor-by-sensor validation is more effective than full-system debugging.

### System Design

* Reliability is more important than feature count.
* Architecture validation must occur before research data collection begins.

---

## Current Problems

### Hardware

* Wiring instability
* Breadboard dependency
* Outdoor exposure risks

### Sensors

* Intermittent communication failures
* Invalid readings
* Sensor detection issues

### Environment

* Rain exposure
* Dust accumulation
* Insect interference

---


Research begins once a stable and validated sensing platform is available.
