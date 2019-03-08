#ifndef CR_SERVO_MODULE_H
#define CR_SERVO_MODULE_H

/* this module rebrands servo_module 
 * to create a driver for continuous rotation servos
 */

/* this internal data structure keeps track of a servo object
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

#endif





