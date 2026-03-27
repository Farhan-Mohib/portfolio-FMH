# Md Farhan Mohib Hemal | Underwater Robotics Engineer & Signal Processing Researcher

**Professional Portfolio | Advanced Engineering Solutions | Research & Innovation**

---

## 📋 Professional Overview

Welcome to my comprehensive professional portfolio and research documentation. This repository showcases my contributions to underwater robotics engineering, advanced signal processing, real-time control systems, and embedded systems design. As an Electrical Engineering student at Military Institute of Science and Technology (MIST), I combine rigorous academic foundations with practical, production-grade engineering implementations.

My work spans multiple domains of advanced engineering:
- **Autonomous Systems**: Complete software architectures for underwater robotics platforms
- **Signal Processing**: Real-time signal analysis, filtering, and communication systems
- **Embedded Systems**: Microcontroller programming and sensor integration
- **Control Systems**: Real-time control loops, adaptive algorithms, and system optimization

This portfolio demonstrates both breadth (diverse project domains) and depth (rigorous technical implementation) in modern engineering practice.

### Core Professional Highlights

- 🏆 **STI 2025 Best Paper Award** - Advanced Control Systems for Underwater Robotics Research
- 🤖 **Co-founder & Software Team Lead** - Mavirov Underwater ROV Platform (Production Deployment)
- 📊 **Published Researcher** - Active publications on Google Scholar & ResearchGate
- 🎓 **Academic Excellence** - A+ in HSC with 96% in Physics, Chemistry, Mathematics (PCM)
- 💼 **Full-Stack Engineer** - From embedded firmware to system architecture
- 🔬 **Research-Driven** - Rigorous methodology, experimental validation, peer-review

---

## 🎯 Professional Research & Technical Areas

### 1. Underwater Robotics & Autonomous Systems
**Domain**: Advanced robotics, autonomous vehicles, real-time control, sensor fusion

- Pixhawk-based autopilot integration and programming
- Real-time multi-sensor telemetry architecture
- Autonomous navigation and path planning algorithms
- Multi-sensor fusion for underwater environment estimation
- Low-latency command and control systems
- QGroundControl integration and mission planning

**Applications**: Subsea research, underwater inspection, autonomous surveying, deep-sea exploration

### 2. Signal Processing & Communication Systems
**Domain**: Digital signal processing, communication theory, filter design, modulation schemes

- Pulse Amplitude Modulation (PAM) system design and implementation
- Time Division Multiplexing (TDM) signal interleaving
- Digital filter design (Butterworth, Chebyshev, Elliptic filters)
- Signal recovery and demodulation techniques
- Real-time signal visualization and analysis
- Communication protocol optimization

**Applications**: Data transmission, noise rejection, signal quality enhancement, wireless communications

### 3. Embedded Systems & Hardware Integration
**Domain**: Microcontroller programming, sensor integration, real-time systems, IoT applications

- ESP32 microcontroller programming and configuration
- BLE (Bluetooth Low Energy) wireless communication
- 9-axis IMU sensor fusion and calibration
- I2C and SPI communication protocols
- Real-time data acquisition and processing
- Power optimization and battery management

**Applications**: Wireless sensor networks, IoT devices, wearable systems, remote monitoring

---

## 🔬 Published Research

### SAM-Augmented Blending for Enhanced Microplastic Detection Using YOLO11

**Status:** ✅ Published | ✅ Peer-Reviewed | ✅ Novel Contribution

**Research Focus:** Underwater microplastic pollution detection using advanced computer vision, synthetic data augmentation, and deep learning object detection for environmental monitoring.

**Core Innovation:**
Developed novel SAM-guided synthetic data augmentation framework combining Segment Anything Model for automated mask generation with intelligent compositing strategies (blending and copy-paste) to address critical data scarcity challenge in environmental monitoring.

**Technical Approach:**

**1. Segment Anything Model (SAM) for Automated Mask Generation**
- Eliminates manual annotation bottleneck
- Generates high-quality pixel-level segmentation masks
- Enables rapid synthetic dataset scaling
- No expert annotation required

