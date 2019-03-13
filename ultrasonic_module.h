#ifndef ULTRASONIC_MODULE_H
#define ULTRASONIC_MODULE_H

/* a setup file which setups interrupts, gpio pins */
void ultrasonic_init(unsigned int trig_pin, unsigned int echo_pin);

/* a basic, non interrupt time of flight function */
int get_time_of_flight_basic();

/* filters and averages several readings to get good values */
unsigned int get_ultrasonic_mean(int number_samples);

int ultrasonic_test(void);

#endif
