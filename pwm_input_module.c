#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"
#include "printf.h"
#include "malloc.h"
#include "strings.h"
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

const unsigned int MAX_PWM_INPUTS = 8;

//helper to search data structure for index of pin, returns -1 if pin not in data structure
static int get_pin_index(unsigned int pin){
  for(int i = 0; i < number_of_pwm_inputs; i++){
    if(pin == pwm_input_pins[i]){
      return i;
    }
  }
  return -1;
}

/* the handler 
 * uses gpioextra to detect events
 * and to identify events, samples the input line
 * quickly after the event occurs (credit for idea goes to Pat)
 */
bool pwm_input_handler(){
  int did_we_trigger = 0;

  //go through each input pin and check / read the event
  for(int i = 0; i < number_of_pwm_inputs; i++){
    if(gpio_check_and_clear_event(pwm_input_pins[i])){
      did_we_trigger = 1;
      if(gpio_read(pwm_input_pins[i]) == 0){
	pwm_input_time_at_falling_edge[i] = timer_get_ticks();
      }else{
	pwm_input_time_at_previous_rising_edge[i] = pwm_input_time_at_rising_edge[i];
	pwm_input_time_at_rising_edge[i] = timer_get_ticks();
      }
    }
  }

  //if any events were detected
  if(did_we_trigger){
    return true;
  }else{
    return false;
  }
}

void pwm_input_init(){
  //enable interrupts
  interrupts_attach_handler(pwm_input_handler);
  interrupts_enable_source(INTERRUPTS_GPIO3);
  interrupts_global_enable();

  //initialize data structure
  pwm_input_pins = malloc(4 * (number_of_pwm_inputs));
  pwm_input_time_at_rising_edge = malloc(4 * (number_of_pwm_inputs));
  pwm_input_time_at_previous_rising_edge = malloc(4 * (number_of_pwm_inputs));
  pwm_input_time_at_falling_edge = malloc(4 * (number_of_pwm_inputs));
}

int pwm_add_input(unsigned int pin){
  if(number_of_pwm_inputs < MAX_PWM_INPUTS){
    //enable pin reading / interrupts
    gpio_set_pullup(pin);
    gpio_set_input(pin);
    gpio_enable_event_detection(pin, GPIO_DETECT_FALLING_EDGE);
    gpio_enable_event_detection(pin, GPIO_DETECT_RISING_EDGE);
    
    //make room in data structure
    pwm_input_pins = realloc(pwm_input_pins, 4 * (number_of_pwm_inputs + 1));
    pwm_input_time_at_rising_edge = realloc(pwm_input_time_at_rising_edge, 4* (number_of_pwm_inputs +1));
    pwm_input_time_at_previous_rising_edge = realloc(pwm_input_time_at_previous_rising_edge,
						     4 * (number_of_pwm_inputs +1));
    pwm_input_time_at_falling_edge = realloc(pwm_input_time_at_falling_edge, 
					     4 * (number_of_pwm_inputs +1));
    
    //initialize data
    pwm_input_pins[number_of_pwm_inputs] = pin;
    pwm_input_time_at_previous_rising_edge[number_of_pwm_inputs] = timer_get_ticks();
    
    //update size, so that interrupt handler knows to read new input
    number_of_pwm_inputs++;
    
    return 1;
  }else{
    return 0;
  }
}
 
/* again, I'm too lazy to write this */
int pwm_remove_input(unsigned int pin){
  return 0;
}

int pwm_input_get_threshold(unsigned int pin){
  int threshold = -1;
  int i = get_pin_index(pin);

  //wait till we get a good valued threshold
  while(threshold < 0 || threshold > 1100){
    threshold = pwm_input_time_at_falling_edge[i] - pwm_input_time_at_rising_edge[i];
  }

  return threshold;
}

/* this function is a fossil from my old implementation of this module that didn't work
 * which had a variable cycle length and resolution
 *
 * perhaps a better way to do this function is to measure an accurate cycle length at the 
 * beginning on program execution, then store that into a global variable
 * for later references, as the cycle length should stay constant
 */
int pwm_input_get_cycle_length(unsigned int pin){
  int cycle_length = -1;
  int i = get_pin_index(pin);
 
  //wait for a good piece of data
  while(cycle_length < 0){
    cycle_length = pwm_input_time_at_rising_edge[i] - pwm_input_time_at_previous_rising_edge[i];
  }

  return cycle_length;
}

int pwm_input_get_number_sources(){
  return number_of_pwm_inputs;
}

//this function needs to be improved. its really bad right now...
/* servo sends PWM feeback at 910 Hz
 * 2.7% duty cycle corresponds to 0 degrees, 97.1 corresponds to 360
 * Doing some math:
 * 1/910 = .0010989 s = 1099 us.
 * 2.7 % of this is 30 us
 * 97.1 % of this is 1066 us
 *
 * conversion rate is this (threshold - 30) * 360 / 1036
 *
 * a better way to do this would utilize pwm_input_get_cycle_length
 * to get more precise measurements on the cycle length to calculate this.
 */
unsigned int pwm_input_get_angle(unsigned int pin){
  //average 8 readings
  unsigned int angle_total = 0;
  for(int i = 0; i < 8; i++){
    int angle = (pwm_input_get_threshold(pin) - 30) * 360 / 1036;
    //wait till we get an approximately good reading
    while(angle < -10 || angle > 370){
      angle = (pwm_input_get_threshold(pin) - 30) * 360 / 1036;
    }
    //correct for some errors...
    if(angle < 0){
      angle = 0;
    }else if(angle > 360){
      angle = 360;
    }
    angle_total += angle;
  }
  return angle_total / 8;
}

int pwm_input_test(){
  printf("starting test \n");
  pwm_add_input(GPIO_PIN18);
  pwm_add_input(GPIO_PIN21);
  while(1){
    printf("angle 1: %d ", pwm_input_get_angle(GPIO_PIN18));
    printf("angle 2: %d, ", pwm_input_get_angle(GPIO_PIN21));
    printf("cycle length: %d\n", pwm_input_get_cycle_length(GPIO_PIN21));
  }
  return 1;
}
