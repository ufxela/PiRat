#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"

/* internal data structure to keep track of what gpio pins
 * we're reading PWM inputs from.
 * also keeps track of the current readings from each pin
 * To keep track of current readings from pin (threshold, cycle length)
 * has an array which stores the times measured at the last rising edge
 * another array to measure times measured at the rising edge before that
 * and finally an array to measure time at the last falling edge
 * if falling_edge[i] - rising_edge[i] > 0, then a threshold is complete
 * and the value of threshold is this number.
 * the cycle length is rising_edge[i] - previous_rising_edge[i]
 */
unsigned int number_of_pwm_inputs = 0;
unsigned int * pwm_input_pins;
unsigned int * pwm_input_time_at_rising_edge;
unsigned int * pwm_input_time_at_previous_rising_edge;
unsigned int * pwm_input_time_at_falling_edge; 

bool pwm_input_rising_edge_handler(){
  int did_we_trigger = 0;
  for(int i = 0; i < number_of_pwm_inputs; i++){
    if(gpio_check_and_clear_event(pwm_input_pins[i])){
      did_we_trigger = 1;
      //crap this is a problem. How do we know if we're at a falling or a rising edge
      //I guess I'll have to intergrate pwm read with pwm write, with the timer interrupts
      //and in addition to writing, also include reading. 
    }
  }
}

void pwm_input_init(){
  
}

int pwm_input_add_source(unsigned int pin);

int pwm_input_remove_source(unsigned int pin);

int pwm_input_read(unsigned int pin);

int pwm_input_get_cycle_length(unsignedint pin);

int pwm_input_get_number_sources();

int pwm_input_test();
