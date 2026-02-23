# MPPT 4820 HC — 1 kW Hardware Upgrade Specification

## 1. Overview

This document describes the hardware modifications required to upgrade the MPPT 4820 HC from its
original ~500 W design (20 A inductor, 80 V PV input) to a 1 kW design supporting dual battery
voltage (24 V / 48 V) and a 140 V PV input.

### 1.1 Design Goals

| Goal | Original | Upgraded |
|------|----------|----------|
| Output power | ~500 W | 1 000 W |
| Battery voltage | 12 V / 24 V | 24 V / 48 V |
| Max PV input voltage | 80 V | 140 V |
| Max inductor current | 20 A | 50 A (peak) |
| Max battery current (24 V) | 20 A | 42 A |
| Max battery current (48 V) | — | 21 A |
| Switching frequency | ~100 kHz | 80 kHz |
| Target efficiency | >93 % | >95 % |
| Ambient temperature | −20 °C to +40 °C | −20 °C to +50 °C |
| Design life | — | 10+ years |

---

## 2. Component Upgrade Specifications

### 2.1 High-Side Power MOSFET — Q1

The high-side switch must block the full PV open-circuit voltage with adequate margin.

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Part number | (100 V NMOS) | **Infineon IPW60R041P6** |
| Package | TO-220 | TO-247 |
| V_DS(max) | 100 V | 650 V |
| R_DS(on) @ 25 °C | — | 41 mΩ |
| I_D (continuous) | — | 38 A |
| Q_g (total) | — | 110 nC |
| t_rr | — | 65 ns |

**Alternatives:**

- STMicroelectronics STW48N60DM2 (600 V, 38 mΩ, TO-247)
- ON Semiconductor FCPF22N60 (600 V, 260 mΩ — only for low-power variant)

**Rationale:** 140 V PV OC × 1.5 safety margin = 210 V minimum; 650 V provides generous margin
for load-dump transients and lightning coupling on PV cable.

---

### 2.2 Low-Side Power MOSFET — Q2

The low-side switch carries the full inductor current and must have ultra-low R_DS(on) to
minimise conduction losses at 42 A.

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Part number | (100 V NMOS) | **Infineon IPT015N10N5** |
| Package | TO-220 | TO-220 |
| V_DS(max) | 100 V | 100 V |
| R_DS(on) @ 25 °C | — | 1.5 mΩ |
| I_D (continuous) | — | 100 A |
| Q_g (total) | — | 91 nC |

**Alternatives:**

- Vishay SiR622DP (100 V, 1.7 mΩ, PowerPAK SO-8)
- Infineon BSC010N04LS (40 V, 1.0 mΩ — only if max battery voltage ≤ 40 V)

**Rationale:** At 42 A the conduction loss is P = I² × R_DS(on) = 42² × 0.0015 = 2.65 W,
acceptable for TO-220 without forced air.

---

### 2.3 Power Inductor — L1

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Part number | — | **Würth Elektronik 7443730470** |
| Inductance | — | 4.7 µH |
| Saturation current | 20 A | 50 A |
| DCR (typical) | — | 2.4 mΩ |
| Core loss @ 80 kHz | — | see §3 |
| Dimensions | — | 22.5 × 22.5 × 19.5 mm |
| Temperature rise (rated) | — | ΔT = 40 °C @ I_sat |

**Alternatives:**

- Coilcraft XAL8080-472 (4.7 µH, 45 A saturation, lower DCR but smaller footprint)
- Bourns SRR1260-4R7Y (4.7 µH, 34 A — marginal; derate to 40 A max if used)

**Selection Criteria:**

```
Ripple current (ΔI_L):
  ΔI_L = (V_in − V_out) × D / (L × f_sw)
       = (140 V − 48 V) × 0.343 / (4.7 µH × 80 kHz)
       = 92 V × 0.343 / 0.376 A
       = 83.9 V / 376 Ω
       ≈ 10.6 A_pp   (25 % of I_peak = 42.5 A) ✓

Peak inductor current:
  I_peak = I_out + ΔI_L / 2 = 42 + 5.3 = 47.3 A < 50 A saturation ✓
```

---