**2. Synthetic Augmentation Strategies**
- **Blending Augmentation**: Soft Gaussian blending at object boundaries to simulate natural underwater integration
- **Copy-Paste Augmentation**: Direct compositing for computationally efficient alternative
- **Background Diversity Analysis**: Evaluates 1BG vs 3BG configurations for contextual variability

**3. Detection Architecture: YOLOv11**
- Latest version with enhanced backbone architecture
- Real-time processing capability
- Robust handling of small, densely-distributed objects
- Proven effectiveness in similar underwater detection domains

**Experimental Results:**

| Model Configuration | mAP@0.5 | mAP@0.5:0.95 | Improvement |
|-------------------|---------|--------------|-------------|
| Original Dataset (Baseline) | 0.7382 | 0.3045 | — |
| **SAM + Blended (1BG)** | **0.7497** | **0.3133** | **+2.89%** ✅ |
| SAM + Blended (3BG) | 0.7403 | 0.3091 | +1.51% |
| SAM + Copy-Paste (1BG) | 0.7482 | 0.3118 | +2.40% |
| SAM + Copy-Paste (3BG) | 0.7429 | 0.3116 | +2.33% |

**Key Findings:**
- ✅ All synthetic augmentation variants improved baseline performance
- ✅ SAM + Blended (1BG) achieved highest accuracy (mAP@0.5:0.95 = 0.3133)
- ✅ Background diversity shows diminishing returns beyond single background
- ✅ Copy-paste offers comparable performance with lower computational cost
- ✅ Synthetic-to-real transfer learning validates practical deployment potential

**Applications:**
- Real-time environmental monitoring in aquatic systems
- Autonomous drone-based water quality assessment
- Large-scale microplastic distribution mapping
- Integration into oceanographic monitoring platforms
- Scalable solution for resource-constrained environments

**Research Significance:**
- First comprehensive evaluation of SAM-guided synthetic augmentation for microplastic detection
- Novel blending compositing technique for realistic training data generation
- Practical framework for addressing data scarcity in environmental monitoring
- Methodologically sound approach with potential for other small-object detection domains

**Documentation:** [Full Research Paper](MICROPLASTIC_DETECTION_RESEARCH.md) | [PDF](SAM_Augmentation_Paper.pdf)

---

## 🚀 Major Projects

### Mavirov ROV - Autonomous Underwater Robotics Platform

**Professional Role:** Co-founder & Software Team Lead  
**Status:** Production Deployment | Active Development

**Project Objective:**
Design and implement a complete software architecture for a fully autonomous underwater remotely-operated vehicle (ROV) capable of subsea exploration, research, and industrial applications. The system prioritizes low-latency control, real-time telemetry, autonomous navigation, and modular design for scalability.

**Architecture Overview:**
```
┌─────────────────────────────────────────────────────────────┐
│                    Subsea Component (ROV)                   │
├─────────────────────────────────────────────────────────────┤
│  Pixhawk 2.4.8      │    Raspberry Pi 5      │   Teensy 4.1 │
│ Flight Controller   │  Central Processing    │  Servo Driver │
│ (Autopilot/Nav)     │  (Telemetry/Streaming) │   (6-DOF Arm) │
└──────────┬──────────┴────────────┬───────────┴────────┬──────┘
           │                       │                   │
        Sensors              Multi-Camera           6-DOF
        (IMU/Depth)          System                 Manipulator
           │                       │                   │
└──────────┴───────────────────────┴───────────────────┘
       Ethernet Tether (Bidirectional Communication)
           │
┌──────────▼────────────────────────────────────────┐
│         Surface Station (Pilot/Control)            │
├──────────────────────────────────────────────────┤
│  • QGroundControl (Autonomous Missions)            │
│  • Video Viewer (Multi-camera RTSP)               │
│  • Control Interface (PS5/Keyboard)               │
│  • Telemetry Monitor (MAVLink/UDP)               │
└──────────────────────────────────────────────────┘
```

**Hardware Subsystems:**

1. **Pixhawk 2.4.8 Flight Controller**
   - 32-bit ARM processor with IMU, barometer, magnetometer
   - Autonomous waypoint navigation via Ardupilot firmware
   - Supports multiple flight modes (manual, stabilized, autonomous)
   - Real-time sensor fusion and attitude estimation
   - Depth sensing integration via barometric pressure

