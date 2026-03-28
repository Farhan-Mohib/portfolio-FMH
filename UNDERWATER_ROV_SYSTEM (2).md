# MATE ROV — Tethered Underwater Drone Control System

**MATE ROV Competition Project | Multi-Camera · MAVLink · UDP Servo Control**

---

## 📋 Project Overview

A full-stack tethered underwater ROV system built for the MATE ROV competition. A single Ethernet tether connects the subsea vehicle to the surface pilot station, carrying MAVLink flight controller telemetry, three simultaneous RTSP camera streams, and servo control commands — all over a shared network. The system is operated using a PS5 DualSense controller with professional-grade smoothing for precise manipulator arm control.

### Key Capabilities

- ✅ **MAVLink telemetry** forwarded from Pixhawk → Raspberry Pi → QGroundControl over Ethernet tether
- ✅ **3 simultaneous USB camera streams** via RTSP on Raspberry Pi 5
- ✅ **Teensy MCU servo control** via UDP pulse commands over Ethernet
- ✅ **PS5 gamepad control** with three-layer smoothing (deadband → expo → EMA + rate limit)
- ✅ **Keyboard fallback control** for bench testing without a gamepad
- ✅ **Step-interpolated PWM** on Teensy to prevent mechanical servo jerks

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
│   │         QGroundControl · RTSP Viewer · Python            │   │
│   └────────────┬────────────────┬──────────────────┬─────────┘   │
│                │                │                  │             │
│   ┌────────────▼──┐  ┌──────────▼──────┐  ┌───────▼──────────┐  │
│   │ QGroundControl│  │   RTSP Viewer   │  │  Python Script   │  │
│   │Telemetry View │  │ Multi-cam Feeds │  │ Arm UDP Control  │  │
│   └───────────────┘  └─────────────────┘  └──────────────────┘  │
└──────────────────────────────────────────────────────────────────┘
```

### IP Address & Port Map

| Device / Service    | IP Address      | Port    | Protocol |
|---------------------|-----------------|---------|----------|
| Pilot Computer (PC) | `192.168.2.103` | —       | —        |
| Raspberry Pi 5      | `192.168.2.102` | —       | —        |
| Teensy MCU          | `192.168.2.1`   | `8888`  | UDP      |
| MAVLink → QGC       | `192.168.2.3`   | `14550` | UDP      |
| RTSP Cam 0          | Pi              | `8554`  | RTSP/TCP |
| RTSP Cam 1          | Pi              | `8555`  | RTSP/TCP |
| RTSP Cam 2          | Pi              | `8556`  | RTSP/TCP |

---

## 🔌 Data Flows During Operation

```
Pixhawk ──USB/MAVLink──► Pi5 ──UDP 14550──────────────────► QGroundControl
Camera 0 ──v4l2src──────► Pi5 ──RTSP 8554────────────────► RTSP Viewer
Camera 1 ──v4l2src──────► Pi5 ──RTSP 8555────────────────► RTSP Viewer
Camera 2 ──v4l2src──────► Pi5 ──RTSP 8556────────────────► RTSP Viewer
PS5 Controller ──────────► Python ──UDP 8888─────────────► Teensy → Servos
```

---

## 🤖 Pixhawk — MAVLink Setup

### Connection

```
Pixhawk ──USB(/dev/ttyACM0)──► Raspberry Pi 5 ──UDP:14550──► Pilot PC
```

### SSH Session 1 — MAVProxy

```bash
ssh pi@192.168.2.102
```

```bash
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
IP Address:   192.168.2.103
Subnet Mask:  255.255.255.0
```

Test connectivity: `ping 192.168.2.102`

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
  --out=udp:192.168.2.3:14550
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

## 📷 Multi-Camera RTSP Streaming

Three USB cameras stream simultaneously from the Raspberry Pi to the pilot station via RTSP.

### SSH Session 2 — Start Cameras

```bash
ssh farhanpi@192.168.2.102
```

```bash
python3 ~/multi_camera_rtsp.py
```

### RTSP Server Script (`~/multi_camera_rtsp.py`)

```python
#!/usr/bin/env python3
import gi
gi.require_version('Gst', '1.0')
gi.require_version('GstRtspServer', '1.0')
from gi.repository import Gst, GstRtspServer, GLib

Gst.init(None)

# Camera configuration: device -> RTSP port -> mount point
cameras = [
    {"device": "/dev/video4", "port": 8554, "mount": "/cam0"},
    {"device": "/dev/video8", "port": 8555, "mount": "/cam1"},
    {"device": "/dev/video0", "port": 8556, "mount": "/cam2"}
]

pi_ip = "192.168.2.102"

