# MATLAB PAM/TDM 4-Channel Real-Time Simulator

**EECE-310 Communications Lab | Experiment 6 | Real-Time Oscilloscope Mode**

**Project Author:** Md Farhan Mohib Hemal
**Course:** EECE-310 Communications Lab — Experiment 6
**Status:** Production-Ready | **Version:** 1.0

---

## 📋 Project Overview

A professional-grade real-time simulator built in MATLAB that replicates the full **Pulse Amplitude Modulation (PAM) and Time Division Multiplexing (TDM)** experiment from the EECE-310 Communications Lab. The GUI behaves like a digital oscilloscope — all four TDM channels update live at 25 fps with zero flicker, interactive sliders, and animated signal dials.

> ⚠️ **Note on the Dials:** The four circular dials are purely a **visual teaching metaphor**. Each represents a virtual message signal source — dragging its slider changes the sine wave amplitude for that channel. They are not physical servos and have no connection to hardware.

### Key Capabilities

- ✅ **Real-time oscilloscope display** at 25 fps (40 ms timer) with zero-flicker animation
- ✅ **4-channel PAM/TDM simulation** on an 8 kHz clock (31.25 µs per slot)
- ✅ **Live pulse width control** (5–28 µs) matching the lab Width knob
- ✅ **Live 6th-order Butterworth LPF** with adjustable cutoff for message recovery
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
    │  Other channels become zero
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

## 💻 Full Annotated Source Code

