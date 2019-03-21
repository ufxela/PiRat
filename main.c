/* this is the primary program of this repository
 * which is the command central, and everything is being called from here
 * includes a lot of test programs to verify functionality of modules
 */

#include "printf.h"
#include "uart.h"
#include "ultrasonic_module.h"
#include "timer.h"
#include "gpio.h"
#include "pwm_output_module.h"
#include "pwm_input_module.h"
#include "servo_module.h"
#include "cr_servo_module.h"
#include "car_control_module.h"
#include "line_follower_module.h"
#include "pi_rat_sensing_module.h"
#include "pi_rat_movement_module.h"
#include "pi_rat_control.h"
#include "gpioextra.h"
#include "interrupts.h"

const unsigned int ULTRASONIC_TRIGGER = GPIO_PIN6;
const unsigned int ULTRASONIC_ECHO = GPIO_PIN5;
const unsigned int RESET_BUTTON = GPIO_PIN13;
const unsigned int START_X = 0;
const unsigned int START_Y = 0;
const unsigned int START_BEARING;
const unsigned int END_X = 5; //invalid so that maze will search for any opening
const unsigned int END_Y = 5;
const unsigned int MAZE_DIMENSION_X = 4;
const unsigned int MAZE_DIMENSION_Y = 4;
const unsigned int IN_1 = GPIO_PIN18; //first motor input pin
const unsigned int IN_2 = GPIO_PIN21;
const unsigned int OUT_1 = GPIO_PIN20;
const unsigned int OUT_2 = GPIO_PIN16;
const unsigned int PAN = GPIO_PIN12;

/* if button is pressed, then wait for ten seconds, and then start solving the maze again */
static bool button_press(unsigned int pc)
{
  if(gpio_check_and_clear_event(RESET_BUTTON)){
    printf("10 seconds until we start again");
    timer_delay(10);
    printf("solving maze now");
    pi_rat_reset();
    return true;
  }else{
    return false;
  }
}

void button_init(void)
{
  gpio_init();
  gpio_set_input(RESET_BUTTON);
  gpio_set_pullup(RESET_BUTTON);
  gpio_enable_event_detection(RESET_BUTTON, GPIO_DETECT_FALLING_EDGE);
  interrupts_attach_handler(button_press);
  interrupts_enable_source(INTERRUPTS_GPIO3);
  interrupts_global_enable();
}

int main(void)
{
  uart_init();
  timer_init();
  button_init();
  /* test the maze solver on a 4x4 maze */

  printf("beginning maze solving in 7 seconds \n");
  timer_delay(7);
  pi_rat_init(IN_1, IN_2, OUT_1, OUT_2, ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, PAN);
  while(1){
    pi_rat_solve_maze(START_X, START_Y, START_BEARING, END_X, END_Y, MAZE_DIMENSION_X, MAZE_DIMENSION_Y);
  }
  return 1;
}
