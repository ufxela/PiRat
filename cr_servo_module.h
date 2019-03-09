#ifndef CR_SERVO_MODULE_H
#define CR_SERVO_MODULE_H

/* this module could rebrand servo_module 
 * to create a driver for continuous rotation servos
 * Could--but won't just because
 * I'm just gonna utilize ctrl c, ctrl v a lot instead lol
 * Hey--I'm lazy!
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

/* internally control throttle */
int cr_servo_write_throttle(cr_servo *cr_servo, unsigned int angle);

/* interally control thresholds */
int cr_servo_write_threshold(cr_servo *cr_servo, unsigned int threshold);

/* internally watch throttle */
unsigned int get_cr_servo_throttle(cr_servo *cr_servo);

/* internally watch threshold */
unsigned int get_cr_servo_threshold(cr_servo *cr_servo);

#endif