```matlab
function PAM_TDM_4Servo_4sec()
% ============================================================================
% PAM_TDM_4Servo_4sec  —  Real-Time PAM/TDM 4-Channel Simulator
% ============================================================================
%
% OVERVIEW:
%   Interactive MATLAB GUI that simulates the full EECE-310 Experiment 6
%   PAM/TDM system in real time. Generates four independent message sine
%   waves, gates each with a non-overlapping pulse train (TDM clock),
%   multiplexes them onto a single TDM bus, then demultiplexes and
%   low-pass-filters each channel to recover the original signals.
%
% USAGE:
%   >> PAM_TDM_4Servo_4sec()
%   Press START → drag sliders → observe live waveform changes.
%
% REQUIREMENTS:
%   MATLAB R2021a+, Signal Processing Toolbox (butter, filter)
%
% ============================================================================

% ============================================================================
% SECTION 1 — FIXED SYSTEM PARAMETERS
% ============================================================================
%
% These values match the EECE-310 lab hardware and are never changed at
% runtime. They define the fundamental timing and frequency grid.

Fs      = 200e3;                  % Sampling rate: 200 kHz
                                  % Resolves a 5 µs pulse with 1 sample precision.
                                  % Nyquist for 8 kHz × harmonics comfortably met.

f_clk   = 8000;                   % TDM clock: 8 kHz (matches lab Master Signals module)
                                  % → Frame period T_clk = 125 µs

num_ch  = 4;                      % Number of TDM channels
                                  % Lab uses 2; this extends to 4 for richer demo.

f_msg   = [2000 1000 1500 800];   % Message sine frequencies per channel (Hz)
                                  % Ch-1: 2 kHz, Ch-2: 1 kHz,
                                  % Ch-3: 1.5 kHz, Ch-4: 800 Hz

T_clk   = 1/f_clk;                % Clock period = 1/8000 = 125 µs (one TDM frame)

slot_us = T_clk*1e6/num_ch;       % Slot width = 125/4 = 31.25 µs per channel
                                  % Pulses must fit inside this window.

win_dur = 0.002;                   % Oscilloscope display window = 2 ms
                                  % Shows 16 complete clock cycles (8kHz × 0.002s).

N       = round(Fs*win_dur);       % Samples per display window = 200kHz × 0.002s = 400

% ============================================================================
% SECTION 2 — MUTABLE STATE  (values that change during simulation)
% ============================================================================
%
% These variables are captured by nested functions via MATLAB's closure
% mechanism. Modifying them inside any nested function updates the shared state.

ang    = [90 45 135 60];          % Current dial angle (degrees, range 0–180)
                                  % Sine amplitude = ang(k) / 180
                                  % Default: Ch1=0.5, Ch2=0.25, Ch3=0.75, Ch4=0.33

tgt    = [90 45 135 60];          % Target dial angle set by the slider
                                  % Dial moves toward tgt only during its active slot.
                                  % Models TDM queuing — value applied at next slot.

pw_us  = 18;                      % PAM pulse width in microseconds (lab: Width knob)
                                  % Range: 5–28 µs. Max = slot_us−1 to prevent overlap.

lpf_hz = 2500;                    % LPF cutoff frequency in Hz (lab: Tuneable LPF knob)
                                  % Range: 300–4000 Hz.

phase  = 0;                       % Scrolling phase offset (seconds)
                                  % Incremented by 0.04 s per tick → signals scroll left.

cur    = 1;                       % Currently active TDM slot (1–4)
                                  % Cycles every 4 seconds: cur = floor(phase/4) mod 4 + 1

% ============================================================================
% SECTION 3 — COLOUR SCHEME
% ============================================================================
%
% Dark-theme oscilloscope palette. RGB values are normalised (0–1).
% Each channel has a distinct hue for easy visual separation.

C   = [0.18 0.68 1.00;            % Ch-1  sky-blue
       0.10 0.90 0.50;            % Ch-2  green
       1.00 0.64 0.10;            % Ch-3  amber
       0.92 0.24 0.44];           % Ch-4  pink-red

BG  = [0.08 0.10 0.16];           % Background: dark navy
PAN = [0.13 0.15 0.23];           % Panel / axes fill
TXT = [0.92 0.94 0.98];           % Primary text: near-white
DIM = [0.38 0.40 0.46];           % Dimmed grey for minor labels

% ============================================================================
% SECTION 4 — LPF DESIGN  (runs once at startup + on every LPF slider move)
% ============================================================================

lpf_b = []; lpf_a = [];           % Filter coefficient vectors (initialised in mkLPF)
mkLPF();                          % Design initial Butterworth coefficients

% ============================================================================
% SECTION 5 — FIGURE AND UI CONSTRUCTION
% ============================================================================
%
% One figure, configured as a dark-theme oscilloscope panel.
% CloseRequestFcn ensures the timer is stopped before the window is destroyed
% — critical to prevent MATLAB "timer still running" orphan errors.

fig = figure('Name',          'PAM + TDM  4-Channel Simulator', ...
             'Color',          BG, ...
             'Position',       [40 40 1600 880], ...
             'Resize',         'off', ...
             'CloseRequestFcn', @onClose, ...
             'MenuBar',        'none', ...
             'ToolBar',        'none');

% ── Dial panels (top-left 4 × columns) ──────────────────────────────────────
%   Each panel holds an axes (for the drawn dial face) and a slider below it.

dial_ax = gobjects(1, num_ch);    % Axes handles for dial drawings
slid    = gobjects(1, num_ch);    % Slider handles for angle control

for k = 1:num_ch
    px = 0.01 + (k-1)*0.12;       % Horizontal position (normalised units)

    % Dark panel background for visual grouping
    uipanel('Parent', fig, 'Units', 'normalized', ...
            'Position', [px 0.72 0.11 0.26], ...
            'BackgroundColor', PAN, 'BorderType', 'none');

    % Axes inside panel for dial drawing (initDials fills this later)
    dial_ax(k) = axes('Parent', fig, 'Units', 'normalized', ...
                      'Position', [px+0.005 0.79 0.10 0.18], ...
                      'Color', PAN, 'XTick', [], 'YTick', [], ...
                      'XLim', [-1.2 1.2], 'YLim', [-1.2 1.2]);

    % Channel label below the dial
    text(dial_ax(k), 0, -1.1, sprintf('Ch-%d  %g Hz', k, f_msg(k)), ...
         'Color', C(k,:), 'HorizontalAlignment', 'center', 'FontSize', 8);

    % Amplitude slider: maps 0°–180° (0.0–1.0 amplitude)
    slid(k) = uicontrol('Style', 'slider', ...
                        'Units', 'normalized', ...
                        'Position', [px+0.01 0.73 0.09 0.03], ...
                        'Min', 0, 'Max', 180, 'Value', ang(k), ...
                        'BackgroundColor', PAN, ...
                        'Callback', @(s,~) cbAng(s, k));
end

% ── Q pulse axes (upper-right panel, 4 small scopes) ────────────────────────
%   Each shows the digital pulse train for one TDM slot.

q_ax = gobjects(1, num_ch);
q_h  = gobjects(1, num_ch);
tms  = (0:N-1)' / Fs * 1e3;       % Time axis in milliseconds (for all axes)

for k = 1:num_ch
    px = 0.50 + (k-1)*0.115;
    q_ax(k) = axes('Parent', fig, 'Units', 'normalized', ...
                   'Position', [px 0.80 0.105 0.15], ...
                   'Color', PAN, 'XColor', DIM, 'YColor', DIM, ...
                   'XLim', [0 win_dur*1e3], 'YLim', [-0.1 1.3]);
    title(q_ax(k), sprintf('Q%d', k), 'Color', C(k,:), 'FontSize', 8);
    q_h(k) = plot(q_ax(k), tms, zeros(N,1), 'Color', C(k,:), 'LineWidth', 1.2);
end

% ── Pulse-width slider ───────────────────────────────────────────────────────
uicontrol('Style', 'text', 'Units', 'normalized', ...
          'Position', [0.50 0.76 0.10 0.02], ...
          'String', 'Pulse Width (µs)', ...
          'BackgroundColor', BG, 'ForegroundColor', TXT, 'FontSize', 8);
pw_sl = uicontrol('Style', 'slider', 'Units', 'normalized', ...
                  'Position', [0.50 0.74 0.22 0.02], ...
                  'Min', 5, 'Max', slot_us-1, 'Value', pw_us, ...
                  'BackgroundColor', PAN, 'Callback', @cbPW);

% ── LPF slider ───────────────────────────────────────────────────────────────
uicontrol('Style', 'text', 'Units', 'normalized', ...
          'Position', [0.73 0.76 0.10 0.02], ...
          'String', 'LPF Cutoff (Hz)', ...
          'BackgroundColor', BG, 'ForegroundColor', TXT, 'FontSize', 8);
lpf_sl = uicontrol('Style', 'slider', 'Units', 'normalized', ...
                   'Position', [0.73 0.74 0.22 0.02], ...
                   'Min', 300, 'Max', 4000, 'Value', lpf_hz, ...
                   'BackgroundColor', PAN, 'Callback', @cbLPF);

% ── START / STOP / RESET buttons ─────────────────────────────────────────────
btn_cfg = {'Units','normalized','FontSize',10,'FontWeight','bold'};
uicontrol(btn_cfg{:}, 'Style','pushbutton', 'String','▶ START', ...
          'Position',[0.96 0.92 0.038 0.04], ...
          'BackgroundColor',[0.10 0.72 0.34], 'ForegroundColor','w', ...
          'Callback', @onStart);
uicontrol(btn_cfg{:}, 'Style','pushbutton', 'String','■ STOP', ...
          'Position',[0.96 0.87 0.038 0.04], ...
          'BackgroundColor',[0.80 0.30 0.20], 'ForegroundColor','w', ...
          'Callback', @onStop);
uicontrol(btn_cfg{:}, 'Style','pushbutton', 'String','↺ RESET', ...
          'Position',[0.96 0.82 0.038 0.04], ...
          'BackgroundColor',[0.30 0.35 0.50], 'ForegroundColor','w', ...
          'Callback', @onReset);

% ============================================================================
% SECTION 6 — TDM COMBINED SIGNAL AXIS  (wide centre strip)
% ============================================================================
%
% Shows all four channels multiplexed onto a single line.
% Pulses are colour-coded by channel.

tdm_ax = axes('Parent', fig, 'Units', 'normalized', ...
              'Position', [0.01 0.60 0.97 0.13], ...
              'Color', PAN, 'XColor', DIM, 'YColor', TXT, ...
              'XLim', [0 win_dur*1e3], 'YLim', [-1.1 1.1]);
title(tdm_ax, 'Combined PAM / TDM Signal  (125 µs clock)', ...
      'Color', TXT, 'FontSize', 9);
xlabel(tdm_ax, 'Time (ms)', 'Color', DIM, 'FontSize', 8);

% One coloured line per channel (stacked, non-overlapping)
tdm_h = gobjects(1, num_ch);
for k = 1:num_ch
    tdm_h(k) = line(tdm_ax, tms, zeros(N,1), 'Color', C(k,:), 'LineWidth', 1.4);
end

% ============================================================================
% SECTION 7 — INDIVIDUAL CHANNEL AXES  (4 side-by-side scopes)
% ============================================================================
%
% Each panel shows: original message (dashed white) + PAM pulses (coloured).

ch_ax  = gobjects(1, num_ch);
pam_h  = gobjects(1, num_ch);    % PAM pulse plot handles
msg_h  = gobjects(1, num_ch);    % Message sine plot handles

for k = 1:num_ch
    ch_ax(k) = axes('Parent', fig, 'Units', 'normalized', ...
                    'Position', [0.01+(k-1)*0.245  0.32  0.235  0.26], ...
                    'Color', PAN, 'XColor', DIM, 'YColor', DIM, ...
                    'XLim', [0 win_dur*1e3], 'YLim', [-1.2 1.2]);
    title(ch_ax(k), sprintf('Ch-%d  %g Hz  — Msg + PAM', k, f_msg(k)), ...
          'Color', C(k,:), 'FontSize', 8);
    xlabel(ch_ax(k), 'Time (ms)', 'Color', DIM, 'FontSize', 7);

    % Message sine (dashed white reference line)
    msg_h(k) = line(ch_ax(k), tms, zeros(N,1), ...
                    'Color', [1 1 1 0.45], 'LineStyle', '--', 'LineWidth', 1.0);

    % PAM pulses (solid, channel colour)
    pam_h(k) = line(ch_ax(k), tms, zeros(N,1), ...
                    'Color', C(k,:), 'LineWidth', 1.4);
end

% ============================================================================
% SECTION 8 — RECOVERY AXIS  (bottom strip, all 4 recovered signals)
% ============================================================================
%
% Shows the LPF output (solid) overlaid on the original message (dashed).
% Visual confirmation that the TDM → demux → LPF chain reconstructs the signal.

rec_ax = axes('Parent', fig, 'Units', 'normalized', ...
              'Position', [0.01 0.04 0.97 0.26], ...
              'Color', PAN, 'XColor', DIM, 'YColor', TXT, ...
              'XLim', [0 win_dur*1e3], 'YLim', [-1.2 1.2]);
title(rec_ax, 'Demux + LPF Recovery  (dashed = original, solid = recovered)', ...
      'Color', TXT, 'FontSize', 9);
xlabel(rec_ax, 'Time (ms)', 'Color', DIM, 'FontSize', 8);

rec_h   = gobjects(1, num_ch);   % Recovered (solid) lines
rref_h  = gobjects(1, num_ch);   % Original reference (dashed) lines

for k = 1:num_ch
    rref_h(k) = line(rec_ax, tms, zeros(N,1), ...
                     'Color', [C(k,:) 0.35], 'LineStyle', '--', 'LineWidth', 0.9);
    rec_h(k)  = line(rec_ax, tms, zeros(N,1), ...
                     'Color', C(k,:), 'LineWidth', 1.6);
end

% Add legend
legend(rec_ax, arrayfun(@(k) sprintf('Ch-%d', k), 1:num_ch, 'UniformOutput', false), ...
       'TextColor', TXT, 'Color', PAN, 'EdgeColor', DIM, ...
       'Location', 'northeast', 'FontSize', 8);

% ============================================================================
% SECTION 9 — DIAL INITIALISATION
% ============================================================================
%
% Draws static dial faces (arc + tick marks) once at startup.
% The needle is the only element updated each frame (inside fast()).

initDials();
tmr = [];                         % Timer handle (created in onStart)

% ============================================================================
% NESTED FUNCTION: fast()
% ============================================================================
% Core render function. Called every 40 ms by the timer.
% Computes ALL signals using vectorised math (no sample-level loops).
% Swaps YData on pre-existing handles — never recreates axes or lines.
%
% PERFORMANCE:
%   ~5–10 ms per call on modern hardware (well within 40 ms budget).
%   All computation is vectorised (matrix ops on N×4 arrays).

    function fast()
        % ── 1. Scrolling time axis ─────────────────────────────────────────
        %   phase advances 0.04 s/tick → signals scroll left continuously.
        %   Adding phase shifts the sine argument without reindexing arrays.
        t = (0:N-1)' / Fs + phase;       % Column vector, length N

        % ── 2. Message sine waves ─────────────────────────────────────────
        %   msgs is N×4. Each column: A_k · sin(2π·f_k·t)
        %   Amplitude = ang(k)/180  (dial angle maps 0°–180° → 0.0–1.0).
        msgs = sin(2*pi*f_msg.*t) .* (ang/180);   % Broadcasting: f_msg is 1×4

        % ── 3. Pulse trains Q1–Q4 ─────────────────────────────────────────
        %   qs is N×4 logical. qs(:,k) = 1 only during slot k of each frame.
        %   mod(t, T_clk) gives position within the current clock period (0–125 µs).
        %   slot_start offsets each channel by one slot width.
        %   pw_us controls the active pulse width within the slot.
        qs = false(N, num_ch);
        for k = 1:num_ch
            slot_start = (k-1) * slot_us * 1e-6;           % e.g. Ch3 → 62.5 µs offset
            phase_in_frame = mod(t, T_clk);
            qs(:,k) = (phase_in_frame >= slot_start) & ...
                      (phase_in_frame <  slot_start + pw_us*1e-6);
        end

        % ── 4. PAM generation ─────────────────────────────────────────────
        %   Element-wise multiply: pulse height = message amplitude at sample time.
        %   pam is N×4. Outside the pulse window, PAM = 0.
        pam = msgs .* qs;

        % ── 5. TDM multiplexing ───────────────────────────────────────────
        %   Sum all channels onto one bus. Safe because slots don't overlap.
        %   tdm is N×1.
        tdm = sum(pam, 2);

        % ── 6. Demultiplexing + LPF recovery ─────────────────────────────
        %   demux_k = tdm × Q_k  (gates out only channel k from the TDM bus)
        %   filter() applies the pre-designed Butterworth IIR coefficients.
        %   recovered is N×4.
        recovered = zeros(N, num_ch);
        for k = 1:num_ch
            demux         = tdm .* qs(:,k);
            recovered(:,k) = filter(lpf_b, lpf_a, demux);
        end

        % ── 7. Update Q pulse plots ───────────────────────────────────────
        for k = 1:num_ch
            set(q_h(k), 'YData', double(qs(:,k)));
        end

        % ── 8. Update TDM combined plot (one line per channel colour) ─────
        %   Each channel's contribution is isolated (pam(:,k)) so the
        %   correct colour is applied per pulse.
        for k = 1:num_ch
            set(tdm_h(k), 'YData', pam(:,k));
        end

        % ── 9. Update individual channel axes ─────────────────────────────
        for k = 1:num_ch
            set(msg_h(k), 'YData', msgs(:,k));
            set(pam_h(k), 'YData', pam(:,k));
        end

        % ── 10. Update recovery axis ──────────────────────────────────────
        for k = 1:num_ch
            set(rref_h(k), 'YData', msgs(:,k));
            set(rec_h(k),  'YData', recovered(:,k));
        end

        % ── 11. Redraw dial needles ───────────────────────────────────────
        %   Only the active channel's dial needle is redrawn each frame
        %   (others only move when they become active — see tCB).
        for k = 1:num_ch
            theta = ang(k) * pi / 180;                     % degrees → radians
            nx = cos(pi - theta);  ny = sin(pi - theta);   % needle tip coordinates
            % Delete previous needle and redraw
            delete(findobj(dial_ax(k), 'Tag', sprintf('needle%d', k)));
            line(dial_ax(k), [0 nx], [0 ny], ...
                 'Color', C(k,:), 'LineWidth', 2.5, ...
                 'Tag', sprintf('needle%d', k));
        end

        drawnow limitrate;        % Flush pending graphics events (respects frame budget)
    end

% ============================================================================
% NESTED FUNCTION: tCB()  — Timer callback (fires every 40 ms)
% ============================================================================
%
% 1. Advances the scrolling phase (creates waveform movement).
% 2. Determines which TDM slot is currently active (cycles every 4 s).
% 3. Applies IIR smoothing to the active channel's dial angle.
% 4. Calls fast() to recompute and render all signals.
%
% IIR SMOOTHER:
%   ang(cur) = 0.75 × ang(cur) + 0.25 × tgt(cur)
%   alpha = 0.25 → converges to within 1% of target in ~17 ticks (0.68 s).
%   Mimics the physical inertia of a real rotary servo.
%
% SLOT SEQUENCING:
%   cur = mod(floor(phase / 4), num_ch) + 1
%   At phase = 0 s:  cur = 1
%   At phase = 4 s:  cur = 2
%   At phase = 8 s:  cur = 3
%   At phase = 12 s: cur = 4
%   At phase = 16 s: cur = 1  (repeat)

    function tCB(~,~)
        phase = phase + 0.04;                              % Advance scroll 40 ms
        cur   = mod(floor(phase/4), num_ch) + 1;          % Active slot (1–4)
        ang(cur) = ang(cur)*0.75 + tgt(cur)*0.25;         % IIR smooth toward target
        fast();                                            % Render frame
    end

% ============================================================================
% NESTED FUNCTION: cbAng()  — Dial slider callback
% ============================================================================
%
% Called when any of the four amplitude sliders is dragged.
% Updates tgt(k) immediately; ang(k) converges in subsequent tCB() calls
% only when that channel's 4-second slot becomes active.
%
% This models TDM queuing: the new value is "scheduled" but not applied
% until the system reaches that channel's time slot.

    function cbAng(src, k)
        tgt(k) = src.Value;
    end

% ============================================================================
% NESTED FUNCTION: cbPW()  — Pulse width slider callback
% ============================================================================
%
% Updates pw_us (microseconds). Effective immediately on the next fast() call.
% Value is clamped to [5, slot_us−1] by slider Min/Max bounds.
% Effect visible: wider pulses → taller recovered amplitude (more energy);
%                 too wide → channel overlap → crosstalk.

    function cbPW(src,~)
        pw_us = src.Value;
    end

% ============================================================================
% NESTED FUNCTION: cbLPF()  — LPF cutoff slider callback
% ============================================================================
%
% Updates lpf_hz and redesigns the Butterworth filter coefficients.
% mkLPF() is called synchronously — fast enough (<1 ms) for slider drag.
%
% Observable effect:
%   Cutoff too low  → signal disappears (message frequency filtered out)
%   Cutoff just right → clean recovered sine (harmonics rejected)
%   Cutoff too high  → pulse harmonics bleed through (noisy recovery)

    function cbLPF(src,~)
        lpf_hz = src.Value;
        mkLPF();
    end

% ============================================================================
% NESTED FUNCTION: onStart() — Starts the animation timer
% ============================================================================
%
% Creates and starts a fixed-rate timer at 25 fps (40 ms period).
% BusyMode='drop': if a frame takes >40 ms (e.g. during heavy slider drag),
% the next tick is skipped rather than queued. This prevents frame pile-up
% and keeps the GUI responsive at all times.

    function onStart(~,~)
        if ~isempty(tmr) && isvalid(tmr), return; end     % Ignore if already running
        tmr = timer('ExecutionMode', 'fixedRate', ...
                    'Period',     0.04, ...
                    'TimerFcn',   @tCB, ...
                    'BusyMode',   'drop');
        start(tmr);
    end

% ============================================================================
% NESTED FUNCTION: onStop() — Pauses the animation
% ============================================================================
%
% Stops and deletes the timer. Waveforms freeze at last frame.
% The figure remains open and interactive.

    function onStop(~,~)
        if ~isempty(tmr) && isvalid(tmr)
            stop(tmr); delete(tmr); tmr = [];
        end
    end

% ============================================================================
% NESTED FUNCTION: onReset() — Restores all defaults
% ============================================================================
%
% Restores ang, tgt, pw_us, lpf_hz, and phase to their initial values.
% Updates slider positions to match. Redesigns LPF for the reset cutoff.
% Renders one frame immediately so the display updates without waiting for
% the next timer tick.

    function onReset(~,~)
        ang    = [90 45 135 60];
        tgt    = [90 45 135 60];
        pw_us  = 18;
        lpf_hz = 2500;
        phase  = 0;
        cur    = 1;
        set(pw_sl,  'Value', pw_us);
        set(lpf_sl, 'Value', lpf_hz);
        for k = 1:num_ch
            set(slid(k), 'Value', ang(k));
        end
        mkLPF();
        fast();
    end

% ============================================================================
% NESTED FUNCTION: onClose() — Safe figure close
% ============================================================================
%
% MATLAB does not automatically stop timers when a figure is closed.
% Without this callback, the timer keeps running after the window is gone,
% causing "Invalid handle" errors every 40 ms until MATLAB is restarted.
%
% Correct shutdown sequence:
%   1. Stop and delete the timer
%   2. Then delete the figure

    function onClose(~,~)
        onStop();
        delete(fig);
    end

% ============================================================================
% NESTED FUNCTION: mkLPF() — Designs Butterworth filter coefficients
% ============================================================================
%
% Called at startup and whenever the LPF slider moves.
%
% DESIGN PARAMETERS:
%   Order: 6 — steep roll-off (~120 dB/decade) for clean harmonic rejection.
%   Type:  Low-pass Butterworth (maximally flat passband).
%   Wn:    Normalised cutoff = lpf_hz / (Fs/2)
%          (MATLAB butter() expects normalised frequency 0–1, where 1 = Nyquist)
%
% EFFECT ON RECOVERY:
%   The 8 kHz TDM clock and its harmonics are well above message frequencies.
%   A 6th-order LPF at 2.5 kHz provides >80 dB attenuation at 8 kHz,
%   cleanly reconstructing the original sine waveform from the pulse train.

    function mkLPF()
        Wn = lpf_hz / (Fs/2);                             % Normalise to Nyquist
        [lpf_b, lpf_a] = butter(6, Wn, 'low');            % 6th-order Butterworth
    end

% ============================================================================
% NESTED FUNCTION: initDials() — Draws static dial face artwork
% ============================================================================
%
% Called once at startup. Renders the arc, tick marks, and labels for all
% four dials. The needle is NOT drawn here — it is added each frame by fast().
%
% DIAL GEOMETRY:
%   Semicircle from 0° to 180° (left = 0, top = 90°, right = 180°)
%   Converted to Cartesian: x = cos(π − θ), y = sin(π − θ)
%   Tick marks at 0°, 30°, 60°, 90°, 120°, 150°, 180° (every 30°).

    function initDials()
        theta_arc = linspace(0, pi, 120);                 % Arc from 0 to π radians
        xarc = cos(theta_arc);
        yarc = sin(theta_arc);

        for k = 1:num_ch
            axes(dial_ax(k));                             %#ok — set current axes
            hold on;

            % Draw semicircular arc (dim grey ring)
            plot(xarc, yarc, 'Color', DIM, 'LineWidth', 1.0);

            % Draw tick marks at 30° intervals
            for deg = 0:30:180
                th  = pi - deg*pi/180;                    % Convert to radian, flip
                r0  = 0.82;  r1 = 1.0;                   % Inner and outer radius
                plot([r0*cos(th) r1*cos(th)], ...
                     [r0*sin(th) r1*sin(th)], ...
                     'Color', DIM, 'LineWidth', 1.0);

                % Amplitude label at major ticks (0, 90, 180°)
                if mod(deg, 90) == 0
                    lbl = sprintf('%.1f', deg/180);
                    text(1.15*cos(th), 1.15*sin(th), lbl, ...
                         'Color', DIM, 'FontSize', 6, ...
                         'HorizontalAlignment', 'center');
                end
            end

            % Centre pivot dot
            plot(0, 0, '.', 'Color', TXT, 'MarkerSize', 8);

            % Initial needle at current angle
            theta = ang(k) * pi / 180;
            nx = cos(pi - theta);  ny = sin(pi - theta);
            line([0 nx], [0 ny], 'Color', C(k,:), 'LineWidth', 2.5, ...
                 'Tag', sprintf('needle%d', k));

            axis equal off;
        end
    end

end  % PAM_TDM_4Servo_4sec
```

