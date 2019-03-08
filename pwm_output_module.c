/* a module for sending PWM signals to servos. Leverages the fact that duty cycles
 * are between 5 and 10%
 * uses interrupts. 
 */
#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"
#include "armtimer.h"
#include "malloc.h"
#include "printf.h"

/* idk... just a placeholder for now, can definitely do more outputs
 */
const unsigned int MAX_PWM_OUTPUTS = 8;

/* this data structure holds each of the pins which are available for use, and the "thresholds"
 * for each pin.
 * thresholds are the marker, in microseconds, of when we should write 0 instead of 1 in the 
 * pwm cycle.
 */
volatile unsigned int number_of_pwm_outputs = 0;
volatile unsigned int * pwm_output_pins; 
volatile unsigned int * pwm_output_thresholds;

/*TODO:
 * WRITE A FUNCTION CALLED SET DUTY CYCLE which sets duty cycle
 * or an angle function
 * or a throttle function
 * perhaps these are better to do in a different module that adds a layer of abstraction.
 */

//TODO:
//make functions to allow for these to be changed
const unsigned int pwm_output_cycle_length_us = 20000; //20 ms, for servos.
const unsigned int pwm_output_max_threshold = 2000; //2ms, corresponds to max of 10% duty cycle
const unsigned int pwm_output_min_threshold = 1000;

//helper function which gets the index of a pin. -1 if not existent
//TODO: use this function in this module
static int pwm_output_get_pin_index(unsigned int pin){
  for(int i = 0; i < number_of_pwm_outputs; i++){
    if(pwm_output_pins[i] == pin){
      return i;
    }
  }
  return -1; 
}

static bool pwm_output_handler(unsigned int pc){
  //check if we cause the interrupt:
  if(armtimer_check_and_clear_interrupt()){
    //    printf("in handler. Time: %d\n", timer_get_ticks());
    //get the current time. QUESTION Is this limited by the 35 minute wraparound?
    unsigned int start_time = timer_get_ticks();

    //write 1 to all servo channels to start duty cycle
    for(int i = 0; i < number_of_pwm_outputs; i++){
      gpio_write(pwm_output_pins[i], 1);
      //  printf("written 1 to pin %d, time %d\n", pwm_output_pins[i], timer_get_ticks());
    }
    
    //loop through and set output to zero once threshold is reached
    unsigned int current_time = timer_get_ticks() - start_time;
    //at 2ms mark, we're done
    while(current_time < pwm_output_max_threshold){
      current_time = timer_get_ticks() - start_time; 
      //check thresholds
      for(int i = 0; i < number_of_pwm_outputs; i++){
	//check if we should kill the output
	if(pwm_output_thresholds[i] <= current_time){
	  gpio_write(pwm_output_pins[i], 0); //this is writing 0 an unnecessary large number of times...
	  //  printf("written 0 to pin %d, time %d\n", pwm_output_pins[i], timer_get_ticks());
	}
      }
    }

    //we're almost done: just make sure everything is turned off
    for(int i = 0; i < number_of_pwm_outputs; i++){
      gpio_write(pwm_output_pins[i], 0);
    }
    //    printf("exiting handler, time: %d\n", timer_get_ticks()); 
    return true;
  }else{
    return false;
  }
}

//setup everything
void pwm_output_init(){
  //setup data structure
  pwm_output_pins = malloc(4 * number_of_pwm_outputs);
  pwm_output_thresholds = malloc(4 * number_of_pwm_outputs);

  //initialize interrupts 
  armtimer_init(pwm_output_cycle_length_us);
  armtimer_enable();
  armtimer_enable_interrupts();
  interrupts_enable_basic(INTERRUPTS_BASIC_ARM_TIMER_IRQ);
  interrupts_attach_handler(pwm_output_handler);
  interrupts_global_enable();
}

int get_number_pwm_outputs(void){
  return number_of_pwm_outputs;
} 

int pwm_add_output(unsigned int pin, unsigned int starting_threshold){
  if(number_of_pwm_outputs < MAX_PWM_OUTPUTS){
    //make more room in data structure
    pwm_output_pins = realloc((void *) pwm_output_pins, 4 * (number_of_pwm_outputs + 1));
    pwm_output_thresholds = realloc((void *) pwm_output_thresholds, 4 * (number_of_pwm_outputs + 1));

    //set the proper pin to output
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);

    //initialize data
    pwm_output_pins[number_of_pwm_outputs] = pin;
    pwm_output_thresholds[number_of_pwm_outputs] = starting_threshold;

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

/* BIG NOTE: should this function wait until the current cycle is completed before changing
 * the duty cycle? Probably
 */
int pwm_change_threshold(unsigned int pin, unsigned int new_threshold){
  if(new_threshold >= pwm_output_min_threshold && new_threshold <= pwm_output_max_threshold){
    //loop over, find pin, change threshold. 
    for(int i = 0; i < get_number_pwm_outputs(); i++){
      if(pwm_output_pins[i] == pin){
	pwm_output_thresholds[i] = new_threshold;
	break; //ugly, but safes unnecessary looping
      }
    }
    
    //should return 0 if looped over and pin wasn't in use.
    return 1;
  }else{
    return 0;
  }
}

int get_threshold(unsigned int pin){
  int pin_index = pwm_output_get_pin_index(pin);
  if(pin_index >= 0){
    return pwm_output_thresholds[pin_index];
  }else{
    return -1;
  }
}

int pwm_output_test(void){
  // printf("starting pwm output test\n");
  pwm_add_output(GPIO_PIN4, 1200);
  //printf("output added");
  while(1){
    //printf("in loop \n");
    pwm_change_threshold(GPIO_PIN4, 1200);
    //printf("1");
    timer_delay_ms(100);
    //printf("2");
    pwm_change_threshold(GPIO_PIN4, 1700);
    //printf("3");
    timer_delay_ms(100);
    //    printf("GPIO_PIN%d threshold: %d", GPIO_PIN4, get_threshold(GPIO_PIN4));
    //printf("beginning next iteration");
  }
  return 1;
}