### 2.4 Current Sense Resistor — R_shunt

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Part number | — | **Bourns CRE2512-FZ-R001E-3** |
| Resistance | — | 1 mΩ |
| Power rating | — | 3 W |
| Tolerance | — | ±1 % |
| Temperature coefficient | — | ±75 ppm/°C |
| Package | — | 2512 SMD |

**Alternatives:**

- Vishay WSBS2515 (1 mΩ, 5 W — preferred if PCB copper pour is limited)
- KOA Speer SMB2A-R001 (1 mΩ, 3 W — direct footprint alternative)

**Voltage at full load:**

```
V_shunt = I × R = 42 A × 0.001 Ω = 42 mV
Power  = I² × R = 42² × 0.001    = 1.76 W < 3 W rating ✓
```

---

### 2.5 Input Capacitors — C_in

The input capacitors must withstand the PV open-circuit voltage with appropriate margin.

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Voltage rating | 100 V | **160 V** |
| Capacitance | — | 4 × 100 µF |
| ESR (each) | — | < 50 mΩ |
| Type | Electrolytic | Electrolytic + X7R ceramic |
| Example part | — | Nichicon UHE2C101MED (100 µF, 160 V, Ø16 × 25 mm) |
| Ceramic bypass | — | 2 × 100 nF, 250 V X7R (e.g., Kemet C1210C104KARACTU) |

**Rating check:**

```
V_PV_OC(max) = 140 V
Rated voltage derating @ 70 % = 160 V × 0.70 = 112 V
Operating margin = 112 V / 140 V = 80 % → pass with 160 V rating ✓

For 85 °C-rated caps at 50 °C ambient, lifetime multiplier ≈ 8× per 10 °C drop
→ Long-life (105 °C rated) caps recommended; see §5.
```

---

### 2.6 Output Capacitors — C_out

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Voltage rating | 35 V / 50 V | **63 V** |
| Capacitance | — | 4 × 680 µF |
| ESR (each) | — | < 30 mΩ |
| Type | Electrolytic | Electrolytic (low ESR) |
| Example part | — | Panasonic EEEFK1J681P (680 µF, 63 V, Ø18 × 20 mm) |

**Ripple current check:**

```
I_ripple_cap = ΔI_L / (2√3) = 10.6 / 3.46 ≈ 3.1 A_rms
Rated ripple (Panasonic EEEFK1J681P) = 3.97 A_rms @ 105 °C → ✓
```

---

### 2.7 Gate Driver — U_GD

The gate driver must support the bootstrap rail required for 140 V high-side drive and provide
sufficient peak current to switch 100+ nC gates at 80 kHz.

| Parameter | Original | Upgraded |
|-----------|----------|---------|
| Part number | — | **Texas Instruments UCC27712** |
| Supply voltage | — | 10–20 V |
| Bootstrap voltage | — | 150 V (VS pin) |
| Source / sink current | — | 4 A / 4 A |
| Propagation delay | — | 50 ns typ |
| Dead-time (adjustable) | — | via R_DT |
| Package | — | SOIC-8 |

**Alternatives:**

- Silicon Labs Si8271 (isolated, 150 V bootstrap, 4 A) — preferred for improved EMI isolation
- Microchip MCP14628 (100 V bootstrap only — not suitable for 140 V PV)

**Bootstrap capacitor:**

```
C_boot ≥ Q_g(Q1) / ΔV_boot = 110 nC / 0.3 V = 367 nF
Use 470 nF, 200 V X7R ceramic (e.g., Kemet C1206C474KARAC7800)
```

---

### 2.8 Voltage Sensing Dividers

Two resistive dividers are required: one for the PV (high) side and one for the battery (low) side.

#### 2.8.1 PV Voltage Divider (140 V range)

```
Target ADC range: 0–3.3 V for 0–165 V (125 % of 132 V = V_PV_OC @ STC)
Ratio: k = 3.3 / 165 = 0.02
R_top / R_bot = (1 − k) / k = 49

Use: R_top = 1 MΩ (2 × 499 kΩ in series for reliability)
     R_bot = 20.0 kΩ (1 % tolerance)
Ratio check: 1 000 / 20 = 50 → V_ADC = 3.3 × (20/1020) × 165 = 3.26 V ✓
Power (R_top @ 140 V) = 140² / 1 000 000 = 19.6 mW → use 0603 0.1 W ✓
```