---

## 🔧 Technical Implementation Details

### Fixed System Parameters

```matlab
Fs      = 200e3;               % 200 kHz sampling rate — resolves 5 µs pulses cleanly
f_clk   = 8000;                % 8 kHz TDM clock (matches lab hardware)
num_ch  = 4;                   % 4 TDM channels
f_msg   = [2000 1000 1500 800];% Message frequencies per channel (Hz)
T_clk   = 1/f_clk;             % Frame period = 125 µs
slot_us = T_clk*1e6/num_ch;    % Slot width = 31.25 µs per channel
win_dur = 0.002;                % Display window = 2 ms (16 clock cycles visible)
N       = round(Fs*win_dur);    % 400 samples per display window
```

### Mutable State (Changed at Runtime)

```matlab
ang    = [90 45 135 60];       % Dial angles (0°–180°); amplitude = ang/180
tgt    = [90 45 135 60];       % Slider targets; ang converges to tgt via IIR
pw_us  = 18;                   % Pulse width in µs (5–28 µs range)
lpf_hz = 2500;                 % LPF cutoff in Hz (300–4000 Hz range)
phase  = 0;                    % Scrolling phase; +0.04 s per tick
cur    = 1;                    % Active TDM slot (1–4, cycles every 4 s)
```

### Colour Scheme