servers = []
for cam in cameras:
    server = GstRtspServer.RTSPServer.new()
    server.set_service(str(cam["port"]))
    mounts = server.get_mount_points()
    factory = GstRtspServer.RTSPMediaFactory.new()
    factory.set_launch(
        f"( v4l2src device={cam['device']} ! videoconvert ! "
        "video/x-raw,format=I420,width=640,height=480,framerate=30/1 ! "
        "x264enc tune=zerolatency bitrate=1500 speed-preset=superfast ! "
        "rtph264pay name=pay0 pt=96 )"
    )
    factory.set_shared(True)
    mounts.add_factory(cam["mount"], factory)
    server.attach(None)
    servers.append(server)
    print(f"RTSP server for {cam['mount']} running at rtsp://{pi_ip}:{cam['port']}{cam['mount']}")

loop = GLib.MainLoop()
loop.run()
```

### Stream URLs (open in VLC or QGroundControl)

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
byte mac[]           = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED};
IPAddress ip           (192,168,2,1);
unsigned int localPort = 8888;

const int SERVO1_PIN = 10;
const int SERVO2_PIN = 11;
const int US_MIN     = 500;
const int US_MAX     = 2500;
```

### UDP Packet Protocol

| Command       | Channel                    | Range       |
|---------------|----------------------------|-------------|
| `PULSE1:<us>` | Servo 1                    | 500–2500 µs |
| `PULSE2:<us>` | Servo 2                    | 500–2500 µs |
| `PULSE:<us>`  | Single servo (legacy mode) | 500–2500 µs |

### Teensy Code — Single Servo

```cpp
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <Servo.h>

byte mac[] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED};
IPAddress ip(192,168,2,3);
unsigned int localPort = 8888;

EthernetUDP Udp;
Servo servo;

const int SERVO_PIN = 10;
const int US_MIN = 500;
const int US_MAX = 2500;
const int STEP_US = 4;
const int STEP_MS = 5;

int currentUs = 1500;
int targetUs  = 1500;
unsigned long lastMs = 0;

static inline int clampUS(int us) {
  return constrain(us, US_MIN, US_MAX);
}

void setup() {
  servo.attach(SERVO_PIN, US_MIN, US_MAX);
  servo.writeMicroseconds(currentUs);
  delay(800);
  Ethernet.begin(mac, ip);
  delay(500);
  Udp.begin(localPort);
}

void loop() {
  if (Udp.parsePacket()) {
    char buf[32];
    int n = Udp.read(buf, sizeof(buf)-1);
    buf[n] = 0;
    if (strncmp(buf, "PULSE:", 6) == 0) {
      targetUs = clampUS(atoi(buf + 6));
    }
  }
  unsigned long now = millis();
  if (now - lastMs >= STEP_MS) {
    lastMs = now;
    if (currentUs < targetUs)
      currentUs = min(currentUs + STEP_US, targetUs);
    else if (currentUs > targetUs)
      currentUs = max(currentUs - STEP_US, targetUs);
    servo.writeMicroseconds(currentUs);
  }
}
```

### Teensy Code — Dual Servo

```cpp
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <Servo.h>

byte mac[] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED};
IPAddress ip(192,168,2,1);
unsigned int localPort = 8888;

EthernetUDP Udp;
Servo servo1;
Servo servo2;

const int SERVO1_PIN = 10;
const int SERVO2_PIN = 11;
const int US_MIN = 500;
const int US_MAX = 2500;

int currentUs1 = 500, targetUs1 = 500;
int currentUs2 = 500, targetUs2 = 500;

void setup() {
  Serial.begin(115200);
  delay(1000);
  servo1.attach(SERVO1_PIN, US_MIN, US_MAX);
  servo2.attach(SERVO2_PIN, US_MIN, US_MAX);
  servo1.writeMicroseconds(currentUs1);
  servo2.writeMicroseconds(currentUs2);
  Ethernet.begin(mac, ip);
  delay(500);
  Udp.begin(localPort);
  Serial.println("Teensy Ready");
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    char buf[32];
    int len = Udp.read(buf, sizeof(buf)-1);
    buf[len] = 0;
    Serial.print("Received: ");
    Serial.println(buf);
    if (strncmp(buf, "PULSE1:", 7) == 0) {
      targetUs1 = constrain(atoi(buf + 7), US_MIN, US_MAX);
      servo1.writeMicroseconds(targetUs1);
    }
    if (strncmp(buf, "PULSE2:", 7) == 0) {
      targetUs2 = constrain(atoi(buf + 7), US_MIN, US_MAX);
      servo2.writeMicroseconds(targetUs2);
    }
  }
}
```

### On-Board Step Interpolation (Single-servo mode)

Prevents mechanical jerks by limiting how fast the servo can move per tick:

```cpp
const int STEP_US = 4;    // µs per step (lower = smoother)
const int STEP_MS = 5;    // step interval in ms

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

| Parameter             | Value   | Effect                                                  |
|-----------------------|---------|---------------------------------------------------------|
| `DEADBAND`            | 0.06    | Stick dead zone — eliminates drift at rest              |
| `EXPO`                | 1.6     | Sensitivity exponent (>1 = softer near centre)          |
| `SEND_HZ`             | 150     | UDP update rate (Hz)                                    |
| `EMA_ALPHA`           | 0.18    | Smoothing strength (0.10 = very smooth, 0.30 = more responsive) |
| `MAX_RATE_US_PER_SEC` | 1800    | Maximum servo travel speed                              |
| `PULSE_CENTER`        | 1500 µs | Neutral / rest position                                 |
| `PULSE_MIN`           | 500 µs  | Hard minimum                                            |
| `PULSE_MAX`           | 2500 µs | Hard maximum                                            |

**Circle button** → instant return to centre (1500 µs)
**ESC key** → quit and send centre pulse on exit

```python
import socket
import pygame
import time
import math

TEENSY_IP = "192.168.2.1"
TEENSY_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

PULSE_CENTER = 1500
PULSE_MIN = 500
PULSE_MAX = 2500
PULSE_RANGE = min(PULSE_CENTER - PULSE_MIN, PULSE_MAX - PULSE_CENTER)

DEADBAND = 0.06
EXPO = 1.6
SEND_HZ = 150
EMA_ALPHA = 0.18
MAX_RATE_US_PER_SEC = 1800
PRINT_HZ = 5

def clamp(x, lo, hi):
    return lo if x < lo else hi if x > hi else x

def apply_deadband(x, db):
    if abs(x) < db:
        return 0.0
    sign = 1.0 if x >= 0 else -1.0
    return sign * (abs(x) - db) / (1.0 - db)

def expo_curve(x, expo):
    return math.copysign(abs(x) ** expo, x)

def send_pulse(us: int):
    sock.sendto(f"PULSE:{us}".encode(), (TEENSY_IP, TEENSY_PORT))

def main():
    pygame.init()
    pygame.joystick.init()

    if pygame.joystick.get_count() == 0:
        print("No controller found. Connect PS5 controller and try again.")
        return

    js = pygame.joystick.Joystick(0)
    js.init()
    print(f"Controller: {js.get_name()}")
    print("Left stick Y = control | Circle = center/stop | ESC = quit")

    filtered_pulse = float(PULSE_CENTER)
    target_pulse = float(PULSE_CENTER)
    dt_target = 1.0 / SEND_HZ
    last_send = time.perf_counter()
    last_print = time.perf_counter()

    running = True
    while running:
        now = time.perf_counter()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        keys = pygame.key.get_pressed()
        if keys[pygame.K_ESCAPE]:
            running = False

        circle_pressed = False
        for b in (1, 2):
            try:
                if js.get_button(b):
                    circle_pressed = True
            except pygame.error:
                pass

        y = -js.get_axis(1)
        y = apply_deadband(y, DEADBAND)
        y = expo_curve(y, EXPO)

        if circle_pressed:
            target_pulse = float(PULSE_CENTER)
        else:
            target_pulse = float(clamp(PULSE_CENTER + (y * PULSE_RANGE), PULSE_MIN, PULSE_MAX))

        if now - last_send >= dt_target:
            dt = now - last_send
            last_send = now
            max_step = MAX_RATE_US_PER_SEC * dt
            delta = clamp(target_pulse - filtered_pulse, -max_step, max_step)
            filtered_pulse += EMA_ALPHA * ((filtered_pulse + delta) - filtered_pulse)
            send_pulse(int(round(clamp(filtered_pulse, PULSE_MIN, PULSE_MAX))))

        if now - last_print >= 1.0 / PRINT_HZ:
            last_print = now
            print(f"stickY={y:+.2f}  target={int(target_pulse)}  out={int(filtered_pulse)}")

    send_pulse(PULSE_CENTER)
    pygame.quit()

if __name__ == "__main__":
    main()
```

---

### Keyboard Dual-Servo Control (Bench Testing)

Controls two independent servos without a gamepad:

```
W  / S      → Servo 1  ±40 µs  (large step)
D  / A      → Servo 1  ±5 µs   (fine step)
↑  / ↓      → Servo 2  ±40 µs  (large step)
→  / ←      → Servo 2  ±5 µs   (fine step)
ESC         → quit
```

```python
import socket
import pygame
import time

TEENSY_IP = "192.168.2.1"
TEENSY_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

PULSE_MIN = 500
PULSE_MAX = 2500
BIG_STEP = 40
SMALL_STEP = 5

servo1 = 500
servo2 = 500

def clamp(x, lo, hi):
    return lo if x < lo else hi if x > hi else x

def send_servo1(us):
    sock.sendto(f"PULSE1:{us}".encode(), (TEENSY_IP, TEENSY_PORT))

