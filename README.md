# Md Farhan Mohib Hemal | Portfolio & Research

**Underwater Robotics Engineer | Signal Processing Researcher | Software Architect**

---

## 📋 Overview

Welcome to my professional portfolio and research repository. This space showcases my work in underwater robotics systems, advanced signal processing, and autonomous control systems. As an Electrical Engineering student at Military Institute of Science and Technology (MIST), I combine academic excellence with practical engineering innovation.

### Key Highlights

- 🏆 **Best Paper Award** - STI 2025 Conference on Underwater Robotics
- 🤖 **Co-founder & Software Lead** - Mavirov Underwater ROV Project
- 📊 **Published Researcher** - Google Scholar & ResearchGate profiles
- 🎓 **Academic Excellence** - A+ in HSC with 96% in Physics, Chemistry, Mathematics (PCM)
- 💼 **Professional Background** - MIST Electrical Engineering student

---

## 🎯 Research Areas

### 1. Underwater Robotics & Autonomous Systems
- Pixhawk-based autopilot integration
- Real-time telemetry and control architecture
- Autonomous navigation and path planning
- Multi-sensor fusion for underwater environments

### 2. Signal Processing & Communications
- Pulse Amplitude Modulation (PAM) systems
- Time Division Multiplexing (TDM) implementation
- Digital filter design (Butterworth, IIR)
- Signal recovery and demodulation techniques

### 3. Control Systems & Hardware Integration
- 6-DOF servo control systems
- Real-time embedded control
- Raspberry Pi and Teensy MCU integration
- PWM signal generation and management

---

## 🚀 Major Projects

### Mavirov ROV - Underwater Robotics Platform

**Role:** Co-founder & Software Team Lead

**Architecture Overview:**
```
Pixhawk Autopilot ↔ Raspberry Pi 5 Hub ↔ Teensy MCU
      (Control)          (Central)       (Servo Driver)
         ↓                    ↓                ↓
    Sensors/Nav      Cameras/Telemetry   6-DOF Manipulator
```

**Key Components:**
- **Pixhawk 2.4.8**: Flight controller with integrated IMU, barometer, magnetometer
- **Raspberry Pi 5**: Central processing hub for MAVProxy, GStreamer, RTSP server
- **Teensy 4.1**: UDP-based servo control with real-time PWM generation
- **Multi-Camera System**: 3x USB cameras with H.264 encoding at 30fps, 1280x720

**Software Stack:**
- MAVLink protocol for telemetry
- GStreamer for H.264 video streaming
- Python for control scripts (PS5 controller, keyboard interface)
- Systemd services for auto-start capabilities

**Control Methods:**
1. **Autonomous Mode**: QGroundControl waypoint navigation
2. **Manual (PS5)**: Real-time joystick control with exponential curves and deadband filtering
3. **Keyboard**: Fine-grain servo adjustment via terminal interface

### PAM + TDM 4-Servo MATLAB Simulator

**Course:** EECE-310 Signals & Communications

**Technical Specifications:**
- 8 kHz master clock (125 µs frame period)
- 4 independent TDM channels with configurable time slots
- Butterworth 4th-order low-pass filter for signal recovery
- Real-time oscilloscope visualization at 25 FPS

**Signal Processing Chain:**
1. **Modulation**: Message sine waves (2kHz, 1kHz, 1.5kHz, 0.8kHz)
2. **PAM**: Pulse amplitude modulation with adjustable pulse width (5-28 µs)
3. **TDM**: Non-overlapping time multiplexing on shared channel
4. **Demux**: TDM × Q_k multiplication for channel extraction
5. **Recovery**: Butterworth LPF for signal reconstruction

**Implementation Highlights:**
- Vectorized MATLAB operations (no loops for performance)
- Pre-allocated graphics handles for smooth animation
- Interactive sliders for parameter exploration
- Multiple synchronized waveform displays (pulse trains, TDM, recovery)

---

## 💼 Technical Skills Matrix

### Software Development
| Language | Proficiency | Applications |
|----------|------------|--------------|
| Python | Expert | Control systems, GStreamer integration, PS5/Keyboard handlers |
| C/C++ | Advanced | Teensy firmware, embedded servo control, real-time processing |
| MATLAB | Advanced | Signal processing, filter design, real-time visualization |
| JavaScript | Intermediate | Web interfaces, interactive dashboards |

### Robotics & Control
- Pixhawk autopilot programming and configuration
- MAVLink protocol implementation
- Real-time servo control systems
- 6-DOF motion planning
- Autonomous navigation algorithms
- Control loop tuning and optimization

