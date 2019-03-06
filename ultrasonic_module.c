#include "gpio.h"
#include "gpioextra.h"
#include "interrupts.h"
#include "timer.h"
#include "printf.h"

void ultrasonic_init(unsigned int trig_pin, unsigned int echo_pin){
  gpio_set_output(trig_pin);

  gpio_set_input(echo_pin);
  gpio_set_pullup(echo_pin);
  gpio_enable_event_detection(echo_pin, GPIO_DETECT_RISING_EDGE);
}

/* a simple test function which gets the time of flight, doesn't use interrupts */
int get_time_of_flight_basic(unsigned int trig_pin, unsigned int echo_pin){
  timer_init();

  ultrasonic_init(trig_pin, echo_pin);

  //send trigger & record time
  gpio_write(trig_pin, 1);
  timer_delay_us(10);
  gpio_write(trig_pin, 0);
  unsigned int trigger_time_us = timer_get_ticks();
  
  //record time when echo is recieved
  while(gpio_read(echo_pin) == 0){
    /* spin */
  }
  return timer_get_ticks() - trigger_time_us;
}

int ultrasonic_test(void){
  while(1){
    printf("time of flight in us: %d \n", get_time_of_flight_basic(GPIO_PIN23, GPIO_PIN24));
    timer_delay(1);
  }

  return 0;
}