2. **Raspberry Pi 5 Central Processing Hub**
   - Primary telemetry aggregation and streaming
   - MAVProxy bridge for autopilot communication forwarding
   - GStreamer-based multi-camera video encoding and RTSP streaming
   - UDP servo control protocol implementation
   - Python-based control scripts and integrations

3. **Teensy 4.1 Microcontroller**
   - Real-time PWM servo control (6 channels, configurable)
   - UDP protocol implementation for servo commands
   - NativeEthernet support for tether communication
   - Deterministic servo response with millisecond-level accuracy
   - Firmware written in C++ for optimization

4. **Multi-Camera System**
   - 3 × USB cameras (640×480 or 1280×720 resolution)
   - H.264 video encoding at 30 FPS
   - GStreamer pipeline for efficient compression
   - Individual stream assignment (forward, side, gripper views)

**Software Implementation:**

**Component 1: Telemetry Bridge (MAVProxy)**
```
Pixhawk → USB/Serial → MAVProxy ← Parses MAVLink
                          ↓
                    UDP Multicast
                          ↓
QGroundControl ← Receives Telemetry
```

**Component 2: Video Streaming (GStreamer)**
```
USB Cameras → V4L2 (Video4Linux2) → GStreamer
                        ↓
                H.264 Encoding
                        ↓
                RTSP Server Port 8554
                        ↓
Network Receivers (VLC, RTSP Viewers)
```

**Component 3: Servo Control (UDP Protocol)**
```
Control Input (PS5/Keyboard) → Python Script
        ↓
JSON Command over UDP Port 5005
        ↓
Teensy MCU → Parses Command
        ↓
PWM Signal → 6 Servo Channels (500-2500 µs)
```

**Control Methods Implemented:**

1. **Autonomous Mode** (QGroundControl Integration)
   - Waypoint mission planning
   - Altitude hold and depth maintenance
   - Automated return-to-home functionality
   - Conditional mission logic (IF-THEN)

2. **Manual Control via PS5 Controller**
   - Dual-stick configuration (6-DOF control)
   - Exponential response curves for precise control
   - Deadband filtering (eliminates stick drift)
   - Exponential moving average (EMA) smoothing
   - Rate limiting to prevent servo oscillation

3. **Keyboard Fine-Control**
   - Individual servo adjustment (+/- PWM steps)
   - Binary thrust vectoring (forward/backward/left/right)
   - Terminal-based interface for scripting

**Technical Achievements:**
- ✅ Sub-100ms control latency over tether
- ✅ Real-time 3-camera video streaming at 30 FPS
- ✅ Autonomous depth maintenance (±0.5 meters)
- ✅ Robust telemetry even with packet loss (>20% tolerance)
- ✅ Hot-swap camera configuration
- ✅ Systemd auto-start services for reliability

### PAM + TDM 4-Servo Real-time Signal Processor

**Technical Domain:** Digital Signal Processing & Communication Systems  
**Status:** Complete Implementation | Real-time System

**Project Objective:**
Develop a comprehensive MATLAB-based signal processing system demonstrating practical implementation of Pulse Amplitude Modulation and Time Division Multiplexing for communication systems. Focus on real-time signal generation, processing, and recovery demonstrating fundamental concepts in digital communications.

**Technical Specifications:**

**Signal Parameters:**
- Master clock frequency: 8 kHz (125 µs frame period)
- Independent TDM channels: 4
- Time slot duration: 31.25 µs per channel
- Message frequencies: 2 kHz, 1 kHz, 1.5 kHz, 0.8 kHz
- Pulse width range: 5-28 microseconds (adjustable)
- LPF cutoff frequency: 300-4000 Hz (adjustable)

**Real-Time Performance:**
- Visualization refresh rate: 25 FPS
- Display window: 2 ms (sliding)
- Sampling rate: 200 kHz (signal generation)
- Total signal outputs: 19 distinct waveforms
- CPU utilization: <30% (efficient vectorized operations)

**Signal Processing Architecture:**

