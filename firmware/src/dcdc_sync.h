/*
 * Synchronous Buck Converter Control for MPPT-4820-HC
 * 48V/20A (1kW) MPPT Charge Controller
 *
 * Features:
 * - Complementary PWM with hardware dead-time
 * - Shoot-through protection
 * - DCM detection for light-load efficiency
 */

#ifndef DCDC_SYNC_H
#define DCDC_SYNC_H

#include <stdint.h>
#include <stdbool.h>

/* Configuration for 48V/20A operation */
#define PV_VOLTAGE_MAX              140.0f  /* Solar input max (V) */
#define BATTERY_VOLTAGE_NOMINAL     48.0f   /* 48V system */
#define BATTERY_VOLTAGE_MAX         58.4f   /* 14.6V/cell × 4 */
#define BATTERY_CURRENT_MAX         20.0f   /* Charge current limit (A) */

/*
 * For 24V/42A operation (alternative):
 * #define BATTERY_VOLTAGE_NOMINAL  24.0f
 * #define BATTERY_VOLTAGE_MAX      29.2f
 * #define BATTERY_CURRENT_MAX      42.0f
 */

/* ADC scaling for new voltage dividers */
#define VPV_ADC_SCALE               48.0f   /* (470k + 10k) / 10k */
#define VBAT_ADC_SCALE_48V          19.0f   /* (180k + 10k) / 10k */
#define VBAT_ADC_SCALE_24V          9.2f    /* (82k + 10k) / 10k */

/* Current sense scaling (1mΩ resistor, 50× gain) */
#define CURRENT_SENSE_GAIN          50.0f
#define CURRENT_SENSE_RESISTOR      0.001f  /* 1mΩ */

/* PWM configuration */
#define PWM_FREQUENCY               50000   /* 50kHz */
#define PWM_DEAD_TIME_NS            500     /* 500ns dead-time */
#define PWM_DUTY_MAX                0.95f   /* 95% max duty cycle */
#define PWM_DUTY_MIN                0.05f   /* 5% min duty cycle */

/* Function prototypes */
void dcdc_sync_init(void);
void dcdc_set_duty_cycle(float duty);
void dcdc_emergency_stop(void);
bool dcdc_check_fault(void);
bool is_dcm_mode(void);

#endif /* DCDC_SYNC_H */
