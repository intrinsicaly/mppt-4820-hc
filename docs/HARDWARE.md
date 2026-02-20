# MPPT 4820 HC Hardware Documentation

## Overview

The MPPT 4820 HC is a Maximum Power Point Tracking (MPPT) solar charge controller with high current capability. This document describes the hardware design files and project structure.

**Key Specifications:**
- Solar Input: Up to 80V (100V rated MOSFETs)
- DC/DC Converter: 20A inductor current capability
- Battery Output: 10V - 32V (12V/24V systems)
- Maximum Current: 20A
- Load Output: 20A switching capability
- MCU: STM32G431 ARM Cortex-M4
- Communication: CAN bus via RJ45 connectors
- Expansion: UEXT connector (Olimex Universal Extension)

## Project Status

**Status:** Prototype built and tested, development ongoing

## Directory Structure

```
kicad/
├── mppt-4820-hc.sch          # Top-level schematic (hierarchical)
├── dcdc.sch                   # DC/DC converter stage (Sheet 2)
├── power-supply.sch           # Power supply circuitry (Sheet 3)
├── mcu.sch                    # Microcontroller and peripherals (Sheet 4)
├── load-switch.sch            # Load switching circuit (Sheet 5)
├── can.sch                    # CAN bus interface (Sheet 6)
├── project.lib                # Custom component library
├── project.dcm                # Component documentation
├── mppt-4820-hc-cache.lib     # Cached library components
├── sym-lib-table              # Symbol library table
├── fp-lib-table               # Footprint library table
├── mppt-4820-hc.pro           # KiCad project settings
├── bom.ini                    # Bill of Materials configuration
└── libre-solar.kicad_wks      # Custom worksheet template
```

## Schematic Hierarchy

The design uses a hierarchical structure with 6 sheets:

### Sheet 1: Top Level (mppt-4820-hc.sch)
- Project overview and sheet interconnections
- Top-level block diagram
- Sheet connector definitions

### Sheet 2: DC/DC Converter (dcdc.sch)
**Purpose:** Main power conversion stage

**Key Components:**
- High-side and low-side MOSFETs (100V rated)
- Power inductor (high current capability)
- Input/output filtering capacitors
- Current sensing circuitry
- Voltage dividers for monitoring

**Signals:**
- `DCDC_HV+/DCDC_HV-`: High voltage (solar) side
- `DCDC_LV+`: Low voltage (battery) side
- `PWM_HS/PWM_LS`: PWM control signals
- `V_DCDC_H/V_DCDC_L`: Voltage sensing
- `I_DCDC`: Current sensing

### Sheet 3: Power Supply (power-supply.sch)
**Purpose:** Generate low-voltage rails for control circuits

**Features:**
- Buck converter from battery voltage
- Multiple voltage rails (likely 3.3V, 5V)
- Charge pump for high-side gate drive
- Protection and filtering

### Sheet 4: MCU (mcu.sch)
**Purpose:** Microcontroller and core peripherals

**Key Components:**
- STM32G431 microcontroller
- Crystal oscillator
- EEPROM (24AA01 - 1K I2C)
- Programming header (SWD)
- Reset circuitry
- Decoupling capacitors

**Features:**
- Advanced timer for PWM generation
- Multiple ADC channels for sensing
- I2C for EEPROM communication
- GPIO for control and status

### Sheet 5: Load Switch (load-switch.sch)
**Purpose:** Controllable load output switching

**Features:**
- High-side or low-side switching (20A capability)
- Current sensing on load output
- Protection circuitry
- PWM dimming capability (optional)

### Sheet 6: CAN Bus (can.sch)
**Purpose:** CAN communication interface

**Key Components:**
- TCAN334 CAN transceiver
- RJ45 connectors for CAN bus
- Termination resistor (jumper selectable)
- ESD protection

**Features:**
- Differential CAN signaling
- Bus termination option
- Daisy-chain capability via dual RJ45

## Component Libraries

### Custom Library (project.lib)
Contains project-specific components including:

**Power Components:**
- Power supply symbols (+10V, +12V references)
- Battery symbol

**ICs:**
- STM32F072RBTx (ARM Cortex-M0 - earlier version)
- STM32G431 (current MCU - ARM Cortex-M4)
- 24AA01 (I2C EEPROM)
- AP2210-3.3 (LDO regulator)
- AZ1117 (voltage regulator)
- LMR16006 (buck converter)
- TCAN334 (CAN transceiver)

**Passive Components:**
- Resistors (R), capacitors (C), inductors (L)
- Resistor packs (R_PACK4, R_PACK8)
- Potentiometers

**Semiconductors:**
- Diodes (D, D_Schottky, D_Zener)
- MOSFETs (Q_NMOS)

**Connectors:**
- Generic connectors
- 8P8C (RJ45)
- UEXT connector

### External Libraries

**LibreSolar Footprints:**
- URL: https://github.com/LibreSolar/kicad-footprints
- Custom footprints for LibreSolar projects
- Optimized for manufacturing

## Bill of Materials (BOM)

### BOM Configuration (bom.ini)

**Features:**
- Grouped by component value and footprint
- DNF (Do Not Fit) component filtering
- Multiple configuration support (+can, +hv_supply variants)
- CSV and HTML output formats

**Output Location:** `../build/mppt-4820-hc_bom_(variant).csv`

**Interactive BOM:** Available at `build/mppt-4820-hc_ibom.html`

### Component Sourcing

