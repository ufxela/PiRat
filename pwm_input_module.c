#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"

void pwm_input_init();

int pwm_input_add_source(unsigned int pin);

int pwm_input_remove_source(unsigned int pin);

int pwm_input_read(unsigned int pin);

int pwm_input_get_cycle_length(unsignedint pin);

int pwm_input_get_number_sources();

int pwm_input_test();
