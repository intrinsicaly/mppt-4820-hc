/*
 * ADC Scaling Constants for MPPT-4820-HC
 * 48V/21A (1kW) MPPT Charge Controller
 *
 * Voltage divider values (all 0603, 1%):
 *   Solar input:   R13 = 499kΩ, R13b = 499kΩ (series), R14 = 20kΩ → ratio 50.9 (max 168V → 3.26V ADC)
 *   Battery 48V:   R17 = 169kΩ, R18 = 10kΩ  → ratio 17.9  (max 58V  → 3.24V ADC)
 *   Battery 24V:   R17 =  82kΩ, R18 = 10kΩ  → ratio  9.2  (max 30V  → 3.33V ADC)
 *
 * Current sense:
 *   Shunt: R6 = 1mΩ (Bourns CRE2512-FZ-R001E-3)
 *   Amplifier gain: INA240A3 = 50×
 */

#ifndef ADC_SCALING_H
#define ADC_SCALING_H

#include "dcdc_sync.h"

/* ADC reference and resolution */
#define ADC_VREF        3.3f
#define ADC_RESOLUTION  4096.0f

/* Solar input voltage (2×499k series + 20k divider) */
#define ADC_SCALE_V_SOLAR   (ADC_VREF / ADC_RESOLUTION * 50.9f)

/* Battery voltage – 48V system (169k:10k divider) */
#define ADC_SCALE_V_BAT_48V (ADC_VREF / ADC_RESOLUTION * 17.9f)

/* Battery voltage – 24V system (82k:10k divider) */
#define ADC_SCALE_V_BAT_24V (ADC_VREF / ADC_RESOLUTION * 9.2f)

/* DC/DC inductor current (1mΩ shunt, 50× gain) */
#define ADC_SCALE_I_DCDC    (ADC_VREF / ADC_RESOLUTION / CURRENT_SENSE_GAIN / CURRENT_SENSE_RESISTOR)

#endif /* ADC_SCALING_H */
