#include "printf.h"
#include "uart.h"
#include "ultrasonic_module.h"
#include "timer.h"
#include "gpio.h"
#include "pwm_output_module.h"

const unsigned int ULTRASONIC_TRIGGER = GPIO_PIN23;
const unsigned int ULTRASONIC_ECHO = GPIO_PIN24;

void main(void) 
{
  uart_init();
  printf("Hello, world!\n");
  
  /* test the ultrasonic module */
  /*
  timer_init();
  ultrasonic_init(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
  ultrasonic_test();
  */

  /* test the pwm output module */
  timer_init();
  //servos have cycle length 20 ms, with a 5% range of that being from 0 to 180 degrees
  //(duty cycle of 5% = 0, 10% = 180. So I'm alloting 100 steps in the workable range)  
  pwm_output_init(2000, 20000);
  pwm_output_test(2000, 20000);
}
