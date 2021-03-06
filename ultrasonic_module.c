#include "gpio.h"
#include "gpioextra.h"
#include "interrupts.h"
#include "timer.h"
#include "printf.h"
#include "uart.h"

/* defult values */
unsigned int TRIGGER = GPIO_PIN23;
unsigned int ECHO = GPIO_PIN24;

void ultrasonic_init(unsigned int trig_pin, unsigned int echo_pin){
  TRIGGER = trig_pin;
  ECHO = echo_pin;
  gpio_set_output(TRIGGER);

  gpio_set_input(ECHO);
  gpio_set_pulldown(ECHO);
}

/* a simple test function which gets the time of flight, doesn't use interrupts */
unsigned int get_time_of_flight_basic(){
  //send trigger & record time
  gpio_write(TRIGGER, 1);
  timer_delay_us(10);
  gpio_write(TRIGGER, 0);
  unsigned int trigger_time_us = timer_get_ticks();

  /*this line is borrowed from Pat's code*/
  timer_delay_us(149); // wait till device settles: 149 = time to go one inch
  
  //wait for echo to be received
  while(gpio_read(ECHO) == 0){
    if(timer_get_ticks() - trigger_time_us > 10000){ //if there's a timeout
      return 10000; //return this value if we get anything higher than it. 
    }
  }

  while(gpio_read(ECHO) == 1){
    if(timer_get_ticks() - trigger_time_us > 10000){
      return 10000;
    }
  }

  //record time at echo
  unsigned int echo_time_us = timer_get_ticks();

  //if converting time into inches, do (echo_time_us - trigger_time_us)/149;
  return (echo_time_us - trigger_time_us);
}

unsigned int get_ultrasonic_mean(int number_samples){
  unsigned int total = 0;
  for(int i = 0; i < number_samples; i++){
    int basic_reading = get_time_of_flight_basic();
    /* loop until we get a valid reading */
    /* this could be bad because in the case the ultrasonic module is covered or points too far
     * this will hold here indefinitely 
     */
    while(basic_reading > 10000){
      basic_reading = get_time_of_flight_basic();
    }
    total += basic_reading;
    timer_delay_us(1000);
  }
  return total / number_samples;
}

int ultrasonic_test(void){
  while(1){
    printf("time of flight in us: %d \n", get_ultrasonic_mean(1));
  }
  return 0;
}