```
STAGE 1: MESSAGE GENERATION
─────────────────────────────
Servo Angle (0-180°)
    ↓
Normalized Amplitude (0-1.0)
    ↓
Sinusoidal Message: A × sin(2πf_msg × t)
    ↓
Results: 4 message signals (f_msg for each channel)

STAGE 2: PULSE AMPLITUDE MODULATION
────────────────────────────────────
Message Signal × Binary Pulse Train
    ↓
PAM Output: Message Amplitude (when Q_k = 1), Zero (when Q_k = 0)
    ↓
Results: 4 PAM signals (pulse-modulated messages)

STAGE 3: TIME DIVISION MULTIPLEXING
────────────────────────────────────
Sum all PAM signals (non-overlapping)
    ↓
TDM Frame: [Ch1 Pulse | Ch2 Pulse | Ch3 Pulse | Ch4 Pulse]
           [0-31.25μs  | 31.25-62.5μs | ...     | 93.75-125μs]
    ↓
Result: Single TDM signal carrying 4 channels on shared medium

STAGE 4: DEMULTIPLEXING & RECOVERY
───────────────────────────────────
For each channel k:
  1. Demux = TDM × Q_k (selects only ch-k pulses)
  2. LPF (Butterworth 4th order, fc selected by user)
  3. Recovered Message ≈ Original message (quality depends on LPF)
    ↓
Results: 4 recovered signals (should match originals if LPF correct)
```

**Key Technical Features:**

1. **Vectorized MATLAB Operations**
   - No loops in signal generation (entire signal arrays computed in single statements)
   - Uses broadcasting and element-wise operations
   - Performance benefit: ~1000× faster than loop-based equivalent
   - Handles 200,000 samples per frame in milliseconds

2. **Pre-allocated Graphics Architecture**
   - All plot handles created once during setup
   - Only YData modified each frame (no object recreation)
   - Smooth animation despite complex signal updates
   - No memory leaks or fragmentation

3. **Non-overlapping Pulse Train Generation**
   - Each channel fires in its assigned time slot
   - Pulse width constraint: pw_us < 31.25 µs (prevents overlap)
   - Modulo-based timing detection (efficient detection of active slot)
   - Deterministic timing regardless of system load

4. **Butterworth Low-Pass Filter Implementation**
   - 4th-order Butterworth design (steep rolloff: -80 dB/decade)
   - Zero-phase filtering via filtfilt() (no time delay distortion)
   - Configurable cutoff frequency (300-4000 Hz)
   - Automatic redesign when user adjusts slider

**Applications:**

- Wireless communications (signal multiplexing)
- Optical communications (TDM systems)
- Satellite telemetry (bandwidth optimization)
- Telephone networks (legacy multiplexing)
- Software-defined radio (SDR) implementation
- Educational signal processing demonstrator

**Visualization Output:**

1. **Q-Pulse Trains** (4 plots): Shows which channel active each time slot
2. **Combined TDM Signal** (1 large plot): All 4 channels interleaved on shared medium
3. **Individual Channel Plots** (4 plots): Message sine wave (dashed) vs PAM pulses (solid)
4. **Recovery Plot** (1 large plot): Original messages vs recovered signals after filtering
5. **Servo Dials** (4 dials): Interactive amplitude control for each message

**Performance Metrics:**
- Rendering: 25 FPS (40 ms frame time)
- Signal computation: 1-2 ms per frame
- Graphics update: 1-3 ms per frame
- Idle time: 35+ ms (efficient CPU utilization)

**Implementation Innovations:**
- ✅ Glow rings highlight active servo (visual feedback)
- ✅ Servo needles track angles smoothly (EMA interpolation)
- ✅ Auto-scaling axes for signal visibility
- ✅ Color-coded channels (blue, green, amber, pink)
- ✅ Grid lines and slot boundaries for reference
- ✅ Real-time parameter adjustment with instant feedback

### Image Processing: Salt & Pepper Noise Reduction

**Technical Domain:** Digital Image Processing & Signal Processing  
**Status:** Complete Implementation | Educational Tool

**Project Objective:**
Develop a comprehensive MATLAB-based image restoration system demonstrating practical application of digital signal processing techniques for noise reduction in images. Focus on salt-and-pepper noise (impulse noise) removal using median filtering and advanced noise suppression algorithms.

**Technical Implementation:**

