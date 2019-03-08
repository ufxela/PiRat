#ifndef SERVO_MODULE_H
#define SERVO_MODULE_H

void servo_init();

/* Servo object has the following information:
 * pin number 
 * input pin number (if applicable)
 * threshold value (1000 to 2000)
 * angle (0 to 180, maybe 0 to 360)
 * max threshold (1000 to 2000)
 * min threshold (1000 to 2000)
 * center threshold (1000 to 2000)
 */
typedef volatile struct servo_motor servo;

/* creates a new servo object, at location pin. 
 * makes calls to pwm_output_module.c, and sets a default value for the 
 * angle of the servo.
 */
servo *servo_new(unsigned int pin);

/* sets up servo (center it, find it's bounds) by prompting the user
 * and making movements. Updates the servo object to reflect setup
 */
void servo_setup(servo *servo);

/* automatically sets up servo to inputter parameters */
void servo_auto_setup(servo *servo, unsigned int center_threshold, unsigned int min_threshold,
		      unsigned int max_threshold); 

/* writes angle to servo, 1 if succcess 0 if fail */
int servo_write_angle(servo *servo, unsigned int angle);

/* writes a threshold to servo, 1 if success, 0 if fail */
int servo_write_threshold(servo *servo, unsigned int threshold);

/* returns the angle that the servo is currently at */
unsigned int get_servo_position(servo *servo);

/* returns the threshold the servo is currently at */
unsigned int get_servo_threshold(servo *servo);

#endif
