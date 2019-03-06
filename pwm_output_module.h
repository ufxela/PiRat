#ifndef PWM_OUTPUT_MODULE_H
#define PWM_OUTPUT_MODULE_H


/* sets up interrupts and all as well as # of interrupts per pwm cycle and the length of the cycle
 * interrupts_per_cycle is basically the resolution
 */
void pwm_init(unsigned int interrupts_per_cycle, unsigned int cycle_length_in_us);

/* adds another pwm output source, located at pin pin and with a duty cycle
 * starting_duty_cycle is an int from 0 to interrupts_per_cycle, which is the resolution
 * returns 1 for success, 0 otherwise
 */
int pwm_add_output(unsigned int pin, unsigned int starting_duty_cycle);

/* removes an output 
 */
int pwm_remove_output(unsigned int pin);

/* changes duty cycle of existing pwm output 
 * returns 1 for success, 0 otherwise
 */
int pwm_change_duty_cycle(unsigned int pin, unsigned int new_duty_cycle);

/* returns the duty cycle of a pin, measured out of 0 from interrupts_per_cycle (resolution
 * -1 if error
 */
int get_duty_cycle(unsigned int pin);

int get_resolution(void);

#endif
