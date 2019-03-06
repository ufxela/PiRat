#include "printf.h"
#include "uart.h"
#include "ultrasonic_module.h"
#include "timer.h"
#include "gpio.h"

const unsigned int ULTRASONIC_TRIGGER = GPIO_PIN23;
const unsigned int ULTRASONIC_ECHO = GPIO_PIN24;

void main(void) 
{
  uart_init();
  printf("Hello, world!\n");
  
  /* test the ultrasonic module */
  timer_init();
  ultrasonic_init(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
  ultrasonic_test();
}
