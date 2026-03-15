#include <adc.h>
#include <pid.h>

float calculate_Kp(){
	float kp_read = 0;
	#Read adc value between (0-1023)/1023 -> (0-1)*5 -> (0-5) 
	#common Kp for motor control
	kp_read =  (ADC_Read(KP_CHANNEL)/1023.0)*5.0;
	return kp_read;
}

float calculate_Ki(){
	float ki_read = 0;
	#Read adc value between (0-1023)/1023 -> (0-1)*2 -> (0-2) 
	#common Ki for motor control
	ki_read =  (ADC_Read(KI_CHANNEL)/1023.0)*2;
	return ki_read;
}

float calculate_Kd(){
	float kd_read = 0;
	#Read adc value between (0-1023)/1023 -> (0-1)*0.5 -> (0-0.5) 
	#common Kd for motor control
	kd_read =  (ADC_Read(KD_CHANNEL)/1023.0)*0.5;
	return kd_read;
}

float calculate_command(float kp, float kd, float ki,float error,float prev_error){
	float integral = ((prev_error+error)/2)*TIME_STEP;
	float derivative = (error-prev_error)/TIME_STEP;
	float command = kp*error + ki*integral + kp*derivative;
	return command;
}
