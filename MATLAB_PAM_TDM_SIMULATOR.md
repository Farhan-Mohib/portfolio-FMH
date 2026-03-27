# MATLAB PAM/TDM 4-Channel Real-Time Simulator

**EECE-310 Communications Lab | Experiment 6 | Real-Time Oscilloscope Mode**

---

## 📋 Project Overview

A professional-grade real-time simulator built in MATLAB that replicates the full **Pulse Amplitude Modulation (PAM) and Time Division Multiplexing (TDM)** experiment from the EECE-310 Communications Lab. The GUI behaves like a digital oscilloscope — all four TDM channels update live at 25 fps with zero flicker, interactive sliders, and animated signal dials.

> ⚠️ **Note on the Dials:** The four circular dials in the GUI are purely a **visual teaching metaphor**. Each dial represents a virtual message signal source — dragging its slider changes the sine wave amplitude for that TDM channel. They are not physical servos and have no connection to any hardware.

### Key Capabilities

- ✅ **Real-time oscilloscope display** at 25 fps (40 ms timer) with zero-flicker animation
- ✅ **4-channel PAM/TDM simulation** on an 8 kHz clock (31.25 µs per slot)
- ✅ **Live pulse width control** (5–28 µs) matching the lab Width knob
- ✅ **Live Butterworth LPF** with adjustable cutoff for message recovery
- ✅ **Scrolling waveform** — signals flow across the screen like a real oscilloscope
- ✅ **TDM slot sequencing** — each channel activates in rotation every 4 seconds
- ✅ **IIR dial smoother** — dials track their targets gradually, simulating a real response
- ✅ **Full demultiplexing display** — recovered signals compared to originals in real time

---

## 🏗️ System Architecture

### Signal Processing Pipeline

```
Message Sine Waves  (4 independent channels)
    │
    │  Ch-1: A₁ · sin(2π · 2000 · t)    amplitude = dial_1 / 180
    │  Ch-2: A₂ · sin(2π · 1000 · t)    amplitude = dial_2 / 180
    │  Ch-3: A₃ · sin(2π · 1500 · t)    amplitude = dial_3 / 180
    │  Ch-4: A₄ · sin(2π ·  800 · t)    amplitude = dial_4 / 180
    │
    ▼
Pulse Trains Q1–Q4  (8 kHz clock, non-overlapping)
    │
    │  Q_k = 1 during slot k, 0 elsewhere
    │  Slot width = 125 µs ÷ 4 = 31.25 µs per channel
    │  Controlled by pw_us slider (5–28 µs actual pulse width)
    │
    ▼
PAM Generation      PAM_k = Message_k × Q_k
    │
    │  Analog switch logic — pulse height follows sine amplitude
    │  Result: sampled pulse train whose envelope = original message
    │
    ▼
TDM Multiplexing    TDM = PAM₁ + PAM₂ + PAM₃ + PAM₄
    │
    │  Pulses are non-overlapping → safe to sum
    │  All 4 channels share ONE signal line
    │
    ▼
Demultiplexing      Demux_k = TDM × Q_k
    │
    │  Gates out only channel k pulses
    │  Other channels are zeroed
    │
    ▼
Low-Pass Filter     Recovered_k = LPF( Demux_k )
    │
    │  6th-order Butterworth removes pulse harmonics
    │  Reveals original sine wave (cutoff set by lpf_hz slider)
    │
    ▼
Recovered Message Signals  (4 channels reconstructed)
```

### GUI Layout

