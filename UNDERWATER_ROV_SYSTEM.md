# MATE ROV — Tethered Underwater Drone Control System

**MATE ROV Competition Project | Multi-Camera · MAVLink · UDP Servo Control**

---

## 📋 Project Overview

A full-stack tethered underwater ROV system built for the MATE ROV competition. A single Ethernet tether connects the subsea vehicle to the surface pilot station, carrying MAVLink flight controller telemetry, three simultaneous camera streams, and servo control commands — all over a shared network. The system is operated using a PS5 DualSense controller with professional-grade smoothing for precise manipulator arm control.

### Key Capabilities

- ✅ **MAVLink telemetry** forwarded from Pixhawk → Raspberry Pi → QGroundControl over Ethernet tether
- ✅ **3 simultaneous USB camera streams** via GStreamer on Raspberry Pi 5 (1280×720 @ 30 fps)
- ✅ **RTSP streaming** as an alternative to UDP camera delivery
- ✅ **Teensy MCU servo control** via UDP pulse commands over Ethernet
- ✅ **PS5 gamepad control** with three-layer smoothing (deadband → expo → EMA + rate limit)
- ✅ **Keyboard fallback control** for bench testing without a gamepad
- ✅ **Step-interpolated PWM** on Teensy to prevent mechanical servo jerks
- ✅ **Stable by-path camera addressing** — cameras never swap on USB reconnect

---

## 🏗️ System Architecture

### Network Topology

```
┌──────────────────────────────────────────────────────────────────┐
│                          ROV  (Subsea)                           │
│                                                                  │
│   ┌──────────────┐   USB    ┌─────────────────────────────┐      │
│   │   Pixhawk    │◄────────►│       Raspberry Pi 5        │      │
│   │  Sensors     │  MAVLink │  MAVProxy · RTSP Server     │      │
│   │  Navigation  │          │  Central Communications Hub │      │
│   │  MAVLink     │          └──────────┬──────────────────┘      │
│   └──────────────┘                     │                         │
│                                        │ Ethernet                │
│                               ┌────────▼────────┐               │
│                               │  Network Switch  │               │
│                               └────────┬────────┘               │
│                                        │ Ethernet                │
│                               ┌────────▼────────┐               │
│                               │   Teensy MCU    │               │
│                               │  Servo Control  │               │
│                               │  Manipulator Arm│               │
│                               └─────────────────┘               │
└──────────────────────────────────────────────────────────────────┘
                         │
                  Ethernet Tether
                         │
┌──────────────────────────────────────────────────────────────────┐
│                    Pilot Station  (Surface)                      │
│                                                                  │
│   ┌──────────────────────────────────────────────────────────┐   │
│   │                    Pilot Computer                        │   │
│   │           QGroundControl · RTSP Viewer                   │   │
│   └────────────┬────────────────┬──────────────────┬─────────┘   │
│                │                │                  │             │
│   ┌────────────▼──┐  ┌──────────▼──────┐  ┌───────▼──────────┐  │
│   │ QGroundControl│  │   RTSP Viewer   │  │  Python Script   │  │
│   │Telemetry View │  │ Multi-cam Feeds │  │ Arm UDP Control  │  │
│   └───────────────┘  └─────────────────┘  └──────────────────┘  │
└──────────────────────────────────────────────────────────────────┘
```

### IP Address & Port Map

| Device / Service | IP Address | Port | Protocol |
|---|---|---|---|
| Pilot Computer (PC) | `192.168.2.103` | — | — |
| Raspberry Pi 5 | `192.168.2.102` | — | — |
| Teensy MCU | `192.168.2.1` | `8888` | UDP |
| MAVLink → QGC | `192.168.2.3` | `14550` | UDP |
| Camera 0 → QGC | Pi → PC | `5602` | RTP/UDP |
| Camera 1 → QGC | Pi → PC | `5603` | RTP/UDP |
| Camera 2 → QGC | Pi → PC | `5604` | RTP/UDP |
| Camera 0 → Python/VLC | Pi → PC | `5700` | RTP/UDP |
| Camera 1 → Python/VLC | Pi → PC | `5701` | RTP/UDP |
| Camera 2 → Python/VLC | Pi → PC | `5702` | RTP/UDP |
| RTSP Cam 0 | Pi | `8554` | RTSP/TCP |
| RTSP Cam 1 | Pi | `8555` | RTSP/TCP |
| RTSP Cam 2 | Pi | `8556` | RTSP/TCP |