**Noise Characteristics:**
- Salt-and-pepper noise (impulse noise)
- Random white and black pixels
- Typical in satellite imagery, medical imaging, document scanning
- SNR varies from 10 dB to 40 dB in test cases

**Filtering Approaches:**
1. **Standard Median Filter**
   - Kernel sizes: 3×3 to 9×9
   - Simple and efficient
   - Good for moderate noise levels

2. **Weighted Median Filter**
   - Adaptive weighting based on pixel statistics
   - Better edge preservation
   - Higher computational complexity

3. **Morphological Operations**
   - Dilation and erosion
   - Adaptive structuring elements
   - Useful for binary or near-binary images

4. **Advanced Techniques**
   - Bilateral filtering (edge-preserving)
   - Non-local means filtering
   - Wavelet-based denoising

**Performance Metrics:**
- **PSNR** (Peak Signal-to-Noise Ratio): Measures reconstruction quality
- **SSIM** (Structural Similarity Index): Evaluates perceived quality
- **Processing Time**: Algorithm efficiency
- **Visual Quality**: Subjective assessment

**Implementation Features:**
- ✅ Batch image processing
- ✅ Real-time noise injection and filtering
- ✅ Comparative analysis framework
- ✅ Quality metric calculation
- ✅ Visualization of before/after results
- ✅ Parameter optimization tools

**Applications:**
- Medical image enhancement (CT, MRI scans)
- Satellite imagery restoration
- Historical document digitization
- Photographic restoration
- Astronomical image processing

**Key Achievements:**
- Achieved 25-30 dB PSNR improvement on standard test images
- Optimized filtering parameters for different noise levels
- Demonstrated trade-off between noise reduction and edge preservation
- Implemented efficient vectorized MATLAB code

---

### Practical Drone Building & Design (MIST)

**Institution:** Military Institute of Science and Technology  
**Role:** Educational Program Designer & Instructor  
**Status:** Completed | Hands-On Workshop

**Program Overview:**
Comprehensive hands-on educational program teaching practical drone design, construction, and operation at MIST. Program covers complete drone development cycle from component selection through final assembly, configuration, and flight testing. Emphasis on learning by doing - students build actual working quadrotor platforms from individual components.

**Program Curriculum:**

**Module 1: Drone Components & Selection**
- Flight controller selection (Pixhawk, APM, other platforms)
- Motor and propeller selection (thrust calculations, KV ratings)
- ESC (Electronic Speed Controller) specifications and matching
- Battery systems (LiPo selection, capacity, discharge rates)
- Frame selection and construction materials
- Sensor packages (IMU, barometer, compass, GPS)

**Module 2: Hands-On Assembly & Wiring**
- Frame construction from raw materials (3D printing, aluminum frames)
- Motor mounting and balancing techniques
- ESC wiring and signal connections
- Power distribution and soldering
- Flight controller installation and orientation
- Sensor mounting and alignment
- Telemetry module integration

**Module 3: Electrical Systems & Power Management**
- Battery management and charging procedures
- Safe power distribution design
- Voltage regulation and power budgeting
- Ground power modules and safety switches
- Cable management and vibration isolation
- Weight distribution and center of gravity calculation

**Module 4: Flight Controller Configuration**
- Flight controller firmware upload (Ardupilot, PX4)
- Motor calibration and arming procedures
- Sensor calibration (compass, accelerometer, gyro)
- Radio receiver binding and channel mapping
- PID tuning for stable flight
- Failsafe configuration and safety features

**Module 5: Testing & Troubleshooting**
- Pre-flight checklist development
- Ground-based testing procedures
- Motor spin tests and ESC verification
- First-flight procedures and safety protocols
- Performance measurement and analysis
- Common issues and fixes

**Practical Hands-On Activities:**

1. **Frame Assembly**
   - Build drone frame from components
   - Proper mounting and alignment
   - Structural integrity verification

2. **Electrical Assembly**
   - Solder connections (proper soldering technique)
   - Wire gauge selection and management
   - Power system integration

3. **Motor & Propeller Setup**
   - Motor KV selection for mission
   - Propeller balancing
   - Thrust verification

4. **Flight Controller Setup**
   - Hardware connection and configuration
   - Firmware installation
   - Sensor calibration procedures

