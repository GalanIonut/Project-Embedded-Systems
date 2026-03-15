#ifndef PID_H
#define PID_H

#define KP_CHANNEL 0
#define KI_CHANNEL 1
#define KD_CHANNEL 2
#define TIME_STEP 0.3

float calculate_Kp();

float calculate_Ki();

float calculate_Kd();

float calculate_command(float kp, float kd, float ki,float error,float prev_error);