Key components require specific attention:

1. **MOSFETs:** 100V rated, low Rds(on)
2. **Inductor:** High current (20A+), low DCR
3. **MCU:** STM32G431 (specific package)
4. **CAN Transceiver:** TCAN334 or compatible

## PCB Design Specifications

### Layer Stack (from mppt-4820-hc.pro)

**4-layer board:**
1. **F.Cu (Top):** Signal and component layer
2. **In1.Cu (GND):** Ground plane
3. **In2.Cu (3V3):** Power plane (3.3V)
4. **B.Cu (Bottom):** Signal and component layer

### Design Rules

**Track Widths:**
- Default: 0.25mm
- Power traces: 0.3mm, 0.4mm, 0.5mm options
- High current: Custom width per trace

**Via Specifications:**
- Minimum diameter: 0.7mm
- Drill: 0.3mm
- Power vias: up to 1.0mm diameter

**Clearances:**
- Minimum track width: 0.2mm
- Minimum hole-to-hole: 0.25mm
- Soldermask clearance: As per manufacturer

**Board Parameters:**
- Thickness: 1.6mm
- Copper layers: 4
- No blind/buried vias

## Manufacturing Files

Generated files in `build/` directory:

```
build/
├── mppt-4820-hc_schematic.pdf     # Schematic documentation
├── mppt-4820-hc_bom_*.csv         # Bill of materials
├── mppt-4820-hc_ibom.html         # Interactive BOM
└── mppt-4820-hc_board.jpg         # Board render
```

## Working with the Project

### Prerequisites

**Required Software:**
- KiCad 5.x or later (project uses v5.1+ features)
- Git (for version control)

**Recommended:**
- KiBoM plugin (for BOM generation)
- InteractiveHtmlBom plugin

### Opening the Project

1. Clone the repository
2. Open KiCad
3. File → Open Project
4. Navigate to `kicad/mppt-4820-hc.pro`

### Library Dependencies

The project uses:
- Standard KiCad libraries
- LibreSolar custom footprints (from GitHub)
- Project-specific library (project.lib)

**Library Setup:**
- Symbol libraries: Defined in `sym-lib-table`
- Footprint libraries: Defined in `fp-lib-table`
- Both use relative paths for portability

### Generating Manufacturing Files

**Gerbers:**
1. Open PCB editor (Pcbnew)
2. File → Plot
3. Select Gerber format
4. Generate drill files

**BOM:**
```bash
# Using KiBoM (if installed)
python3 -m kibom kicad/mppt-4820-hc.xml build/ -c kicad/bom.ini
```

**Assembly Files:**
- Position files: Generated from Pcbnew
- Interactive BOM: Use InteractiveHtmlBom plugin

## Variants and Configurations

The project supports multiple build configurations using the "Config" field:

### Variant: +can
Includes CAN bus components:
- TCAN334 transceiver
- RJ45 connectors
- Termination resistor

### Variant: +hv_supply
High voltage supply option:
- Alternative input filtering
- Extended voltage range components

### Variant: default
Base configuration without optional features

## Firmware Integration

**Firmware Repository:** [LibreSolar/charge-controller-firmware](https://github.com/LibreSolar/charge-controller-firmware)

**Hardware-Firmware Interface:**
- PWM outputs for MOSFET control
- ADC inputs for voltage/current sensing
- I2C for EEPROM
- CAN bus for communication
- GPIO for status LEDs and enable signals

## Revision History

- **v0.2.3** (2021-05-03): Current revision
  - Schematic updates
  - Component value optimizations

- Earlier versions documented in git history

## Design Notes

### Critical Circuits

**High-Side Gate Drive:**
- Requires charge pump or bootstrap circuit
- Ensure sufficient gate voltage for full enhancement
- Dead-time control to prevent shoot-through

**Current Sensing:**
- Shunt resistor location and value critical
- Filtering required for ADC accuracy
- Consider temperature effects

**Power Inductor:**
- Selection based on peak current, DCR, and saturation
- Core material affects efficiency
- Physical size constraint

### Safety Considerations

1. **Reverse Polarity Protection:** Verify on input and output
2. **Overcurrent Protection:** Hardware and software limits
3. **Overvoltage Protection:** MOV or TVS diodes
4. **Thermal Management:** Heat sink required for MOSFETs

### Testing Points

Recommended test points (if not already present):
- Solar input voltage
- Battery voltage
- Load voltage
- Inductor current sense
- Gate drive signals
- MCU power rails

## License

This hardware design is licensed under CERN-OHL-W version 2 (weakly reciprocal).

**Copyright © 2020 Libre Solar Technologies GmbH**

See [LICENCE](../LICENCE) file for full license text.

## Contributing

Contributions are welcome! Please see the main project repository for:
- Issue tracking
- Pull request guidelines
- Development discussion

## Support and Documentation

- **Main Repository:** https://github.com/intrinsicaly/mppt-4820-hc
- **Libre Solar Website:** https://libre.solar
- **Community Forum:** (link TBD)
- **Schematic PDF:** [build/mppt-4820-hc_schematic.pdf](../build/mppt-4820-hc_schematic.pdf)

## Related Projects

- **MPPT 2420 HC:** Lower voltage/current variant
- **Charge Controller Firmware:** Compatible firmware
- **LibreSolar BMS:** Battery management integration

---

**Document Version:** 1.0  
**Last Updated:** 2026-02-20  
**Author:** Martin Jäger (original hardware), Documentation team
