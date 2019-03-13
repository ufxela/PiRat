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

/* changes duty cycle of existing pwm output 
 * returns 1 for success, 0 otherwise
 */
int pwm_change_threshold(unsigned int pin, unsigned int new_threshold);

/* returns the duty cycle of a pin, measured from o to 2000. 
 * -1 if error
 */
int get_threshold(unsigned int pin);

//a test program
int pwm_output_test(void);

/* gets the time that the interrupt will occur at / occured at to help other modules
 * used in sync with this module that are time sensitive to coordinate their actions
 * so as not to be interrupted by this module 
 */
unsigned int get_time_at_output_interrupt();

#endif