| Component | Value | Package | Rating |
|-----------|-------|---------|--------|
| R_PV_top1 | 499 kΩ, 1 % | 0603 | 100 mW |
| R_PV_top2 | 499 kΩ, 1 % | 0603 | 100 mW |
| R_PV_bot  | 20.0 kΩ, 1 % | 0603 | 100 mW |
| C_PV_filt | 10 nF, 200 V | 0603 | X7R |

#### 2.8.2 Battery Voltage Divider (58 V range)

```
Target ADC range: 0–3.3 V for 0–60 V
Ratio: k = 3.3 / 60 = 0.055
R_top / R_bot = (1 − k) / k = 17.18 → use 16.9 (standard ratio)

Use: R_top = 169 kΩ (standard E96)
     R_bot = 10.0 kΩ
Ratio check: 169 / 10 = 16.9 → V_ADC @ 58 V = 58 × 10 / 179 = 3.24 V ✓
```

| Component | Value | Package | Rating |
|-----------|-------|---------|--------|
| R_BAT_top | 169 kΩ, 1 % | 0603 | 100 mW |
| R_BAT_bot | 10.0 kΩ, 1 % | 0603 | 100 mW |
| C_BAT_filt | 10 nF, 50 V | 0603 | X7R |

---

### 2.9 Current Sense Amplifier

The 42 mV full-scale shunt signal must be amplified to match the ADC input range.

```
Required gain: G = 3.3 V / 0.042 V ≈ 78.6 V/V
Use G = 78 V/V: R_gain = 10 kΩ, R_fb = 780 kΩ (nearest standard: 787 kΩ)
Actual gain: 787 / 10 = 78.7 V/V → V_ADC @ 42 A = 42 × 0.001 × 78.7 = 3.305 V ✓
```

| Parameter | Value |
|-----------|-------|
| Recommended IC | Texas Instruments INA240A3 (80 V common-mode, 20 V/V fixed — requires external stage) |
| Alternative | Analog Devices AD8418A (70 V CM, adjustable gain, SOT-23-8) |
| Gain (set externally) | 78 V/V |
| R_gain | 10 kΩ, 1 % (0402) |
| R_feedback | 787 kΩ, 1 % (0402) |
| Bandwidth | > 100 kHz |
| Common-mode range | 0–140 V |

---

### 2.10 Protection Components

#### 2.10.1 PV Input TVS Diode

| Parameter | Value |
|-----------|-------|
| Part number | Littelfuse SMBJ150A |
| Standoff voltage | 150 V |
| Clamping voltage | 243 V @ 10 A |
| Peak pulse power | 600 W |
| Package | DO-214AA (SMB) |

#### 2.10.2 Gate Drive Bootstrap TVS

| Parameter | Value |
|-----------|-------|
| Part number | Vishay MMSZ5248B |
| Zener voltage | 18 V |
| Purpose | Clamp V_GS of Q1 to 20 V max |
| Package | SOD-123 |

#### 2.10.3 Battery Output TVS

| Parameter | Value |
|-----------|-------|
| Part number | Littelfuse SMBJ58A |
| Standoff voltage | 58 V |
| Clamping voltage | 93.6 V @ 10 A |
| Package | DO-214AA (SMB) |

---

### 2.11 Power Supply Modifications

The auxiliary power supply (Sheet 3 in the schematic hierarchy) must be modified to:

1. **Accept 24 V–58 V battery input** (previously 10–32 V) — verify LMR16006 or replace with a
   part rated for 60 V input such as the **Texas Instruments LMR16030** (60 V, 3 A, SIMPLE
   SWITCHER).
2. **Maintain 5 V @ 500 mA** rail for the gate driver.
3. **Add 15 V rail** for UCC27712 V_DD (use LM7815 or TI TLV62568 buck regulated from 5 V).

| Rail | Original | Upgraded |
|------|----------|---------|
| V_IN range | 10–32 V | 24–58 V |
| 3.3 V (MCU) | ✓ keep | ✓ keep |
| 5 V (aux) | ✓ keep | ✓ keep |
| 15 V (gate driver) | — | **add** |
| Buck IC | LMR16006 (42 V) | **LMR16030 (60 V)** |

---

## 3. Thermal Management

### 3.1 Power Loss Calculations

