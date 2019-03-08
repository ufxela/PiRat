#ifndef PWM_INPUT_MODULE_H
#define PWM_INPUT_MODULE_H

/* setsup interupts, other setup stuff */
void pwm_input_init();

/* adds an input channel for PWM reading
 * returns 0 if failure, 1 if success
 */
int pwm_add_input(unsigned int pin);

/* removes an input channel for PWM reading
 * 0 if failure, 1 if success
 */
int pwm_remove_input(unsigned int pin);

/* gets current threshold value for PWM source
 * it's returned as an int from 0 to cycle_length
 * duty cycle = threshold / cycle_length
 * units of thresholds is microseconds (us)
 * -1 if invalid
 */
int pwm_input_get_threshold(unsigned int pin);

/* gets the cycle length for a PWM source
 * value returned is in units of microseconds (us)
 * -1 if invalid 
 */
int pwm_input_get_cycle_length(unsigned int pin);

/* returns the number of sources that are accepting PWM inputs
 */
int pwm_input_get_number_sources();

/* test function that showcases functionality of this module
 */
int pwm_input_test();

#endif
