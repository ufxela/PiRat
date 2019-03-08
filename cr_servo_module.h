#ifndef CR_SERVO_MODULE_H
#define CR_SERVO_MODULE_H

/* this module rebrands servo_module as well as adds pwm_input feedback
 * to create a driver for the parallax feedback 360 servo
 */

/* this internal data structure keeps track of a servo object
 * but also has two ints, one which keeps track of 
 * how many rotations the servo motor is at (from -infinity to infinity)
 * as well as the relative angle that the servo is at [0, 360).
 * These two parameters together keep track of the absolute position of the servo
 */
typedef volatile struct cr_servo_motor cr_servo;

/* sets everything up */
void cr_servo_init();

/* make a new cr_servo (continuous rotation servo) object */
cr_servo *cr_servo_new(unsigned int pin);

/* similar setup to servo_module's */
void cr_servo_setup(cr_servo *cr_servo);

/* similar to servo_module */
void cr_servo_auto_setup(cr_servo *cr_servo, unsigned int center_threshold, unsigned int min_threshold,
                      unsigned int max_threshold);

/* would be nice if user could set the throttle which the motor operates at
 * to move to a given position 
 * maybe that's what I could use these below functions for
 */

/* internally control throttle */
static int cr_servo_write_throttle(cr_servo *cr_servo, unsigned int angle);

/* interally control thresholds */
static int cr_servo_write_threshold(cr_servo *cr_servo, unsigned int threshold);

/* internally watch throttle */
static unsigned int get_cr_servo_throttle(cr_servo *cr_servo);

/* internally watch threshold */
static unsigned int get_cr_servo_threshold(cr_servo *cr_servo);

/* moves the servo arm to the specified absolute position, using throttle management and feedback
 * angle is from -infinity to infinity
 * this function is the MEAT: it updates the rotations / angle as it goes. 
 */
int go_to_absolute_position(cr_servo * cr_servo, int angle);

/* returns the absolute angle, in degrees that we're at
 */
int get_absolute_position(cr_servo * cr_servo);

#endif





