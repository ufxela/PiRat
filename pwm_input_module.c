#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"
#include "printf.h"
#include "malloc.h"

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

unsigned int INPUT_PIN = GPIO_PIN26;

volatile unsigned int time_at_rising_edge = 0;
volatile unsigned int time_at_previous_rising_edge = 0;
volatile unsigned int time_at_falling_edge = 0;

//helper to search data structure for index of pin, returns -1 if pin not in data structure
static int get_pin_index(unsigned int pin){
  for(int i = 0; i < number_of_pwm_inputs; i++){
    if(pin == pwm_input_pins[i]){
      return i;
    }
  }
  return -1;
}

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
  //enable pin reading / interrupts
  gpio_set_pullup(pin);
  gpio_set_input(pin);
  gpio_enable_event_detection(pin, GPIO_DETECT_FALLING_EDGE);
  gpio_enable_event_detection(pin, GPIO_DETECT_RISING_EDGE);

  //make room in data structure
  pwm_input_pins = realloc(pwm_input_pins, 4 * (number_of_pwm_inputs + 1));
  pwm_input_time_at_rising_edge = realloc(pwm_input_time_at_rising_edge, 4 * (number_of_pwm_inputs +1));
  pwm_input_time_at_previous_rising_edge = realloc(pwm_input_time_at_previous_rising_edge,
						   4 * (number_of_pwm_inputs +1));
  pwm_input_time_at_falling_edge = realloc(pwm_input_time_at_falling_edge, 
					   4 * (number_of_pwm_inputs +1));
  
  //initialize data
  pwm_input_pins[number_of_pwm_inputs] = pin;
  pwm_input_time_at_previous_rising_edge[number_of_pwm_inputs] = timer_get_ticks();

  //update size, so that interrupt handler knows to read new input
  number_of_pwm_inputs++;
}
int pwm_remove_input(unsigned int pin);

int pwm_input_get_threshold(unsigned int pin){
  //wait till you get a vaild data point
  int threshold = -1;
  int i = get_pin_index(pin);
  //wait till we get a good valued threshold
  while(threshold < 0 || threshold > 1100){
    threshold = pwm_input_time_at_falling_edge[i] - pwm_input_time_at_rising_edge[i];
  }
  return threshold;
}

int pwm_input_get_cycle_length(unsigned int pin){
  //wait for valid data
  int cycle_length = -1;
  int i = get_pin_index(pin);
  while(cycle_length < 0){
    cycle_length = pwm_input_time_at_rising_edge[i] - pwm_input_time_at_previous_rising_edge[i];
  }
  return cycle_length;
}

int pwm_input_get_number_sources();

int pwm_input_test(){
  printf("starting test \n");
  pwm_add_input(GPIO_PIN26);
  pwm_add_input(GPIO_PIN16);
  while(1){
    printf("angle 1: %d ", (pwm_input_get_threshold(GPIO_PIN26)-30) * 360 / 1060);
    printf("angle 2: %d\n", (pwm_input_get_threshold(GPIO_PIN16)-30) * 360 / 1060);
    printf("cycle length: %d\n", pwm_input_get_cycle_length(GPIO_PIN26));
  }
  return 1;
}