```
┌─────────────────────────────────────────────────────────────────────────────┐
│       PAM + TDM  4-Channel Simulator  |  8 kHz Clock  |  EECE-310           │
├────────┬────────┬────────┬────────┬──────────────────────────────────────────┤
│  Dial  │  Dial  │  Dial  │  Dial  │  Q1   │  Q2   │  Q3   │  Q4   │ Buttons │
│  Ch-1  │  Ch-2  │  Ch-3  │  Ch-4  │ pulse │ pulse │ pulse │ pulse │ START   │
│[Slider]│[Slider]│[Slider]│[Slider]│       │       │       │       │ STOP    │
│        │        │        │        │  PW Slider      LPF Slider    │ RESET   │
├────────┴────────┴────────┴────────┴──────────────────────────────────────────┤
│              Combined PAM / TDM Signal  (125 µs clock period)                │
│  ▐Ch1▌  ▐Ch2▌  ▐Ch3▌  ▐Ch4▌  ▐Ch1▌  ▐Ch2▌  ...  colour-coded per channel  │
├──────────────┬──────────────┬──────────────┬────────────────────────────────┤
│  Ch-1  2kHz  │  Ch-2  1kHz  │ Ch-3 1.5kHz  │     Ch-4  0.8kHz              │
│ Msg + PAM    │ Msg + PAM    │ Msg + PAM    │     Msg + PAM                  │
├──────────────┴──────────────┴──────────────┴────────────────────────────────┤
│      Demux & Recovery  —  TDM × Q_k → LPF = Recovered Message               │
│  Ch1 ───  Ch2 ───  Ch3 ───  Ch4 ───   (dashed=original, solid=recovered)    │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Technical Implementation

### Fixed System Parameters

```matlab
Fs      = 200e3;                  % Sampling rate: 200 kHz
                                  % High enough to resolve a 5 µs pulse cleanly

f_clk   = 8000;                   % 8 kHz TDM clock (matches lab Master Signals module)

num_ch  = 4;                      % 4 TDM channels (lab uses 2; this extends to 4)

f_msg   = [2000 1000 1500 800];   % Message frequencies per channel (Hz)

T_clk   = 1/f_clk;                % Clock period = 125 µs (one full TDM frame)

slot_us = T_clk*1e6/num_ch;       % Each channel slot = 31.25 µs

win_dur = 0.002;                   % Display window = 2 ms (oscilloscope timebase)

N       = round(Fs*win_dur);       % Samples per window = 400
```

### Mutable State (Changes During Simulation)

```matlab
ang    = [90 45 135 60];  % Current dial angle (degrees, 0–180)
                          % Amplitude of message sine = angle / 180

tgt    = [90 45 135 60];  % Target angle set by slider
                          % Dial only moves toward tgt during its active 4 s slot

pw_us  = 18;              % PAM pulse width in microseconds (Width knob equivalent)

lpf_hz = 2500;            % LPF cutoff frequency in Hz (Tuneable LPF equivalent)

phase  = 0;               % Scrolling phase — increases 0.04 s per tick

cur    = 1;               % Currently active TDM slot (1–4), cycles every 4 s
```

### Colour Scheme

```matlab
C   = [0.18 0.68 1.00;   % Ch-1  sky-blue
       0.10 0.90 0.50;   % Ch-2  green
       1.00 0.64 0.10;   % Ch-3  amber
       0.92 0.24 0.44];  % Ch-4  pink-red

BG  = [0.08 0.10 0.16];  % Dark navy background
PAN = [0.13 0.15 0.23];  % Panel / axes fill
TXT = [0.92 0.94 0.98];  % White text
DIM = [0.38 0.40 0.46];  % Dimmed grey for minor labels
```

---

## ⏱️ Animation — How Zero-Flicker Works

The key to smooth real-time rendering in MATLAB is that **all plot handles are created once at startup** and reused every frame. Only `YData` is swapped — no axes, lines, or legends are ever recreated mid-animation.

```matlab
% ── One-time creation at startup (slow path) ──
pam_h(ch) = plot(chax(ch), tms, zeros(N,1), 'Color', C(ch,:), 'LineWidth', 1.4);

% ── Every 40ms frame (fast path — data swap only) ──
set(pam_h(ch), 'YData', new_pam_signal);
```

This applies to all 11 simultaneous plot axes: 4 Q pulse plots, 1 TDM plot, 4 channel PAM plots, and 1 recovery plot.

### Timer Configuration

```matlab
tmr = timer('ExecutionMode', 'fixedRate', ...
            'Period',     0.04, ...       % 40 ms = 25 fps
            'TimerFcn',   @tCB, ...
            'BusyMode',   'drop');        % skip frame if previous isn't done yet
```

`BusyMode = 'drop'` prevents frame queuing — if a tick takes longer than 40 ms (e.g. during a slider drag), the next tick is simply skipped rather than piling up. This keeps the animation smooth and prevents the GUI from freezing.

---

## 📊 TDM Slot Sequencing

```matlab
function tCB(~,~)
    phase = phase + 0.04;                       % advance scrolling phase

    cur = mod(floor(phase/4), num_ch) + 1;      % active slot cycles every 4 s

    ang(cur) = ang(cur)*0.75 + tgt(cur)*0.25;   % IIR smoother tracks target
    fast();                                      % redraw all plots
