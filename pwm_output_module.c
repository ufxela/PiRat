/* NOTE since I have a different pwm read and write file, may be nice if made functions names
 * more specific for output in this module
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
 * thresholds are when in the pwm cycle that we should output 1 and when we should do 0.
 */
unsigned int number_of_pwm_outputs = 0;
unsigned int * pwm_output_pins; 
unsigned int * pwm_output_thresholds;

//number of interrupts per pwm cycle & length of cycle in us
unsigned int pwm_output_resolution = 0;
unsigned int pwm_output_cycle_length_us = 0; //Q: is this variable necessary?

//keeps track of which timer interrupt we're currently on, to compare against thresholds
volatile unsigned int current_timer_interrupt_count = 0;

/* is this time sensitive enough that we have to worry? servo cycle length is 20ms
 * but duty cycle is typically 5 - 10 % = 1 - 2 ms
 * if we have a resolution of 180, this means time between interrupts ~5 us
 * the pi runs at 700 mhz => 700 instructions per microsecond. Yeah I think we're good
 * as the handler only takes 100 instructions max I think
 */
static bool pwm_output_handler(unsigned int pc){
  //check if we cause the interrupt:
  if(armtimer_check_and_clear_overflow()){
    //increment timer interrupt count
    current_timer_interrupt_count++;
    //if cycle is finished, wraparound
    if(current_timer_interrupt_count >= pwm_output_resolution){ 
      current_timer_interrupt_count = 0;
    }

    //for each pwm output
    for(int i = 0; i < number_of_pwm_outputs; i++){
      //write 0 if we're above the threshold
      if(pwm_output_thresholds[i] < current_timer_interrupt_count){
	gpio_write(pwm_output_pins[i], 0);
      }//otherwise, write 1, to make the duty cycle
      else{
	gpio_write(pwm_output_pins[i], 1);
      }
    }

    return true;
  }else{
    return false;
  }
}

//will calculate the time between interrupts, and then setup interrupts to trigger then
//Q: should I have checks for the parameters? also should I have default values?
void pwm_output_init(unsigned int interrupts_per_cycle, unsigned int cycle_length_in_us){
  //setup data structure
  //Q: function for this?
  pwm_output_pins = malloc(4 * number_of_pwm_outputs);
  pwm_output_thresholds = malloc(4 * number_of_pwm_outputs);

  pwm_output_resolution = interrupts_per_cycle;
  pwm_output_cycle_length_us = cycle_length_in_us;

  //calculate the time between each interrupt
  //hopefully cycle_length_in_us is a multiple of interrupts_per_cycle
  unsigned int time_between_pwm_output_interrupts = cycle_length_in_us / interrupts_per_cycle; 
  
  //initialize interrupts
  armtimer_init(time_between_pwm_output_interrupts);
  armtimer_enable_interrupts();
  interrupts_enable_basic(INTERRUPTS_BASIC_ARM_TIMER_IRQ);
  interrupts_attach_handler(pwm_output_handler);
  interrupts_global_enable();
}

int get_resolution(void){
  return pwm_output_resolution;
}

int get_number_pwm_outputs(void){
  return number_of_pwm_outputs;
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

int pwm_output_test(void){
  //add a servo on signal line 4, default duty cycle is 6%
  pwm_add_output(GPIO_PIN4, 2000 * 6 / 100);
  while(1){
    //set duty cycle to 6%
    pwm_change_duty_cycle(GPIO_PIN4, 2000 * 6 / 100);

    //wait
    timer_delay(1);
    
    //set duty cycle to 9 %
    pwm_change_duty_cycle(GPIO_PIN4, 2000 * 9 /100);    
    printf("%d", current_timer_interrupt_count);
    //wait
    timer_delay(1);
  }
}