All calculations at maximum load: P_out = 1 008 W (42 A × 24 V), f_sw = 80 kHz, T_amb = 50 °C.

#### Q1 — High-Side MOSFET

```
Duty cycle @ 48 V battery, 140 V PV:   D = V_out / V_in = 48 / 140 = 0.343
Conduction loss: P_cond = I²_rms × R_DS(on)
  I_rms(Q1) = I_out × √D = 21 A × √0.343 = 12.3 A
  P_cond    = 12.3² × 0.041 = 6.2 W

Switching loss (approximate):
  P_sw = 0.5 × V_DS × I_D × (t_r + t_f) × f_sw
       = 0.5 × 140 × 21 × (25 ns + 30 ns) × 80 000
       = 0.5 × 140 × 21 × 55e-9 × 80 000
       ≈ 6.5 W

Total Q1 loss: P_Q1 = 6.2 + 6.5 = 12.7 W

Junction temperature:
  θ_JC (IPW60R041P6) = 0.42 °C/W
  θ_CS  (TO-247, insulated pad) ≈ 0.5 °C/W
  θ_SA  (target) = (175 − 50) / 12.7 − 0.42 − 0.5 = 9.9 − 0.92 ≈ 9.0 °C/W
  Use heatsink: R_SA ≤ 9 °C/W

  T_J = T_amb + P × (θ_JC + θ_CS + θ_SA)
      = 50 + 12.7 × (0.42 + 0.5 + 5.0)       [example: R_SA = 5 °C/W]
      = 50 + 12.7 × 5.92
      = 50 + 75 ≈ 125 °C   (margin to 175 °C = 50 °C) ✓

Conservative worst-case (shared heatsink, R_SA = 7 °C/W):
  T_J = 50 + 12.7 × (0.42 + 0.5 + 7.0) = 50 + 101 = 151 °C → still < 175 °C ✓
  Margin = 175 − 151 = 24 °C ✓
```

#### Q2 — Low-Side MOSFET

```
Duty cycle complement: D' = 1 − D = 1 − 0.343 = 0.657
I_rms(Q2) = I_out × √D' = 42 A × √0.657 = 34.0 A
P_cond    = 34.0² × 0.0015 = 1.73 W

Switching loss (body diode reverse recovery):
  P_sw_Q2 = Q_rr × V_DS × f_sw = 45e-9 × 100 × 80 000 = 0.36 W

Total Q2 loss: P_Q2 = 1.73 + 0.36 = 2.09 W

θ_JC (IPT015N10N5) = 0.5 °C/W
T_J = 50 + 2.09 × (0.5 + 0.5 + 5.0) = 50 + 12.5 = 62.5 °C ✓
Margin to 175 °C = 112 °C ✓
```

#### L1 — Power Inductor

```
DCR loss:   P_DCR = I²_rms × DCR = 35 A² × 0.0024 = 2.94 W
            (I_rms ≈ 35 A for 50 % duty cycle, worst case)

Core loss (estimated from datasheet curves at 80 kHz, ΔB ~ 50 mT):
  P_core ≈ 5 W

Total inductor loss: P_L = 2.94 + 5 = ~8 W

Temperature rise:
  ΔT_L = P_L × R_th(winding-to-ambient)
  Würth 7443730470 rated ΔT = 40 °C @ P_rated
  At 8 W with natural convection: ΔT ≈ 40 °C
  T_L = 50 + 40 = 90 °C < 155 °C (core rated max) ✓
  Margin = 65 °C ✓
```

#### Shunt Resistor

```
P_shunt = I² × R = 42² × 0.001 = 1.76 W
Bourns CRE2512 rated 3 W → derating @ 50 °C: 3 × (155 − 50) / (155 − 25) = 2.42 W
1.76 W < 2.42 W ✓
```

### 3.2 Total Efficiency

```
Total losses:
  P_Q1      = 12.7 W
  P_Q2      =  2.1 W
  P_L       =  8.0 W
  P_shunt   =  1.8 W
  P_gate    =  1.5 W  (Q_g_total × V_GS × f_sw × 2)
  P_control =  2.0 W  (aux supply, MCU, etc.)
  ─────────────────
  P_loss    = 28.1 W  (48 V output mode)

η = P_out / (P_out + P_loss) = 1008 / (1008 + 28.1) = 97.3 %   (48 V mode) ✓

24 V mode (worst case, higher duty cycle):
  Additional switching losses on Q1 ≈ 9 W (higher D)
  P_loss ≈ 46 W
  η = 1008 / (1008 + 46) = 95.6 %                               (24 V mode) ✓
```