end
```

### IIR Dial Smoother

```
alpha = 0.25  →  each tick moves 25% of remaining distance to target
                 Converges to within 1% in ~17 ticks ≈ 0.68 seconds
```

When a channel's slot is not active, dragging the slider still updates `tgt` immediately — but the dial only physically moves when that channel's 4-second window arrives. This models how a real TDM system queues a new value and applies it at the next available time slot.

---

## 🔬 Butterworth LPF Design

```matlab
function mkLPF()
    Wn = lpf_hz / (Fs/2);                      % normalize to Nyquist
    [lpf_b, lpf_a] = butter(6, Wn, 'low');     % 6th-order Butterworth
end
```

Called once on startup, then again whenever the LPF slider moves. The 6th-order design provides a steep roll-off that cleanly separates message frequencies from clock harmonics — the effect is directly visible in the recovery plot as the cutoff slider is dragged.

### LPF Cutoff Guidelines

| Cutoff Setting | Observed Effect |
|---|---|
| Below message frequency | Signal disappears — over-filtered |
| Just above message frequency | Clean sine recovery — ideal |
| Too high (near 4 kHz) | Pulse harmonics bleed through — noisy recovery |

---

## 📐 Lab Mapping — Experiment 6 Correlation

| Lab Hardware Component | MATLAB Equivalent | Notes |
|---|---|---|
| Master Signals 8 kHz clock | `f_clk = 8000 Hz` | Fixed system parameter |
| Twin Pulse Generator Q1–Q4 | `qs(:,1..4)` | 4 pulse trains, each offset by one slot |
| Width knob | `pw_us` slider (5–28 µs) | Max = slot_us−1 to prevent overlap |
| Dual Analog Switch | `PAM_k = Message_k × Q_k` | Multiplication = gating logic |
| TDM bus output | `tdm = PAM₁+PAM₂+PAM₃+PAM₄` | Non-overlapping — safe to sum |
| Multiplier module | `demux_k = tdm × Q_k` | "Steals" clock to extract channel |
| Tuneable LPF | `butter(6, lpf_hz/(Fs/2))` | Redesigned live on slider change |
| Message source dial | Sine amplitude = `ang/180` | Visual teaching device — not hardware |

---

## 📈 Lab Questions Demonstrated

| Question | Answer Visible in Simulator |
|---|---|
| Q3: Effect of pulse width on TDM capacity | Narrow `pw_us` → more gap between pulses → room for more channels |
| Q7: Pulses per cycle per channel | 8 kHz ÷ 2 kHz = **4 pulses/cycle** for Ch-1 (visible in channel plot) |
| Q8: Total TDM pulses per cycle | 4 channels × 4 = **16 pulses/cycle** in the TDM plot |
| Q10: Effect of pulse width on recovery amplitude | Narrow pulses → smaller LPF output → reduced recovered amplitude |

---

## 💻 Code Architecture

### Main Function Structure

```
PAM_TDM_4Servo_4sec()
├── Section 1 — Fixed Parameters    (Fs, f_clk, f_msg, slot timing)
├── Section 2 — Mutable State       (ang, tgt, pw_us, lpf_hz, phase, cur)
├── Section 3 — Colour Scheme
├── Section 4 — LPF Design          mkLPF()
├── Section 5 — Figure & UI         (sliders, buttons, dials)
├── Section 6 — Q Pulse Axes        (4 small pulse train scopes)
├── Section 7 — TDM Axis            (combined multiplexed signal)
├── Section 8 — Channel Axes        (4 individual PAM + message plots)
├── Section 9 — Recovery Axis       (demux + LPF output vs original)
└── Nested Functions
    ├── fast()       — recomputes and sets all YData each frame
    ├── tCB()        — timer callback (phase advance + slot sequencing)
    ├── cbAng()      — dial slider callback
    ├── cbPW()       — pulse width slider callback
    ├── cbLPF()      — LPF cutoff slider callback
    ├── onStart()    — creates and starts timer
    ├── onStop()     — stops and deletes timer
    ├── onReset()    — restores all defaults
    ├── onClose()    — safe figure close (stops timer first)
    ├── mkLPF()      — designs Butterworth filter coefficients
    └── initDials()  — draws all 4 dial faces (static, one-time only)