---

## 🔌 Data Flows During Operation

```
Pixhawk ──USB/MAVLink──► Pi5 ──UDP 14550──────────────────► QGroundControl
Camera 0 ──GStreamer──► Pi5 ──UDP 5602 / 5700────────────► QGC / VLC
Camera 1 ──GStreamer──► Pi5 ──UDP 5603 / 5701────────────► QGC / VLC
Camera 2 ──GStreamer──► Pi5 ──UDP 5604 / 5702────────────► QGC / VLC
PS5 Controller ──────► Python ──UDP 8888──────────────────► Teensy → Servos
```

---

## 🤖 Pixhawk — MAVLink Setup

### Connection

```
Pixhawk ──USB(/dev/ttyACM0)──► Raspberry Pi 5 ──UDP:14550──► Pilot PC
```

### SSH and MAVProxy Command

```bash
# From pilot PC — open SSH session on Pi:
ssh pi@192.168.2.102

# Activate virtual environment and forward MAVLink:
source ~/mavenv/bin/activate
mavproxy.py --master=/dev/ttyACM0 --baudrate 115200 --out=udp:192.168.2.3:14550
```

Look for `HEARTBEAT received` in the MAVProxy output — this confirms the Pixhawk is detected and data is flowing.

### Raspberry Pi Static Ethernet IP

```bash
sudo nano /etc/dhcpcd.conf
```

```conf
interface eth0
static ip_address=192.168.2.2/24
static routers=192.168.2.1
static domain_name_servers=8.8.8.8
```

### Pilot PC Static IP (Windows)

```
IP Address:   192.168.2.1
Subnet Mask:  255.255.255.0
```

Test connectivity: `ping 192.168.2.2`

### MAVProxy Auto-Start on Boot

```bash
sudo nano /etc/systemd/system/mavproxy.service
```

```ini
[Unit]
Description=Pixhawk USB MAVLink
After=network.target

[Service]
ExecStart=/usr/local/bin/mavproxy.py \
  --master=/dev/ttyACM0 \
  --baudrate 115200 \
  --out=udp:192.168.2.1:14550
Restart=always
User=pi

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable mavproxy
sudo systemctl start mavproxy
```

---

## 📷 Multi-Camera Streaming

Three USB cameras stream simultaneously from the Raspberry Pi to the pilot station via GStreamer.

### Camera Device Mapping

Stable `by-path` names are used instead of `/dev/video0` to prevent camera assignment shuffling on USB reconnect:

```bash
CAM_MAP[0]="/dev/v4l/by-path/platform-xhci-hcd.0-usb-0:2:1.0-video-index0"
CAM_MAP[1]="/dev/v4l/by-path/platform-xhci-hcd.0-usb-0:1:1.0-video-index0"
CAM_MAP[2]="/dev/v4l/by-path/platform-xhci-hcd.1-usb-0:1:1.0-video-index0"
```

### GStreamer Pipeline (per camera)

```bash
gst-launch-1.0 -v \
  v4l2src device=$device ! \
  image/jpeg,width=1280,height=720,framerate=30/1 ! \
  jpegdec ! videoconvert ! \
  x264enc tune=zerolatency bitrate=1500 speed-preset=superfast ! \
  rtph264pay config-interval=1 pt=96 ! tee name=t ! \
    queue ! udpsink host=192.168.2.3 port=$qgc_port sync=false t. ! \
    queue ! udpsink host=192.168.2.3 port=$py_port  sync=false
```

`tune=zerolatency` and `speed-preset=superfast` minimize encoding delay for real-time underwater piloting.

### Starting All Cameras

```bash
# SSH into Pi:
ssh farhanpi@192.168.2.102

# Start all three cameras with 1-second gaps:
./camera_manager.sh start 0
sleep 1
./camera_manager.sh start 1
sleep 1
./camera_manager.sh start 2

# Check status:
./camera_manager.sh status

# Stop all:
./camera_manager.sh stop
```

### RTSP Alternative

```bash
python3 ~/multi_camera_rtsp.py
```

Stream URLs (open in VLC or QGroundControl):

```
rtsp://192.168.2.102:8554/cam0
rtsp://192.168.2.102:8555/cam1
rtsp://192.168.2.102:8556/cam2
```

---

## 🎮 Teensy MCU — Servo Control

