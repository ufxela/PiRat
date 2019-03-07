#include "gpio.h"
#include "gpioextra.h"
#include "timer.h"
#include "interrupts.h"
#include "printf.h"

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

volatile unsigned int rising_edge_count = 0;
volatile unsigned int falling_edge_count = 0;

volatile unsigned int time_at_rising_edge = 0;
volatile unsigned int time_at_previous_rising_edge = 0;
volatile unsigned int time_at_falling_edge = 0;

/* for now just check one input */
bool pwm_input_handler(){
  if(gpio_check_and_clear_event(INPUT_PIN)){
    if(gpio_read(INPUT_PIN) == 0){
      falling_edge_count++;
      time_at_falling_edge = timer_get_ticks();
    }else{
      rising_edge_count++;
      time_at_previous_rising_edge = time_at_rising_edge;
      time_at_rising_edge = timer_get_ticks();
    }
    return true;
  }    
  else{
    return false;
  }
}

void pwm_input_init(){
  gpio_set_input(INPUT_PIN);
  gpio_set_pullup(INPUT_PIN);
  gpio_enable_event_detection(INPUT_PIN, GPIO_DETECT_FALLING_EDGE);
  gpio_enable_event_detection(INPUT_PIN, GPIO_DETECT_RISING_EDGE);
  interrupts_attach_handler(pwm_input_handler);
  interrupts_enable_source(INTERRUPTS_GPIO3);
  interrupts_global_enable();
}

int pwm_input_add_source(unsigned int pin);

int pwm_input_remove_source(unsigned int pin);

int pwm_input_read(unsigned int pin);

int pwm_input_get_cycle_length(unsigned int pin);

int pwm_input_get_number_sources();

int pwm_input_test(){
  int cur_rising_edge_cnt = 0;
  int cur_falling_edge_cnt = 0;
  printf("starting test \n");
  while(1){
    if(cur_falling_edge_cnt != falling_edge_count || cur_rising_edge_cnt != rising_edge_count){
      cur_falling_edge_cnt = falling_edge_count;
      cur_rising_edge_cnt = rising_edge_count;
      printf("RE cnt %d; FE cnt: %d \n", cur_falling_edge_cnt, cur_rising_edge_cnt);
    }
  }
  return 1;
}
