/**
 * @file    pid_controller.c
 * @brief   ADC-tunable PID controller for DC motor speed/position control.
 *
 * Kp, Ki, and Kd are read live from potentiometers via ADC channels,
 * allowing real-time tuning without reflashing firmware.
 *
 * Gain ranges (chosen for typical brushed DC motor loops):
 *   Kp  →  [0.0,  5.0]   (proportional – main correction force)
 *   Ki  →  [0.0,  2.0]   (integral     – eliminates steady-state error)
 *   Kd  →  [0.0,  0.5]   (derivative   – damps oscillation)
 *
 * @note    TIME_STEP must match the control-loop period (seconds).
 */

#include <adc.h>
#include <pid.h>

 /* ─────────────────────────────────────────────────────────────────────────────
  * GAIN READERS
  * Each function maps a 10-bit ADC reading (0–1023) to its physical gain range.
  * ───────────────────────────────────────────────────────────────────────────── */

  /**
   * @brief  Read proportional gain Kp from a potentiometer.
   * @return Kp in the range [0.0, 5.0]
   *
   * ADC (0–1023) → normalise ÷1023 → scale ×5.0
   */
float calculate_Kp(void)
{
    return (ADC_Read(KP_CHANNEL) / 1023.0f) * 5.0f;
}

/**
 * @brief  Read integral gain Ki from a potentiometer.
 * @return Ki in the range [0.0, 2.0]
 *
 * ADC (0–1023) → normalise ÷1023 → scale ×2.0
 */
float calculate_Ki(void)
{
    return (ADC_Read(KI_CHANNEL) / 1023.0f) * 2.0f;
}

/**
 * @brief  Read derivative gain Kd from a potentiometer.
 * @return Kd in the range [0.0, 0.5]
 *
 * ADC (0–1023) → normalise ÷1023 → scale ×0.5
 */
float calculate_Kd(void)
{
    return (ADC_Read(KD_CHANNEL) / 1023.0f) * 0.5f;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * PID OUTPUT
 * ───────────────────────────────────────────────────────────────────────────── */

 /**
  * @brief  Compute one PID control step.
  *
  * Uses the trapezoidal (bilinear) rule for the integral term and a
  * backward-difference approximation for the derivative:
  *
  *   integral   = ((prev_error + error) / 2) × TIME_STEP   [trapezoidal]
  *   derivative = (error − prev_error) / TIME_STEP          [backward diff]
  *   command    = Kp×error + Ki×integral + Kd×derivative
  *
  * @param  kp          Proportional gain (from calculate_Kp)
  * @param  ki          Integral gain     (from calculate_Ki)
  * @param  kd          Derivative gain   (from calculate_Kd)
  * @param  error       Current  error  = setpoint − measured_value
  * @param  prev_error  Previous error (saved from the last control cycle)
  * @return Control command to be forwarded to the actuator / PWM stage
  */
float calculate_command(float kp, float ki, float kd,
    float error, float prev_error)
{
    /* Integral term: trapezoidal approximation over one time step */
    float integral = ((prev_error + error) / 2.0f) * TIME_STEP;

    /* Derivative term: rate of error change over one time step */
    float derivative = (error - prev_error) / TIME_STEP;

    /* PID sum */
    float command = (kp * error)
        + (ki * integral)
        + (kd * derivative); 

    return command;
}