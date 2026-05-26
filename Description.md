# 🏭 Multi-Node Monitoring System (ESP-NOW)

Monitoring system based on an **ESP-NOW** local wireless network utilizing ESP32 microcontrollers. This system is designed to monitor operational processes across two machine units in real-time. The network implements a **4-to-1 Star Topology**, where four independent sender nodes periodically transmit telemetry data to a single centralized receiver station (*Central Receiver*).

---

## 📐 System Architecture & Topology

The system architecture divides the workload among the following sender node clusters:

* **Machine 1 Monitoring Cluster:**
    * **Node 1 (Button Sender):** Reads the physical button states (*Start, Timer, Emergency*).
    * **Node 3 (Scale Sender):** Reads and parses weight metrics from a digital weighing scale via Serial2 communication.
* **Machine 2 Monitoring Cluster:**
    * **Node 2 (Button Sender):** Reads the physical button states (*Start, Timer, Emergency*).
    * **Node 4 (Scale Sender):** Reads and parses weight metrics from a digital weighing scale via Serial2 communication.
* **Central Receiver Station:** Aggregates all data packets from the four sender nodes asynchronously, synchronizes them, and prints the combined diagnostics over the Serial Monitor.

---

## ✨ Features & Technical Implementation

* **Data Struct Synchronization (`#pragma pack`):** Ensures that the data byte-alignment on both sender and receiver structs is perfectly identical (`pack(push, 1)`). This step is critical to prevent data corruption or shifting during over-the-air transmission.
* **Digital Debounce Filter:** Button nodes are equipped with an adaptive delay function when a `LOW` input is detected to filter out mechanical switch bouncing noise common in  environments.
* **Latching State Machine Logic:** The button nodes process transient physical inputs into well-defined software states:
    * `State 0`: Stopped / *Idle*.
    * `State 1`: Running (Activated when the *Start* button is pressed, automatically clears when the *Timer* cycle ends).
    * `State 2`: Emergency Lockout (Locks the system down and overrides all running functions if the emergency stop button is active).
* **Dynamic Serial Data Parsing:** Scale nodes actively read raw text lines from the weighing indicators, strip away unnecessary unit characters (such as `kg`), and automatically cast the processed payload into a floating-point number (*float*).

---

## 🔧 Hardware Pin Configuration

### A. Button Nodes (Node 1 & Node 2)
| Function / Feature | ESP32 GPIO Pin | Pin Mode |
| :--- | :--- | :--- |
| **Head A - Start Button** | GPIO 32 | `INPUT_PULLUP` |
| **Head A - Timer Sensor** | GPIO 33 | `INPUT_PULLUP` |
| **Head A - Emergency Stop** | GPIO 25 | `INPUT_PULLUP` |
| **Head B - Start Button** | GPIO 26 | `INPUT_PULLUP` |
| **Head B - Timer Sensor** | GPIO 27 | `INPUT_PULLUP` |
| **Head B - Emergency Stop** | GPIO 21 | `INPUT_PULLUP` |

### B. Scale Nodes (Node 3 & Node 4)
| Function / Feature | ESP32 GPIO Pin | Communication Line |
| :--- | :--- | :--- |
| **RX Line (Receive Data)** | GPIO 16 | Hardware `Serial2` (9600 bps) |
| **TX Line (Transmit Data)**| GPIO 17 | Hardware `Serial2` (9600 bps) |

---

## 🖥️ Receiver Terminal Output Format

On the Central Receiver side, incoming data from the corresponding button and scale nodes are combined asynchronously, then printed out to the Serial Monitor using the following structured format:

```text
[MC1] Weight: 1.250 | A:[St:1 Ti:0 Em:0 Mode:1] | B:[St:0 Ti:0 Em:1 Mode:2]
[MC2] Weight: 0.000 | A:[St:0 Ti:0 Em:0 Mode:0] | B:[St:0 Ti:0 Em:0 Mode:0]