```matlab
C   = [0.18 0.68 1.00;   % Ch-1  sky-blue
       0.10 0.90 0.50;   % Ch-2  green
       1.00 0.64 0.10;   % Ch-3  amber
       0.92 0.24 0.44];  % Ch-4  pink-red

BG  = [0.08 0.10 0.16];  % Dark navy background
PAN = [0.13 0.15 0.23];  % Panel / axes fill
TXT = [0.92 0.94 0.98];  % Near-white text
DIM = [0.38 0.40 0.46];  % Dimmed grey for minor labels
```

---

## ⏱️ Zero-Flicker Animation Design

The key to smooth 25 fps rendering in MATLAB is that **all plot handles are created once at startup** and only `YData` is swapped each frame — no axes, lines, or legends are ever recreated mid-animation.

```matlab
% ── One-time creation at startup (slow path) ──
pam_h(k) = line(ch_ax(k), tms, zeros(N,1), 'Color', C(k,:), 'LineWidth', 1.4);

% ── Every 40 ms frame (fast path — YData swap only) ──
set(pam_h(k), 'YData', new_pam_signal);
```

This applies to all 11 simultaneous plot axes: 4 Q pulse plots, 1 TDM plot, 4 channel PAM plots, and 1 recovery plot.

### Timer Configuration