5. **Flight Testing**
   - Controlled test flights
   - Performance measurement
   - Parameter tuning
   - Safety verification

**Materials & Tools Covered:**
- ✅ Soldering iron and solder selection
- ✅ Multimeter usage for diagnostics
- ✅ Propeller balancer operation
- ✅ Battery chargers and monitoring
- ✅ Flight controller configuration software
- ✅ RC transmitter programming

**Learning Outcomes:**
Students complete:
- ✅ Assembled, configured, working quadrotor drone
- ✅ Understanding of drone mechanical design
- ✅ Electrical system knowledge
- ✅ Flight controller programming skills
- ✅ Safe operation and testing procedures
- ✅ Troubleshooting and maintenance capability

**Educational Impact:**
- Transformed theoretical knowledge into practical skills
- Students gained hands-on engineering experience
- Created functioning drone platforms from scratch
- Developed understanding of system integration
- Established troubleshooting methodology
- Prepared students for advanced robotics projects

**Course Deliverables:**
- Building guides and step-by-step manuals
- Component selection spreadsheets
- Configuration checklists
- Safety procedures documentation
- Troubleshooting flowcharts
- Performance measurement protocols

---

**Role:** Embedded Systems Engineer

**System Architecture:**
```
BNO055 (9-Axis IMU) ↔ ESP32 Microcontroller ↔ BLE Central Device
  (Sensor Data)      (Processing & Fusion)    (Wireless Link)
       ↓                      ↓                     ↓
9-DOF Sensing      Real-time Calculations    Low-latency Stream
```

**Technical Specifications:**

**Hardware Components:**
- **Microcontroller**: ESP32 with dual-core processor
- **IMU Sensor**: BNO055 (9-axis: accelerometer, gyroscope, magnetometer)
- **Communication**: Bluetooth Low Energy (BLE) v5.0
- **I2C Interface**: SDA (GPIO4), SCL (GPIO6)
- **Sampling Rate**: 20 Hz (50 ms intervals)
- **Data Format**: Binary packed (76 bytes per packet)

**Sensor Data Processing:**

1. **Linear Acceleration**: 3-axis accelerometer compensated for gravity
2. **Gravity Vector**: Isolated gravity component from accelerometer
3. **Gyroscope**: 3-axis angular velocity measurement
4. **Magnetometer**: 3-axis magnetic field sensing
5. **Euler Angles**: Roll, Pitch, Yaw orientation
6. **Quaternion**: Unit quaternion (w, x, y, z) for orientation representation

**Calibration System:**
- Automatic calibration status monitoring (0-3 scale per axis)
- System calibration level tracking
- Gyroscope offset calibration
- Accelerometer calibration
- Magnetometer calibration

**BLE Protocol Design:**

**Packet Structure** (19 floats = 76 bytes):
```
Byte Offset  |  Data Field           | Size (bytes)
0-11         |  Linear Acceleration  | 12 (3 × float)
12-23        |  Gravity Vector       | 12 (3 × float)
24-35        |  Gyroscope Data       | 12 (3 × float)
36-47        |  Magnetometer Data    | 12 (3 × float)
48-59        |  Euler Angles         | 12 (3 × float)
60-75        |  Quaternion           | 16 (4 × float)
```

**BLE Configuration:**
- **MTU**: 185 bytes (maximum transmission unit)
- **TX Power**: P9 (maximum power level)
- **Service UUID**: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
- **Characteristic UUID**: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
- **Properties**: NOTIFY | READ
- **Update Rate**: 20 Hz (50 ms intervals)

**Key Features:**
- Real-time sensor fusion at 20 Hz sampling rate
- Efficient binary protocol for bandwidth optimization
- Automatic BLE reconnection and advertising
- Real-time calibration status monitoring
- Dual-core ESP32 exploitation for concurrent operations
- Low-latency wireless data transmission
- Memcpy-based efficient data packing

**Software Architecture:**
- Modular design with separate BLE and sensor handling
- Binary packing for efficient data transmission
- Timer-based data transmission scheduling (50 ms intervals)
- Callback-based BLE event handling
- Automatic reconnection on device disconnect

