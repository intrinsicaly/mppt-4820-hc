/*
 * MPPT Control Loop for MPPT-4820-HC
 * 48V/20A (1kW) MPPT Charge Controller
 *
 * Integrates synchronous buck converter control with the MPPT algorithm.
 * Firmware companion to LibreSolar/charge-controller-firmware.
 */

#include "dcdc_sync.h"
#include "adc_scaling.h"

/* System state definitions */
typedef enum {
    STATE_IDLE,
    STATE_MPPT,
    STATE_CV,
    STATE_FAULT
} mppt_state_t;

/* Shared ADC results (populated by ADC interrupt handler) */
float v_solar   = 0.0f;
float v_battery = 0.0f;
float i_solar   = 0.0f;

static mppt_state_t system_state = STATE_IDLE;
static float mppt_duty   = PWM_DUTY_MIN;
static float prev_power  = 0.0f;
static float prev_duty   = 0.0f;
static const float step  = 0.001f;

/**
 * @brief  Initialize the MPPT charge controller.
 *         Call once at startup before entering the main control loop.
 */
void mppt_init(void)
{
    /* Initialize synchronous buck converter (TIM1 complementary PWM) */
    dcdc_sync_init();

    /* Reset control state */
    mppt_duty  = PWM_DUTY_MIN;
    prev_power = 0.0f;
    prev_duty  = 0.0f;

    system_state = STATE_MPPT;
}

/**
 * @brief  Main MPPT control loop iteration.
 *         Call periodically from a timer interrupt or RTOS task (e.g. 10ms).
 */
void mppt_control_loop(void)
{
    /* Check for hardware converter faults (shoot-through protection) */
    if (dcdc_check_fault()) {
        system_state = STATE_FAULT;
        return;
    }

    if (system_state == STATE_FAULT) {
        return;
    }

    /*
     * Perturb-and-observe MPPT algorithm.
     * The duty cycle is adjusted in small steps to track the maximum power
     * point of the solar panel.
     */
    float power = v_solar * i_solar;

    if (power > prev_power) {
        /* Moving toward MPP – continue in the same direction */
        if (mppt_duty >= prev_duty) {
            mppt_duty += step;
        } else {
            mppt_duty -= step;
        }
    } else {
        /* Moving away from MPP – reverse direction */
        if (mppt_duty >= prev_duty) {
            mppt_duty -= step;
        } else {
            mppt_duty += step;
        }
    }

    prev_power = power;
    prev_duty  = mppt_duty;

    /* Apply updated duty cycle to the synchronous converter */
    dcdc_set_duty_cycle(mppt_duty);
}
