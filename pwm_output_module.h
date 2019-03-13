#ifndef PWM_OUTPUT_MODULE_H
#define PWM_OUTPUT_MODULE_H


/* sets up interrupts and all and the length of the cycle
 * Basically sets the frequency and resolution of pwm signals. TODO: write functions which allow
 * user to change time between interrupts and time spent in interrupts!
 */
void pwm_output_init();

/* adds another pwm output source, located at pin pin and with a duty cycle
 * starting_duty_cycle is an int from 0 to 2000
 * returns 1 for success, 0 otherwise
 */
int pwm_add_output(unsigned int pin, unsigned int starting_threshold);

/* removes an output 
 */
int pwm_remove_output(unsigned int pin);

/* changes duty cycle of existing pwm output 
 * returns 1 for success, 0 otherwise
 */
int pwm_change_threshold(unsigned int pin, unsigned int new_threshold);

/* returns the duty cycle of a pin, measured from o to 2000. 
 * -1 if error
 */
int get_threshold(unsigned int pin);

//a test file
int pwm_output_test(void);

unsigned int get_time_at_output_interrupt();

#endif