**Applications:**
- Underwater vehicle orientation tracking
- Real-time motion monitoring and analysis
- IMU calibration verification and diagnostics
- Wireless inertial measurement data logging
- Mobile device integration via Bluetooth Low Energy
- Drone and robotics orientation sensing

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

---

## 🏆 Certifications & Professional Recognition

### Academic Awards & Recognition

**STI 2025 Best Paper Award**
- **Organization**: STI 2025 Conference
- **Award**: Best Paper Award
- **Topic**: Advanced Control Systems for Underwater Robotics
- **Recognition**: Outstanding research contribution in subsea automation and autonomous navigation
- **Significance**: Demonstrates research excellence, innovation, and impact in underwater robotics domain

**Secondary & Higher Secondary Education Excellence**
- **HSC (Higher Secondary Certificate)**: A+ Grade
- **Score**: 96% in Physics, Chemistry, Mathematics (PCM)
- **Institution**: Bangladesh Noubahini School & College, Chattogram
- **SSC (Secondary School Certificate)**: A+ Grade
- **Recognition**: Consistent academic excellence in advanced science subjects

### Research & Publication Credentials

**Published Research**
- **Google Scholar Profile**: [Md Farhan Mohib Hemal](https://scholar.google.com)
- **ResearchGate Profile**: [Md Farhan Hemal](https://www.researchgate.net/profile/Md-Farhan-Hemal)
- **Research Focus**: Underwater robotics, autonomous systems, control engineering
- **Publications**: Peer-reviewed contributions and collaborative research
- **Engagement**: Active in research community, peer review participation

### Professional Certifications & Competencies

**Technical Expertise Certifications**

| Domain | Certification | Proficiency |
|--------|---------------|-------------|
| Embedded Systems | ESP32, Microcontroller Programming, Real-time Systems | Expert |
| Signal Processing | MATLAB, Digital Filter Design, DSP Theory | Advanced |
| Robotics Systems | Autonomous Control, Sensor Fusion, System Architecture | Expert |
| Wireless Communication | BLE Protocol, UDP/TCP, MAVLink Protocol | Advanced |
| Image Processing | MATLAB Image Toolbox, Noise Reduction, Filtering | Proficient |
| Drone Systems | UAV Flight Control, Autonomous Navigation | Advanced |

**Professional Experience**
- ✅ Co-founder & Software Team Lead - Mavirov ROV Platform
- ✅ Educational Program Designer - MIST Drone Systems Course
- ✅ Published Researcher - STI 2025 Conference
- ✅ Full-Stack Engineer - From embedded firmware to system architecture
- ✅ Teaching Professional - Curriculum development and instruction

### Industry Standards & Best Practices

**Software Development Standards**
- Professional code documentation and version control
- Modular architecture and design patterns
- Real-time systems optimization
- Embedded systems best practices
- Error handling and robust design

**Engineering Practices**
- Rigorous testing and validation
- Performance optimization and profiling
- System integration and deployment
- Technical documentation and communication
- Team coordination and leadership

**Research Methodology**
- Experimental design and validation
- Data analysis and interpretation
- Peer review participation
- Academic writing and publication
- Collaborative research contributions

---

## 📊 Professional Summary

### Key Strengths

**Technical Expertise**
- ✅ Advanced embedded systems design and implementation
- ✅ Real-time control systems and optimization
- ✅ Signal processing and communication systems
- ✅ Software architecture and system integration
- ✅ Autonomous systems and robotics

**Professional Qualities**
- ✅ Research-driven approach to problem-solving
- ✅ Leadership and team coordination abilities
- ✅ Educational communication and teaching skills
- ✅ Innovation and continuous learning mindset
- ✅ Attention to detail and quality assurance

**Project Experience**
- ✅ Complex system design and implementation
- ✅ Hardware-software integration
- ✅ Real-time performance optimization
- ✅ Production-grade code development
- ✅ Multi-disciplinary project collaboration

### Career Focus

**Current**: Advanced robotics, embedded systems, and autonomous systems engineering  
**Research Interest**: Underwater robotics, real-time control systems, advanced signal processing  
**Teaching Interest**: Aerial robotics, embedded systems, signal processing education  
**Innovation**: Novel approaches to autonomous navigation and system control

---

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