### Signal Processing
- Digital filter design (Butterworth, Chebyshev, Elliptic)
- PAM/TDM multiplexing systems
- Signal demodulation and recovery
- Fourier analysis and spectrum analysis
- Real-time signal visualization

### Hardware & Embedded Systems
- Pixhawk integration with Raspberry Pi
- Teensy microcontroller programming
- UDP/Ethernet communication protocols
- PWM generation and servo control
- Sensor calibration and integration
- Low-latency system architecture

### Communications
- Ethernet tether systems for underwater vehicles
- UDP and TCP protocol implementation
- RTSP video streaming setup
- GStreamer pipeline configuration
- MAVLink telemetry forwarding
- Low-latency optimization techniques

---

## 📚 Publications & Research

### STI 2025 Conference
- **Award**: Best Paper Award
- **Topic**: Advanced Control Systems for Underwater Robotics
- **Focus**: Real-time multi-sensor integration and autonomous navigation

### Research Profiles
- **Google Scholar**: [Md Farhan Mohib Hemal](https://scholar.google.com)
- **ResearchGate**: [Md Farhan Hemal](https://www.researchgate.net/profile/Md-Farhan-Hemal)
- **LinkedIn**: [Md Farhan Mohib Hemal](https://www.linkedin.com/in/md-farhan-mohib-hemal-467589258)

---

## 🎓 Education

**Bachelor of Science in Electrical Electronic & Communication Engineering**
- Institution: Military Institute of Science and Technology (MIST)
- Status: Current student
- Specialization: Robotics, Control Systems, Embedded Electronics

**Higher Secondary Certificate (HSC)**
- Grade: A+
- Score: 96% in Physics, Chemistry, Mathematics (PCM)
- Institution: Bangladesh Noubahini School & College, Chattogram

**Secondary School Certificate (SSC)**
- Grade: A+
- Institution: Bangladesh Noubahini School & College, Chattogram

---

## 🔗 Connect & Collaborate

- **WhatsApp**: [+88 01824100874](https://wa.me/+8801824100874)
- **Email**: [farhanmohibhemal@gmail.com](mailto:farhanmohibhemal@gmail.com)
- **LinkedIn**: [md-farhan-mohib-hemal-467589258](https://www.linkedin.com/in/md-farhan-mohib-hemal-467589258)
- **GitHub**: [farhan-mohib](https://github.com/farhan-mohib)
- **Facebook**: [farhan.mohib.2024](https://www.facebook.com/farhan.mohib.2024)

---

## 📁 Repository Structure

```
portfolio-FMH/
├── index.html              # Main portfolio website
├── README.md              # This file
└── projects/              # Detailed project documentation
    ├── mavirov-rov/       # ROV software architecture
    ├── pam-tdm-simulator/ # MATLAB signal processing project
    └── research/          # Research papers and documentation
```

---

## 🛠️ Technologies & Tools

### Development Environments
- MATLAB R2023a+
- Python 3.8+
- VS Code / JetBrains IDEs
- Git & GitHub

### Frameworks & Libraries
- MAVProxy (autopilot communication)
- GStreamer (multimedia framework)
- pygame (joystick handling)
- SciPy / NumPy (scientific computing)
- OpenCV (computer vision)

### Hardware Platforms
- Pixhawk 2.4.8 (autopilot)
- Raspberry Pi 5 (processing)
- Teensy 4.1 (microcontroller)
- Arduino ecosystem

### Development Tools
- Systemd (service management)
- RTSP servers
- UDP/TCP networking
- H.264 video encoding

---

## 📖 Documentation

Comprehensive documentation for major projects is available in individual project folders:
- Architecture diagrams (to be added)
- Technical specifications
- Installation & setup guides
- API documentation
- Code examples

---

## 🎯 Current Focus

- Advanced control algorithms for underwater vehicles
- Real-time signal processing optimization
- Multi-camera streaming integration
- Autonomous mission planning systems
- Research paper publication in peer-reviewed journals

---

## 🤝 Collaboration

I am actively seeking collaborations in:
- Underwater robotics research
- Advanced control systems design
- Real-time signal processing applications
- Open-source robotics projects
- Academic partnerships with research institutions

---

## 📄 License

This portfolio and associated documentation are provided for professional and educational purposes. Projects may have individual licensing terms.

---

## ✨ About This Portfolio

This repository serves as a comprehensive showcase of my professional work, research contributions, and technical expertise. It demonstrates my capabilities in software architecture, robotics systems engineering, and advanced signal processing across both academic and practical applications.

**Last Updated**: March 2024  
**Version**: 2.0 (Professional Technical Edition)

---

*"Engineering excellence through innovation, precision, and continuous learning."*
