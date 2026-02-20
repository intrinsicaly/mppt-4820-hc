/*
 * ADC Scaling Constants for MPPT-4820-HC
 * 48V/20A (1kW) MPPT Charge Controller
 *
 * Voltage divider values (all 0603, 1%):
 *   Solar input:   R13 = 470kΩ, R14 = 10kΩ  → ratio 48.0  (max 160V → 3.33V ADC)
 *   Battery 48V:   R17 = 180kΩ, R18 = 10kΩ  → ratio 19.0  (max 63V  → 3.33V ADC)
 *   Battery 24V:   R17 =  82kΩ, R18 = 10kΩ  → ratio  9.2  (max 40V  → 3.33V ADC)
 *
 * Current sense:
 *   Shunt: R6A||R6B = 1mΩ effective (2× CRE2512-FZ-R002E-3 in parallel)
 *   Amplifier gain: INA186A1 = 50×
 */

#ifndef ADC_SCALING_H
#define ADC_SCALING_H

#include "dcdc_sync.h"

/* ADC reference and resolution */
#define ADC_VREF        3.3f
#define ADC_RESOLUTION  4096.0f

/* Solar input voltage (470k:10k divider) */
#define ADC_SCALE_V_SOLAR   (ADC_VREF / ADC_RESOLUTION * 48.0f)

/* Battery voltage – 48V system (180k:10k divider) */
#define ADC_SCALE_V_BAT_48V (ADC_VREF / ADC_RESOLUTION * 19.0f)

/* Battery voltage – 24V system (82k:10k divider) */
#define ADC_SCALE_V_BAT_24V (ADC_VREF / ADC_RESOLUTION * 9.2f)

/* DC/DC inductor current (1mΩ shunt, 50× gain) */
#define ADC_SCALE_I_DCDC    (ADC_VREF / ADC_RESOLUTION / CURRENT_SENSE_GAIN / CURRENT_SENSE_RESISTOR)

#endif /* ADC_SCALING_H */
