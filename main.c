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

const unsigned int ULTRASONIC_TRIGGER = GPIO_PIN23;
const unsigned int ULTRASONIC_ECHO = GPIO_PIN24;

void test_car_control_module(unsigned int input1, unsigned int input2, unsigned int output1, 
			     unsigned int output2){
  printf("Beginning car control test\n");
  car_control_module_init(input1, input2, output1, output2, 100, 340); //estimated wheelbase/circumfrence
  
  printf("move forward 10\n");
  move_forward(10);

  timer_delay(5);

  printf("wheel positions: %d, %d\n", get_wheel1_angle(), get_wheel2_angle());

  printf("move forward -10\n");
  move_forward(-10);

  printf("wheel positions: %d, %d\n", get_wheel1_angle(), get_wheel2_angle());

  printf("move forward_2, 10\n");
  move_forward_2(10);

  printf("wheel positions: %d, %d\n", get_wheel1_angle(), get_wheel2_angle());

  printf("move forward_2, -10\n");
  move_forward_2(-10);
}

void test_cr_servo_module(unsigned int pin){
  printf("Beginning cr servo module test\n");
  cr_servo_module_init();

  cr_servo * my_cr_servo = cr_servo_new(pin);

  cr_servo_setup(my_cr_servo);

  int throttle = 0;

  while(1){
    while(throttle < 100){
      cr_servo_go_to_throttle(my_cr_servo, throttle);
      throttle++;
      timer_delay_ms(75);
    }
    while(throttle > -100){
      cr_servo_go_to_throttle(my_cr_servo, throttle);
      throttle--;
      timer_delay_ms(75);
    }
  }
}

void test_servo_module(unsigned int pin){
  printf("Beginning servo module test \n");
  servo_module_init();
  servo * my_servo = servo_new(pin);

  servo_setup(my_servo);
  //servo_auto_setup(my_servo, 500, 2400); //the settings which I found to work
  int angle = 0;

  while(1){
    while(angle < 180){
      servo_go_to_angle(my_servo, angle);
      angle++;
      timer_delay_ms(50);
    }
    while(angle > 0){
      servo_go_to_angle(my_servo, angle);
      angle--;
      timer_delay_ms(50);
    }
  }
}
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

void main(void) 
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
  /* 
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
  
  test_car_control_module(GPIO_PIN23, GPIO_PIN24, GPIO_PIN20, GPIO_PIN21);
  
}