```matlab
tmr = timer('ExecutionMode', 'fixedRate', ...
            'Period',     0.04, ...       % 40 ms = 25 fps
            'TimerFcn',   @tCB, ...
            'BusyMode',   'drop');        % Skip frame if previous not done
```

`BusyMode = 'drop'` prevents frame queuing — if a tick takes longer than 40 ms, the next is silently skipped rather than piling up. This keeps the animation smooth and prevents the GUI from freezing during slider interaction.

---

## 📊 TDM Slot Sequencing & IIR Dial Smoother

```matlab
function tCB(~,~)
    phase    = phase + 0.04;                        % Advance scroll 40 ms/tick
    cur      = mod(floor(phase/4), num_ch) + 1;    % Active slot cycles every 4 s
    ang(cur) = ang(cur)*0.75 + tgt(cur)*0.25;      % IIR: 25% step toward target
    fast();
end
```

**IIR Convergence:**
```
alpha = 0.25 → each tick covers 25% of remaining distance to target
              Converges to within 1% in ~17 ticks ≈ 0.68 seconds
```

When a channel's slot is not active, dragging the slider still updates `tgt` immediately — but `ang` only moves when that channel's 4-second window arrives. This faithfully models how a real TDM system queues a new value and applies it at the next available time slot.

---