def send_servo2(us):
    sock.sendto(f"PULSE2:{us}".encode(), (TEENSY_IP, TEENSY_PORT))

def main():
    global servo1, servo2
    pygame.init()
    screen = pygame.display.set_mode((300, 200))
    pygame.display.set_caption("Servo Keyboard Control")
    print("Servo1: W/S big, A/D small | Servo2: UP/DOWN big, LEFT/RIGHT small | ESC quit")

    running = True
    while running:
        pygame.event.pump()
        keys = pygame.key.get_pressed()

        if keys[pygame.K_ESCAPE]: running = False
        if keys[pygame.K_w]: servo1 += BIG_STEP
        if keys[pygame.K_s]: servo1 -= BIG_STEP
        if keys[pygame.K_d]: servo1 += SMALL_STEP
        if keys[pygame.K_a]: servo1 -= SMALL_STEP
        if keys[pygame.K_UP]:    servo2 += BIG_STEP
        if keys[pygame.K_DOWN]:  servo2 -= BIG_STEP
        if keys[pygame.K_RIGHT]: servo2 += SMALL_STEP
        if keys[pygame.K_LEFT]:  servo2 -= SMALL_STEP

        servo1 = clamp(servo1, PULSE_MIN, PULSE_MAX)
        servo2 = clamp(servo2, PULSE_MIN, PULSE_MAX)

        send_servo1(servo1)
        send_servo2(servo2)
        print(f"S1:{servo1}   S2:{servo2}")
        time.sleep(0.03)

    pygame.quit()

if __name__ == "__main__":
    main()
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
        python3 ~/multi_camera_rtsp.py

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
ls /dev/ttyACM*                       # should show /dev/ttyACM0
dmesg | grep tty                      # check USB detection in kernel log
sudo usermod -a -G dialout $USER      # fix permission — reboot after
```

### USB Data Drops

```bash
# Disable USB autosuspend — add to /boot/cmdline.txt on a single line:
usbcore.autosuspend=-1
sudo reboot
```

### Camera Not Streaming

```bash
v4l2-ctl --list-devices      # verify USB cameras are visible to system
ls /dev/video*               # check available video devices
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

| Component          | Specification           | Role                             |
|--------------------|-------------------------|----------------------------------|
| Flight Controller  | Pixhawk 2.4.8 / Cube    | Sensors, navigation, MAVLink     |
| Companion Computer | Raspberry Pi 5          | Comms hub, RTSP server, MAVProxy |
| Servo Controller   | Teensy (NativeEthernet) | 2× servo PWM via UDP             |
| Network            | Gigabit Ethernet switch | Subsea LAN backbone              |
| Cameras            | 3× USB cameras          | Multi-angle pilot vision         |
| Gamepad            | PS5 DualSense (USB/BT)  | Arm control input                |
| Tether             | Cat5e / Cat6 Ethernet   | Single cable subsea ↔ surface    |

---

## 📊 Performance Specifications

| Metric                   | Value                           |
|--------------------------|---------------------------------|
| Servo UDP update rate    | 150 Hz                          |
| Servo pulse range        | 500–2500 µs                     |
| Servo step interpolation | 4 µs per 5 ms tick              |
| EMA smoothing alpha      | 0.18                            |
| Maximum servo rate       | 1800 µs/sec                     |
| Camera resolution        | 640×480                         |
| Camera frame rate        | 30 fps                          |
| Video bitrate            | 1500 kbps per camera            |
| Video encoding latency   | ~100–200 ms (zerolatency preset)|
| MAVLink baud rate        | 115200 via USB                  |
| MAVLink UDP port         | 14550                           |

---

## 📚 Software Dependencies

### Raspberry Pi

```
Raspberry Pi OS Lite (64-bit)
├── mavproxy           pip install mavproxy
├── gstreamer-1.0      apt install gstreamer1.0-tools gstreamer1.0-plugins-*
├── python3-gi         apt install python3-gi
└── gst-rtsp-server    apt install gstreamer1.0-rtsp
```

### Pilot PC

```
├── QGroundControl     latest release — qgroundcontrol.io
├── Python 3.x
│   ├── pygame         pip install pygame
│   └── socket         built-in
└── VLC                videolan.org
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

✅ **Single-tether design** — one Ethernet cable carries telemetry, three RTSP camera streams, and servo commands simultaneously with no signal interference

✅ **Professional servo control** — three-layer smoothing (deadband → expo → EMA + rate limit) delivers precise, jitter-free manipulator arm operation during competition tasks

✅ **Reliable MAVLink forwarding** — systemd service ensures MAVProxy restarts automatically if it crashes, keeping QGroundControl telemetry live throughout the dive

---

**Project Author:** Md Farhan Mohib Hemal
**Competition:** MATE ROV
**Status:** Production-Ready
**Version:** 2.0
