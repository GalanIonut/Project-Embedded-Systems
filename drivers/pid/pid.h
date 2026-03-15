/**
 * @file    pid.h
 * @brief   PID controller — compile-time configuration & public API.
 *
 * ┌─────────────────────────────────────────────────────────────┐
 * │              ADC-Tunable PID Controller                     │
 * │                                                             │
 * │   Kp ──[pot 0]──►  [0.0 … 5.0]   proportional               │
 * │   Ki ──[pot 1]──►  [0.0 … 2.0]   integral                   │
 * │   Kd ──[pot 2]──►  [0.0 … 0.5]   derivative                 │
 * │                                                             │
 * │   u(t) = Kp·e + Ki·∫e dt + Kd·(de/dt)                       │
 * └─────────────────────────────────────────────────────────────┘
 *
 * @author  emi
 */

#ifndef PID_H
#define PID_H

 /* ─────────────────────────────────────────────────────────────────────────────
  * ADC CHANNEL MAPPING
  * Each gain is wired to a dedicated potentiometer on its own ADC channel.
  * Change these if your hardware layout differs.
  * ───────────────────────────────────────────────────────────────────────────── */
#define KP_CHANNEL  0   /**< ADC ch.0 → proportional gain potentiometer */
#define KI_CHANNEL  1   /**< ADC ch.1 → integral     gain potentiometer */
#define KD_CHANNEL  2   /**< ADC ch.2 → derivative   gain potentiometer */

  /* ─────────────────────────────────────────────────────────────────────────────
   * CONTROL LOOP TIMING
   * TIME_STEP must equal the actual period at which calculate_command() is called.
   * Mismatch here will silently scale your Ki and Kd — double-check against your
   * scheduler / timer ISR period.
   * ───────────────────────────────────────────────────────────────────────────── */
#define TIME_STEP   0.3f    /**< Control loop period [seconds] → 300 ms */

   /* ─────────────────────────────────────────────────────────────────────────────
    * GAIN READER API
    * Call these once per control cycle (before calculate_command) to sample the
    * current potentiometer positions and obtain fresh gain values.
    * ───────────────────────────────────────────────────────────────────────────── */

    /**
     * @brief  Sample proportional gain Kp via ADC.
     * @return Kp ∈ [0.0, 5.0]
     */
float calculate_Kp(void);

/**
 * @brief  Sample integral gain Ki via ADC.
 * @return Ki ∈ [0.0, 2.0]
 */
float calculate_Ki(void);

/**
 * @brief  Sample derivative gain Kd via ADC.
 * @return Kd ∈ [0.0, 0.5]
 */
float calculate_Kd(void);

/* ─────────────────────────────────────────────────────────────────────────────
 * PID COMMAND API
 * ───────────────────────────────────────────────────────────────────────────── */

 /**
  * @brief  Compute one PID control output.
  *
  * @param  kp          Proportional gain  — from calculate_Kp()
  * @param  ki          Integral gain      — from calculate_Ki()
  * @param  kd          Derivative gain    — from calculate_Kd()
  * @param  error       Current  error  =  setpoint − measurement
  * @param  prev_error  Error from the previous control cycle
  * @return Actuator command  (PWM duty cycle, voltage, etc.)
  *
  * @note   Persist prev_error in your control loop and update it each cycle:
  * @code
  *   float prev_error = 0.0f;
  *
  *   // inside the loop:
  *   float e   = setpoint - sensor_read();
  *   float cmd = calculate_command(kp, ki, kd, e, prev_error);
  *   actuator_set(cmd);
  *   prev_error = e;
  * @endcode
  */
float calculate_command(float kp, float ki, float kd,
    float error, float prev_error);

#endif /* PID_H */