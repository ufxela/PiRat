#include "printf.h"
#include "uart.h"
#include "ultrasonic_module.h"

void main(void) 
{
  uart_init();
  printf("Hello, world!\n");
  ultrasonic_test();
  //pat_main();
}