The Teensy receives UDP packets over Ethernet and drives PWM signals to the manipulator arm servos.

### Teensy Network Configuration

```cpp
byte mac[]         = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED};
IPAddress ip         (192,168,2,1);
unsigned int localPort = 8888;

const int SERVO1_PIN = 10;
const int SERVO2_PIN = 11;
const int US_MIN     = 500;
const int US_MAX     = 2500;
```

### UDP Packet Protocol

| Command | Channel | Range |
|---|---|---|
| `PULSE1:<us>` | Servo 1 | 500–2500 µs |
| `PULSE2:<us>` | Servo 2 | 500–2500 µs |
| `PULSE:<us>` | Single servo (legacy mode) | 500–2500 µs |

### On-Board Step Interpolation (Single-servo mode)

Prevents mechanical jerks by limiting how fast the servo can move per tick:

```cpp
const int STEP_US = 4;    // µs per step (lower = smoother)
const int STEP_MS = 5;    // step interval in ms

// Each 5ms tick:
if (currentUs < targetUs)
    currentUs = min(currentUs + STEP_US, targetUs);
else if (currentUs > targetUs)
    currentUs = max(currentUs - STEP_US, targetUs);

servo.writeMicroseconds(currentUs);
```

---

## 🐍 Python Control Scripts

### PS5 Gamepad Control (Competition Mode)

Three-layer smoothing pipeline for professional servo operation:

```
PS5 Left Stick Y
       │
       ▼
 Deadband filter (±0.06)
 — eliminates stick drift near centre
       │
       ▼
 Expo curve (^1.6)
 — soft response near centre, strong at full deflection
       │
       ▼
 Rate limiter (1800 µs/sec max)
 — caps maximum speed of movement
       │
       ▼
 EMA low-pass filter (α = 0.18)
 — smooths out any remaining jitter
       │
       ▼
 UDP → PULSE:<int>  at 150 Hz → Teensy → Servo
```

#### Tuning Parameters

| Parameter | Value | Effect |
|---|---|---|
| `DEADBAND` | 0.06 | Stick dead zone — eliminates drift at rest |
| `EXPO` | 1.6 | Sensitivity exponent (>1 = softer near centre) |
| `SEND_HZ` | 150 | UDP update rate (Hz) |
| `EMA_ALPHA` | 0.18 | Smoothing strength (0.10 = very smooth, 0.30 = more responsive) |
| `MAX_RATE_US_PER_SEC` | 1800 | Maximum servo travel speed |
| `PULSE_CENTER` | 1500 µs | Neutral / rest position |
| `PULSE_MIN` | 500 µs | Hard minimum |
| `PULSE_MAX` | 2500 µs | Hard maximum |

**Circle button** → instant return to centre (1500 µs)
**ESC key** → quit and send centre pulse on exit

---

### Keyboard Dual-Servo Control (Bench Testing)

Controls two independent servos without a gamepad:

```
W  / S          → Servo 1  ±40 µs  (large step)
D  / A          → Servo 1  ±5 µs   (fine step)
↑  / ↓          → Servo 2  ±40 µs  (large step)
→  / ←          → Servo 2  ±5 µs   (fine step)
ESC             → quit
```

```python
BIG_STEP   = 40    # µs — fast repositioning
SMALL_STEP = 5     # µs — fine precise adjustment
```

### Minimal Keyboard Control (Single servo)

Lightweight script for quick bench tests:

```
W       → +10 µs
S       → -10 µs
SPACE   → centre (1500 µs)
ESC     → quit
```

---

## ✅ Competition Day Setup Sequence

```
1.  Power on ROV electronics (Raspberry Pi, Teensy, network switch)
2.  Connect Ethernet tether to pilot station
3.  Verify Pi reachability:          ping 192.168.2.102
4.  SSH Session 1 — start MAVProxy:
        ssh pi@192.168.2.102
        source ~/mavenv/bin/activate
        mavproxy.py --master=/dev/ttyACM0 --baudrate 115200 --out=udp:192.168.2.3:14550
5.  SSH Session 2 — start cameras:
        ssh farhanpi@192.168.2.102
        ./camera_manager.sh start 0 && sleep 1
        ./camera_manager.sh start 1 && sleep 1
        ./camera_manager.sh start 2
6.  Open QGroundControl → UDP port 14550 → confirm live telemetry
7.  Open RTSP viewer / VLC → confirm all 3 camera feeds are live
8.  Connect PS5 controller to pilot PC (USB preferred)
9.  Run arm control Python script on pilot PC
10. Confirm servo response on Teensy serial monitor
11. Begin dive ✅
```