---

## 4. PCB Layout Requirements

### 4.1 Power Trace Widths

Use IPC-2221 (external trace, 30 °C temperature rise) as the baseline:

| Net | Current | Min Width (1 oz) | Min Width (2 oz) | Recommended |
|-----|---------|-----------------|-----------------|-------------|
| PV+ / PV− | 42 A | 22 mm | 11 mm | **Pour + 2 oz copper** |
| Inductor loop | 50 A (peak) | 26 mm | 13 mm | **Pour + 2 oz copper** |
| Battery+ / Battery− | 42 A | 22 mm | 11 mm | **Pour + 2 oz copper** |
| Gate drive | 0.5 A | 0.3 mm | 0.2 mm | 0.5 mm |
| Sense signals | < 10 mA | 0.2 mm | 0.2 mm | 0.3 mm |

**Recommendation:** Use 2 oz (70 µm) copper on the power layers. Add thermal relief vias to
transfer heat from surface pads to internal copper planes.

### 4.2 Voltage Clearances

IEC 60664-1 (overvoltage category II, pollution degree 2):

| Voltage | Min PCB Clearance | Min Creepage |
|---------|------------------|--------------|
| 140 V (PV) | 1.0 mm | 2.0 mm |
| 48 V (battery) | 0.5 mm | 1.0 mm |
| 15 V (gate supply) | 0.3 mm | 0.6 mm |

### 4.3 Thermal Via Arrays

Place thermal via arrays (0.3 mm drill, 0.6 mm pad) under:

- Q2 source pad: minimum 16 vias in 4 × 4 grid
- Shunt resistor pads: minimum 9 vias per pad
- Inductor footprint: 4 corner vias to improve heat spreading

### 4.4 Gate Drive Loop Minimisation

- Route gate and source of Q1 and Q2 as tightly coupled differential pairs
- Keep gate resistors (R_G) within 10 mm of MOSFET gate pin
- Separate gate-drive return from power-stage return; join only at the MOSFET source pad

### 4.5 Copper Weight

| Layer | Original | Upgraded |
|-------|----------|---------|
| F.Cu (Top) | 1 oz | **2 oz** |
| In1.Cu (GND) | 1 oz | **2 oz** |
| In2.Cu (PWR) | 1 oz | **2 oz** |
| B.Cu (Bottom) | 1 oz | **2 oz** |

---

## 5. Design Life Analysis

### 5.1 Capacitor Lifetime (Electrolytic)

Electrolytic capacitor lifetime follows the Arrhenius model:

```
L = L_0 × 2^((T_rated − T_op) / 10)

Nichicon UHE2C101MED (105 °C rated, L_0 = 5 000 h):
  T_op = T_amb + ΔT_ripple = 50 + 10 = 60 °C  (estimated ripple heating)
  L = 5 000 × 2^((105 − 60) / 10)
    = 5 000 × 2^4.5
    = 5 000 × 22.6
    = 113 000 h ≈ 12.9 years (continuous operation) ✓

For typical solar installation (6 h/day operation):
  Effective life = 113 000 / 6 ≈ 18 800 days ≈ 51 years ✓
```

### 5.2 MOSFET Gate Oxide Lifetime

Gate oxide stress is proportional to V_GS. At V_GS = 15 V and T_J < 150 °C, both devices are
within manufacturer-specified operating limits for 100 000+ hours.

### 5.3 Solder Joint Fatigue (Thermal Cycling)

For components subject to power cycling (MOSFETs, shunt):

- Use leaded or SAC305 solder on TO-220/TO-247 packages
- Apply thermal interface material (T_IM) with sufficient elasticity (e.g., Bergquist GP1500)
- Expected cycles to failure (Coffin-Manson): > 10⁵ cycles at ΔT = 30 °C → > 10 years @ 30 cycles/day

---

## 6. Summary of Upgrades

