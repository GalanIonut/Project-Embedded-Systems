/*
 * ADC-tunable PID controller for DC motor speed/position control.
 *
 * Kp, Ki, and Kd are read live from potentiometers via ADC channels,
 * allowing real-time tuning without reflashing firmware.
 *
 * Gain ranges (chosen for typical brushed DC motor loops):
 *   Kp → [0.0, 5.0]   proportional — main correction force
 *   Ki → [0.0, 2.0]   integral     — eliminates steady-state error
 *   Kd → [0.0, 0.5]   derivative   — damps oscillation
 *
 * TIME_STEP must match the control-loop period in seconds.
 */

#include "drivers/adc/adc.h"
#include "pid.h"

/* -------------------------------------------------------------------------
 * GAIN READERS
 * Each function maps a 10-bit ADC reading (0–1023) to its gain range.
 * ------------------------------------------------------------------------- */

float calculate_Kp(void) {
    return (ADC_Read(KP_CHANNEL) / 1023.0f) * 5.0f;
}

float calculate_Ki(void) {
    return (ADC_Read(KI_CHANNEL) / 1023.0f) * 2.0f;
}

float calculate_Kd(void) {
    return (ADC_Read(KD_CHANNEL) / 1023.0f) * 0.5f;
}

/* -------------------------------------------------------------------------
 * PID OUTPUT
 * ------------------------------------------------------------------------- */

float calculate_command(float kp, float ki, float kd,
                        float error, float prev_error) {
    /* Integral term: trapezoidal approximation over one time step */
    float integral = ((prev_error + error) / 2.0f) * TIME_STEP;

    /* Derivative term: rate of error change over one time step */
    float derivative = (error - prev_error) / TIME_STEP;

    return (kp * error) + (ki * integral) + (kd * derivative);
}