---

## 🛠️ Troubleshooting

### Pixhawk Not Detected

```bash
ls /dev/ttyACM*           # should show /dev/ttyACM0
dmesg | grep tty          # check USB detection in kernel log
sudo usermod -a -G dialout $USER   # fix permission — reboot after
```

### USB Data Drops

```bash
# Disable USB autosuspend — add to /boot/cmdline.txt on a single line:
usbcore.autosuspend=-1
sudo reboot
```

### Camera Not Streaming

```bash
./camera_manager.sh status        # check which cameras are running
v4l2-ctl --list-devices           # verify USB cameras are visible to system
ls /dev/v4l/by-path/              # verify stable device paths are present
```

### Servo Not Responding

```python
# Quick UDP test from pilot PC:
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(b'PULSE1:1500', ('192.168.2.1', 8888))
print('Sent')
# Check Teensy Serial monitor for: Received: PULSE1:1500
```

### MAVProxy Not Forwarding

```bash
sudo systemctl status mavproxy     # check service health
sudo systemctl restart mavproxy    # restart if needed
# Manually verify: look for "HEARTBEAT received" in output
```

---

## 🔧 Hardware Bill of Materials

| Component | Specification | Role |
|---|---|---|
| Flight Controller | Pixhawk 2.4.8 / Cube | Sensors, navigation, MAVLink |
| Companion Computer | Raspberry Pi 5 | Comms hub, RTSP server, MAVProxy |
| Servo Controller | Teensy (NativeEthernet) | 2× servo PWM via UDP |
| Network | Gigabit Ethernet switch | Subsea LAN backbone |
| Cameras | 3× USB cameras | Multi-angle pilot vision |
| Gamepad | PS5 DualSense (USB/BT) | Arm control input |
| Tether | Cat5e / Cat6 Ethernet | Single cable subsea ↔ surface |

---

## 📊 Performance Specifications

| Metric | Value |
|---|---|
| Servo UDP update rate | 150 Hz |
| Servo pulse range | 500–2500 µs |
| Servo step interpolation | 4 µs per 5 ms tick |
| EMA smoothing alpha | 0.18 |
| Maximum servo rate | 1800 µs/sec |
| Camera resolution | 1280×720 |
| Camera frame rate | 30 fps |
| Video bitrate | 1500 kbps per camera |
| Video encoding latency | ~100–200 ms (zerolatency preset) |
| MAVLink baud rate | 115200 via USB |
| MAVLink UDP port | 14550 |

---

## 📚 Software Dependencies

### Raspberry Pi

```
Raspberry Pi OS Lite (64-bit) — recommended OS
├── mavproxy           pip install mavproxy
├── gstreamer-1.0      apt install gstreamer1.0-tools gstreamer1.0-plugins-*
├── python3-gi         apt install python3-gi  (for RTSP server mode)
└── gst-rtsp-server    apt install gstreamer1.0-rtsp
```

### Pilot PC

```
├── QGroundControl     latest release — qgroundcontrol.io
├── Python 3.x
│   ├── pygame         pip install pygame   (controller + keyboard input)
│   └── socket         built-in
└── VLC                videolan.org  (for RTSP / UDP camera feeds)
```

### Teensy

```
Arduino IDE + Teensyduino add-on
├── NativeEthernet
├── NativeEthernetUdp
└── Servo
```

---

## 📄 Conclusion

✅ **Single-tether design** — one Ethernet cable carries telemetry, three camera streams, and servo commands simultaneously with no signal interference

✅ **Professional servo control** — three-layer smoothing (deadband → expo → EMA + rate limit) delivers precise, jitter-free manipulator arm operation during competition tasks

✅ **Resilient camera system** — stable `by-path` device addressing prevents camera reassignment on reconnect; `camera_manager.sh` handles start/stop/status per camera independently

✅ **Reliable MAVLink forwarding** — systemd service ensures MAVProxy restarts automatically if it crashes, keeping QGroundControl telemetry live throughout the dive

---

**Project Author:** Md Farhan Mohib Hemal
**Competition:** MATE ROV
**Status:** Production-Ready
**Version:** 1.0