| Ref | Component | Original Spec | Upgraded Spec | Part Number |
|-----|-----------|--------------|--------------|-------------|
| Q1 | High-side MOSFET | 100 V, TO-220 | **650 V, 41 mΩ, TO-247** | Infineon IPW60R041P6 |
| Q2 | Low-side MOSFET | 100 V, TO-220 | **100 V, 1.5 mΩ, TO-220** | Infineon IPT015N10N5 |
| L1 | Power inductor | 20 A, µH TBD | **4.7 µH, 50 A, 2.4 mΩ DCR** | Würth 7443730470 |
| R_shunt | Current shunt | — | **1 mΩ, 3 W, 2512** | Bourns CRE2512-FZ-R001E-3 |
| C_in | Input capacitors | 100 V | **160 V, 4 × 100 µF** | Nichicon UHE2C101MED |
| C_out | Output capacitors | 35–50 V | **63 V, 4 × 680 µF** | Panasonic EEEFK1J681P |
| U_GD | Gate driver | — | **UCC27712 (150 V bootstrap)** | Texas Instruments |
| R_PV | PV voltage divider | 80 V range | **140 V range (1 MΩ / 20 kΩ)** | 1 % SMD |
| R_BAT | Battery divider | 32 V range | **58 V range (169 kΩ / 10 kΩ)** | 1 % SMD |
| U_CSA | Current sense amp | — | **INA240A3 + gain stage** | Texas Instruments |
| D_TVS1 | PV input TVS | — | **150 V, 600 W** | Littelfuse SMBJ150A |
| U_PS | Aux power supply IC | LMR16006 (42 V) | **LMR16030 (60 V)** | Texas Instruments |
| PCB | Copper weight | 1 oz | **2 oz all layers** | — |

---

## 7. Cost Impact Estimate

Estimates based on 10-unit prototype pricing (Mouser/Digi-Key, 2025):

| Item | Original BOM cost (est.) | Incremental cost | Notes |
|------|--------------------------|-----------------|-------|
| Q1 (IPW60R041P6) | — | +$4.50 | vs. generic 100 V FET |
| Q2 (IPT015N10N5) | — | +$3.00 | lower R_DS(on) |
| L1 (Würth 7443730470) | — | +$8.00 | vs. 20 A inductor |
| R_shunt (Bourns CRE2512) | — | +$0.80 | precision 1 mΩ |
| C_in (4 × 160 V) | — | +$6.00 | vs. 100 V caps |
| C_out (4 × 63 V) | — | +$4.50 | vs. 35 V caps |
| Gate driver (UCC27712) | — | +$2.50 | new component |
| Current sense amp | — | +$3.50 | new component |
| TVS diodes | — | +$1.50 | new components |
| Aux supply IC upgrade | — | +$1.00 | LMR16030 vs. LMR16006 |
| PCB 2 oz copper surcharge | — | +$15.00 | per prototype panel |
| **Total incremental cost** | | **≈ +$50.80** | per unit (prototype qty) |
| **Estimated production cost** | | **≈ +$25–35** | at 100+ units |

---

## 8. Performance Specifications

| Parameter | 24 V Mode | 48 V Mode | Unit |
|-----------|-----------|-----------|------|
| PV input voltage range | 15–140 | 15–140 | V |
| PV input voltage (max open-circuit) | 140 | 140 | V |
| Battery voltage range | 22–29 | 44–58 | V |
| Max output current | 42 | 21 | A |
| Max output power | 1 008 | 1 008 | W |
| Switching frequency | 80 | 80 | kHz |
| Peak efficiency | 97.3 | 97.3 | % |
| Efficiency @ full load, 50 °C | 95.6 | 97.3 | % |
| Standby power consumption | < 1 | < 1 | W |
| T_J(Q1) @ full load, 50 °C amb. | 143 | 125 | °C |
| T_J(Q2) @ full load, 50 °C amb. | 68 | 63 | °C |
| T_inductor @ full load, 50 °C amb. | 90 | 85 | °C |
| Ambient operating range | −20 to +50 | −20 to +50 | °C |
| Altitude (derating ≥ 2 000 m) | — | — | m |
| Design life (target) | 10+ | 10+ | years |

---

## Appendix A: Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-02-23 | Hardware team | Initial release |

---

**Document Version:** 1.0  
**Last Updated:** 2026-02-23  
**Status:** Draft — pending design review