## 🔬 Butterworth LPF Design

```matlab
function mkLPF()
    Wn = lpf_hz / (Fs/2);                  % Normalise to Nyquist (0–1)
    [lpf_b, lpf_a] = butter(6, Wn, 'low'); % 6th-order Butterworth LPF
end
```

Called once at startup, then again on every LPF slider movement. The 6th-order design provides a steep ~120 dB/decade roll-off that cleanly separates message frequencies from clock harmonics.

### LPF Cutoff Effect

| Cutoff Setting | Observed Effect |
|---|---|
| Below message frequency | Signal disappears — over-filtered |
| Just above message frequency | Clean sine recovery — ideal |
| Too high (near 4 kHz) | Pulse harmonics bleed through — noisy recovery |

---

## 📐 Lab Hardware Mapping — Experiment 6

| Lab Hardware | MATLAB Equivalent | Notes |
|---|---|---|
| Master Signals 8 kHz clock | `f_clk = 8000` | Fixed parameter |
| Twin Pulse Generator Q1–Q4 | `qs(:, 1..4)` | 4 non-overlapping pulse trains |
| Width knob | `pw_us` slider (5–28 µs) | Max = slot_us−1 to prevent overlap |
| Dual Analog Switch | `pam = msgs .* qs` | Multiplication = gating logic |
| TDM bus output | `tdm = sum(pam, 2)` | Non-overlapping — safe to sum |
| Multiplier (demux) | `demux = tdm .* qs(:,k)` | Extracts one channel from TDM |
| Tuneable LPF | `butter(6, lpf_hz/(Fs/2))` | Redesigned live on slider move |
| Message source dial | Sine amplitude = `ang/180` | Visual teaching metaphor |

