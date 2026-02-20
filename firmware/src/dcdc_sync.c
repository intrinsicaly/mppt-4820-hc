/*
 * Synchronous Buck Converter Implementation for MPPT-4820-HC
 *
 * Configures STM32G4 TIM1 for complementary PWM with hardware dead-time
 * to drive the IR2104 half-bridge gate driver.
 *
 * High-side:  TIM1_CH1  → IR2104 HIN → Q1A, Q1B (IPP200N25N3G × 2)
 * Low-side:   TIM1_CH1N → IR2104 LIN → Q4, Q5, Q6 (IPP200N25N3G × 3)
 */

#include "dcdc_sync.h"
#include "stm32g4xx_hal.h"
#include <math.h>

/* Private variables */
static TIM_HandleTypeDef htim1;
static bool fault_detected = false;

/**
 * @brief  Initialize synchronous buck converter.
 *         Configures TIM1 for complementary PWM with hardware dead-time.
 */
void dcdc_sync_init(void)
{
    /* Enable TIM1 clock */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* Calculate timer parameters (170MHz system clock assumed) */
    uint32_t timer_clock = HAL_RCC_GetPCLK2Freq() * 2;
    uint32_t period = (timer_clock / PWM_FREQUENCY) - 1;

    /* Dead-time counts: PWM_DEAD_TIME_NS at timer_clock Hz.
     * Reorder to avoid overflow: (timer_clock / 1000000U) gives MHz, then × ns / 1000. */
    uint32_t dead_time_counts = (timer_clock / 1000000U) * PWM_DEAD_TIME_NS / 1000U;

    htim1.Instance               = TIM1;
    htim1.Init.Prescaler         = 0;
    htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim1.Init.Period            = period;
    htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_PWM_Init(&htim1);

    /* Configure PWM channel 1 for complementary output */
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.Pulse        = 0;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

    /* Configure break and dead-time */
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
    sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_ENABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime         = (uint8_t)(dead_time_counts & 0xFFU);
    sBreakDeadTimeConfig.BreakState       = TIM_BREAK_ENABLE;
    sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter      = 0;
    sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;

    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

    /* Start complementary PWM on CH1 / CH1N */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

    fault_detected = false;
}

/**
 * @brief  Set PWM duty cycle.
 * @param  duty  Duty cycle in range [0.0, 1.0].
 *               Applied simultaneously to high-side (CH1) and complementary
 *               low-side (CH1N) with hardware-enforced dead-time.
 */
void dcdc_set_duty_cycle(float duty)
{
    if (duty > PWM_DUTY_MAX) {
        duty = PWM_DUTY_MAX;
    }
    if (duty < PWM_DUTY_MIN) {
        duty = PWM_DUTY_MIN;
    }

    if (fault_detected) {
        duty = 0.0f;
    }

    uint32_t pulse = (uint32_t)(duty * (float)(htim1.Init.Period + 1U));
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}

/**
 * @brief  Emergency stop – disable all PWM outputs immediately.
 */
void dcdc_emergency_stop(void)
{
    __HAL_TIM_MOE_DISABLE(&htim1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    fault_detected = true;
}

/**
 * @brief  Check for shoot-through or other hardware faults.
 * @return true if a fault is active, false otherwise.
 */
bool dcdc_check_fault(void)
{
    /* Hardware break flag indicates shoot-through protection triggered */
    if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_BREAK) != RESET) {
        __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_BREAK);
        dcdc_emergency_stop();
        return true;
    }

    return fault_detected;
}

/**
 * @brief  Detect discontinuous conduction mode (DCM).
 *         Used to optimize light-load efficiency by skipping low-side pulses.
 * @return true if operating in DCM, false if CCM.
 */
bool is_dcm_mode(void)
{
    extern float i_solar;
    extern float v_solar;
    extern float v_battery;

    float duty = (float)__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) /
                 (float)(htim1.Init.Period + 1U);

    /* ΔI_L = (V_in − V_out) × D / (L × f_sw) */
    const float L   = 47e-6f;  /* 47µH (Bourns SRR1260-470M) */
    const float f_sw = (float)PWM_FREQUENCY;
    float i_ripple  = (v_solar - v_battery) * duty / (L * f_sw);

    /* DCM when average current is less than half the ripple */
    return (fabsf(i_solar) < (i_ripple / 2.0f));
}
