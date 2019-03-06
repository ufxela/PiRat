/* NOTE since I have a different pwm read and write file, may be nice if made functions names
 * more specific for output in this module
 */
#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"

/* idk... just a placeholder for now, can definitely do more outputs
 */
const unsigned int MAX_PWM_OUTPUTS = 8;

/* this data structure holds each of the pins which are available for use, and the "thresholds"
 * for each pin.
 * thresholds are when in the pwm cycle that we should output 1 and when we should do 0.
 */
unsigned int number_of_pwm_outputs = 0;
unsigned int * pwm_output_pins; 
unsigned int * pwm_output_thresholds;

unsigned int pwm_output_resolution = 0;

//will calculate the time between interrupts, and then setup interrupts to trigger then
void pwm_init(unsigned int interrupts_per_cycle, unsigned int cycle_length_in_us){
  //setup data structure
  //Q: function for this?
  pwm_output_pins = malloc(4 * number_of_pwm_outputs);
  pwm_output_thresholds = malloc(4 * number_of_pwm_outputs);

  pwm_output_resolution = interrupts_per_cycle;

  //setup interrupts
}

static bool pwm_write_timer_interrupt_handler(unsigned int pc){
}

int pwm_add_output(unsigned int pin, unsigned int starting_duty_cycle){
  if(number_of_pwm_outputs < MAX_PWM_OUTPUTS){
    //make more room in data structure
    pwm_output_pins = realloc((void *) pwm_output_pins, 4 * (number_of_pwm_outputs + 1));
    pwm_output_thresholds = realloc((void *) pwm_output_thresholds, 4 * (number_of_pwm_outputs + 1));

    //initialize data
    pwm_output_pins[number_of_pwm_outputs] = pin;
    pwm_output_thresholds[number_of_pwm_outputs] = starting_duty_cycle;

    //update number of pwm outputs, done at end to avoid messyness is interrupt happens in the middle
    number_of_pwm_outputs++;
    
    return 1;
  }
  else{
    return 0;
  }
}


int pwm_remove_output(unsigned int pin){
  return 0;
}

int pwm_change_duty_cycle(unsigned int pin, unsigned int new_duty_cycle){
  if(new_duty_cycle < get_resolution()){
    //loop over, find pin, change threshold. 
    for(int i = 0; i < get_number_pwm_outputs(); i++){
      if(pwm_output_pins[i] == pin){
	pwm_output_thresholds[i] = new_duty_cycle;
	break; //ugly, but safes unnecessary looping
      }
    }

    //should return 0 if looped over and pin wasn't in use.
    return 1;
  }else{
    return 0;
  }
}

int get_duty_cycle(unsigned int pin){
  for(int i = 0; i < get_number_pwm_outputs(); i++){
    if(pwm_output_pins[i] == pin){
      return pwm_output_thresholds[i];
    }
  }
  return -1; //not found
}

int get_resolution(void){
  return pwm_output_resolution;
}

int get_number_pwm_outputs(void){
  return number_of_pwm_outputs;
} 