---

## 📈 Lab Questions Answered by the Simulator

| Question | What to Observe |
|---|---|
| Q3: Effect of pulse width on TDM capacity | Narrow `pw_us` → more gap between pulses → room for more channels |
| Q7: Pulses per cycle per channel | 8 kHz ÷ 2 kHz = **4 pulses/cycle** for Ch-1 (visible in channel plot) |
| Q8: Total TDM pulses per cycle | 4 channels × 4 = **16 pulses/cycle** in the TDM plot |
| Q10: Effect of pulse width on recovery amplitude | Narrower pulses → smaller LPF output → reduced recovered amplitude |

---

## 🛠️ Interactive Controls Reference

| Control | Range | Purpose |
|---|---|---|
| Dial sliders × 4 | 0°–180° | Set message sine amplitude per channel |
| Pulse Width slider | 5–28 µs | Controls PAM pulse width (lab Width knob) |
| LPF Cutoff slider | 300–4000 Hz | Sets Butterworth cutoff (lab Tuneable LPF) |
| ▶ START | — | Creates and starts 25 fps timer |
| ■ STOP | — | Stops timer; waveforms freeze |
| ↺ RESET | — | Restores all values to defaults |

---

## 📊 Performance Metrics

| Metric | Value |
|---|---|
| Frame rate | 25 fps (40 ms timer period) |
| Display sampling rate | 200 kHz |
| Display window | 2 ms (16 clock cycles) |
| Samples per frame | 400 |
| TDM clock | 8 kHz |
| TDM frame period | 125 µs |
| Slot width (4 channels) | 31.25 µs |
| LPF order | 6th-order Butterworth |
| Dial IIR alpha | 0.25 (~17 ticks to converge) |
| Active slot duration | 4 seconds per channel |
| CPU time per frame | ~5–10 ms (well within 40 ms budget) |

