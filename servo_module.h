#ifndef SERVO_MODULE_H
#define SERVO_MODULE_H

/* NOTE THAT THIS FILE SHOULD ASSUME THAT CYCLE LENGTH IS 20000 us (I think)
 */

void servo_init();

typedef volatile struct servo_motor servo;

/* creates a new servo object */
servo *servo_new(void);

void servo_setup(servo *servo);

void servo_auto_setup(servo *servo, unsigned int center_threshold, unsigned int min_threshold,
		      unsigned int max_threshold); 

/* writes angle to servo, 1 if succcess 0 if fail */
int servo_write_angle(servo *servo, unsigned int angle);

int servo_write_threshold(servo *servo, unsigned int threshold);

/* returns the angle that the servo is currently at */
unsigned int get_servo_position(servo *servo);

unsigned int get_servo_threshold(servo *servo);

#endif
