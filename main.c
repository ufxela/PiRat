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

const unsigned int ULTRASONIC_TRIGGER = GPIO_PIN23;
const unsigned int ULTRASONIC_ECHO = GPIO_PIN24;

/* test function which combines pwm_input and pwm_output modules */
void test_pwm_IO(unsigned int input_pin1, unsigned int input_pin2, unsigned int output_pin1,
		 unsigned int output_pin2){
  printf("Beginning PWM IO Test \n");
  pwm_output_init();
  pwm_input_init();
  
  //add pwm outputs
  pwm_add_output(output_pin1, 1500);
  pwm_add_output(output_pin2, 1500);
  pwm_add_input(input_pin1);
  pwm_add_input(input_pin2);
  
  while(1){
    printf("new loop \n");
    pwm_change_threshold(output_pin1, 1400);
    pwm_change_threshold(output_pin2, 1450);
    printf("hello");
    for(int i = 0; i < 10; i++){
      timer_delay_ms(100);
      printf("hello");
      printf("Output 1 angle: %d, Output 2 angle %d \n", pwm_input_get_angle(input_pin1), 
	     pwm_input_get_angle(input_pin2));
    }
    pwm_change_threshold(output_pin1, 1600);
    pwm_change_threshold(output_pin2, 1550);
    for(int i =0; i < 10; i++){
      timer_delay_ms(100);
      printf("Output 1 angle: %d, Output 2 angle %d \n", (pwm_input_get_threshold(input_pin1)-30) * 360
             / 1060, (pwm_input_get_threshold(input_pin2)-30) * 360 / 1060);
    }
  }
}

/* a second test function for pwm_input and pwm_outputs */
void test_pwm_IO2(unsigned int input_pin, unsigned int output_pin){
  printf("Beginning PWM IO Test 2 \n");
  pwm_output_init();
  pwm_input_init();
  
  pwm_add_output(output_pin, 1500);
  pwm_add_input(input_pin);
  
  int angle = 0;
  while(1){
    if(angle >= 360){
      angle = 0;
    }
    
    printf("going to angle %d", angle);
    
    pwm_change_threshold(output_pin, 1700);
    while(((pwm_input_get_threshold(input_pin)-30) * 360 / 1060) > angle + 5 ||
	  ((pwm_input_get_threshold(input_pin)-30) * 360 / 1060) < angle - 5){
    }
    pwm_change_threshold(output_pin, 1500);
    
    printf("current angle: %d\n", (pwm_input_get_threshold(input_pin)-30) * 360 / 1060);
    timer_delay(2);
    angle += 30;
  }
}

int main(void) 
{
  uart_init();
  timer_init();
  
  /* test the ultrasonic module */
  /*
  timer_init();
  ultrasonic_init(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
  ultrasonic_test();
  */

  /* test the pwm output module */
  /*pwm_output_init();
  pwm_output_test();
  */
  
  /* test pwm input module (works) */
  /* 
  pwm_input_init();
  pwm_input_test();
  */
  
  /* test PWM IO */
  /* 
  test_pwm_IO(GPIO_PIN23, GPIO_PIN24, GPIO_PIN20, GPIO_PIN21); 
  test_pwm_IO2(GPIO_PIN27, GPIO_PIN18);  
  */
  
  /* test servo module */
  /*
  test_servo_module(GPIO_PIN4);
  */
  
  /*test cr servo module */
  /*  
  test_cr_servo_module(GPIO_PIN20);
  */
  
  /*test car control module */
  /*  
  test_car_control_module(GPIO_PIN23, GPIO_PIN24, GPIO_PIN20, GPIO_PIN21);
  */

  /* test line follower */

  line_follower_test();

  return 1;
}