---

## 📚 Function Reference

| Function | Role | Called From |
|---|---|---|
| `fast()` | Computes and renders all signals each frame | `tCB()`, `onReset()` |
| `tCB()` | Timer callback: advances phase, IIR smooth, calls `fast()` | Timer (every 40 ms) |
| `cbAng(src, k)` | Updates `tgt(k)` on dial slider change | Slider `Callback` |
| `cbPW(src)` | Updates `pw_us` on pulse width slider change | Slider `Callback` |
| `cbLPF(src)` | Updates `lpf_hz`, calls `mkLPF()` | Slider `Callback` |
| `onStart()` | Creates and starts timer | START button |
| `onStop()` | Stops and deletes timer | STOP button / `onClose()` |
| `onReset()` | Restores all defaults, rerenders | RESET button |
| `onClose()` | Calls `onStop()` then deletes figure | `CloseRequestFcn` |
| `mkLPF()` | Designs Butterworth coefficients | Startup, `cbLPF()` |
| `initDials()` | Draws static dial arc + ticks | Startup |

---

## 🚀 How to Run

```matlab
% In the MATLAB command window:
PAM_TDM_4Servo_4sec()

% 1. Press START  → animation begins at 25 fps
% 2. Drag dial sliders  → change message amplitude per channel
% 3. Drag Pulse Width slider  → observe pulse spacing and recovery amplitude change
% 4. Drag LPF Cutoff slider  → observe how well the original signal is recovered
% 5. Press STOP  → freeze waveforms for inspection
% 6. Press RESET → restore all defaults
% 7. Close the window → timer stops safely (CloseRequestFcn handles this)
```

**Requirements:**
```
MATLAB R2021a or newer
Signal Processing Toolbox  — butter(), filter()
No additional toolboxes required
```

---

## 📄 Conclusion

This MATLAB simulator delivers a complete, interactive recreation of EECE-310 Experiment 6:

✅ **Zero-flicker 25 fps rendering** — handle reuse and `BusyMode='drop'` timer discipline
✅ **Direct lab correlation** — every hardware module maps to a named parameter or function
✅ **Live parameter exploration** — sliders let students observe each variable's effect in real time
✅ **Correct TDM slot sequencing** — channels activate in rotation, faithfully modelling time-slot allocation
✅ **Vectorised computation** — all signal math uses matrix operations; no sample-level loops

---

*Project Author: Md Farhan Mohib Hemal | EECE-310 Communications Lab | Version 1.0*