```

### fast() — Core Render Function

Called every 40 ms by the timer. Computes all signals using vectorised math (no sample-level loops) and pushes results to pre-existing plot handles:

```matlab
% 1. Build scrolling time axis
t = (0:N-1)'/Fs + phase;

% 2. Compute message sine waves — amplitude set by dial angle
msgs = sin(2*pi*f_msg.*t) .* (ang/180);

% 3. Build Q pulse trains (digital slot gates)
for k = 1:num_ch
    slot_start = (k-1) * slot_us * 1e-6;
    qs(:,k) = (mod(t, T_clk) >= slot_start) & ...
              (mod(t, T_clk) <  slot_start + pw_us*1e-6);
end

% 4. PAM = message × pulse gate
pam = msgs .* qs;

% 5. TDM = all channels on one line
tdm = sum(pam, 2);

% 6. Demux + LPF per channel
for k = 1:num_ch
    demux         = tdm .* qs(:,k);
    recovered(:,k) = filter(lpf_b, lpf_a, demux);
end

% 7. Update plot handles (no redraw)
set(pam_h(ch),  'YData', pam(:,ch));
set(rec_h(ch),  'YData', recovered(:,ch));
set(msg_h(ch),  'YData', msgs(:,ch));
% ... Q pulse and TDM handles updated similarly
```

---

## 🛠️ Interactive Controls Reference

| Control | Range | Lab Equivalent |
|---|---|---|
| Dial sliders × 4 | 0°–180° | Message source amplitude |
| Pulse Width slider | 5–28 µs | WIDTH knob on Twin Pulse Generator |
| LPF Cutoff slider | 300–4000 Hz | Tuneable LPF cut-off control |
| START | — | Starts 25 fps timer animation |
| STOP | — | Pauses animation (waveforms freeze) |
| RESET | — | Restores all values to defaults |

---

## 📊 Performance Metrics

| Metric | Value |
|---|---|
| Frame rate | 25 fps (40 ms timer period) |
| Display sampling rate | 200 kHz |
| Display window | 2 ms (oscilloscope timebase) |
| Samples per frame | 400 |
| TDM clock | 8 kHz |
| TDM frame period | 125 µs |
| Slot width (4 channels) | 31.25 µs |
| LPF order | 6th-order Butterworth |
| Dial IIR alpha | 0.25 (~17 ticks to converge) |
| Active slot duration | 4 seconds per channel |

---

## 📚 Requirements

```
MATLAB R2021a or newer
Signal Processing Toolbox   — butter(), filter()
No additional toolboxes required
```

---

## 🚀 How to Run

```matlab
% In the MATLAB command window:
PAM_TDM_4Servo_4sec()

% 1. Press START to begin animation
% 2. Drag dial sliders to change message amplitudes per channel
% 3. Drag Pulse Width slider — observe TDM pulse spacing change
% 4. Drag LPF Cutoff slider — observe recovery quality change
% 5. Press STOP to pause and inspect waveforms at any moment
% 6. Press RESET to restore all defaults
% 7. Close the window — timer stops safely before figure deletes
```

---

## 📄 Conclusion

This MATLAB simulator delivers a complete, interactive recreation of EECE-310 Experiment 6:

✅ **Zero-flicker 25 fps rendering** — achieved through handle reuse and `BusyMode='drop'` timer discipline

✅ **Direct lab correlation** — every hardware module in Experiment 6 maps to a named parameter or function in the code

✅ **Live parameter exploration** — pulse width and LPF cutoff sliders let students observe the effect of each variable in real time without rewiring hardware

✅ **Correct TDM slot sequencing** — channels activate in rotation, faithfully modelling how a real TDM system allocates time slots across multiple signal sources

✅ **Vectorised computation** — all signal math runs without sample-level loops, keeping each frame comfortably within the 40 ms budget

---

**Project Author:** Md Farhan Mohib Hemal
**Course:** EECE-310 Communications Lab — Experiment 6
**Status:** Production-Ready
**Version:** 1.0
