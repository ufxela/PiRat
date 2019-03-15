#include "line_follower_module.h"
#include "ultrasonic_module.h"
#include "servo_module.h"
#include "timer.h"
#include "car_control_module.h"
#include "printf.h"
/* 20cm to inches = 7.4 inches * 149 us/inch = 1173. I'll give it a bit of extra room as well */
const unsigned int WALL_THRESHOLD_US = 1500; //threshold to determine if there is a wall or not. 
const unsigned int NUM_LINE_READINGS = 3;

servo * ultrasonic_pan;

void pi_rat_sensing_module_init(unsigned int trigger_pin, unsigned int echo_pin, 
				unsigned int servo_pin){
  ultrasonic_init(trigger_pin, echo_pin);
  line_follower_init();
  servo_module_init();
  
  ultrasonic_pan = servo_new(servo_pin);
  servo_auto_setup(ultrasonic_pan, 500, 2300); //I found these values beforehand
  servo_go_to_angle(ultrasonic_pan, 90);
}

static int is_there_a_wall(){
  //samples 10 times. if a significant number of samples say there is a wall, then there's a wall
  int wall_yes_count = 0;
  for(int i = 0; i < 10; i++){
    if(get_ultrasonic_mean(1) <= WALL_THRESHOLD_US){
      wall_yes_count++;
    }
  }
  if(wall_yes_count >= 7){
    return 1;
  }else{
    return 0;
  }
}

int pi_rat_get_walls(){
  int walls_result = 0;
  
  //check each of the three walls
  servo_go_to_angle(ultrasonic_pan, 0);
  timer_delay_ms(500);
  printf("1");
  if(is_there_a_wall()){
    walls_result += 1;
  }
  printf("2");
  servo_go_to_angle(ultrasonic_pan, 90);
  timer_delay_ms(500);
  printf("3");
  if(is_there_a_wall()){
    walls_result += 2;
  }
  printf("4");
  servo_go_to_angle(ultrasonic_pan, 180);
  timer_delay_ms(500);
  if(is_there_a_wall()){
    walls_result += 4;
  }
  
  return walls_result;
}

int pi_rat_line_position(){
  int in_a_row = 0;
  
  int line_pos = -1;
  //make sure we get 5 of the same reading in a row for accuracy.
  //this is bad because what if we're right in the middle. Will loop infinitely.
  //A better practice is to use the mode as the measure of center
  while(in_a_row < NUM_LINE_READINGS){
    //if we're at an invalid line position
    if(line_pos == -1){
      line_pos = get_line_index();
      in_a_row = 1;
    }else if(get_line_index() == line_pos){ //good value, increment in a row
      in_a_row++;
    }else{ //inconsistent reading; reset
      line_pos = -1;
    }
  }
  return line_pos;
}

int pi_rat_get_wheel_base_mm(){
  return get_wheel_base();
}

int pi_rat_get_wheel_circumference_mm(){
  return get_wheel_circumference(); 
}

int pi_rat_get_line_sensor_dist_mm(){
  return get_distance_between_sensors();
}

void pi_rat_sensing_test(unsigned int trigger_pin, unsigned int echo_pin, unsigned int servo_pin){
  pi_rat_sensing_module_init(trigger_pin, echo_pin, servo_pin);
  while(1){
    printf("Line position: %d ", pi_rat_line_position());
    int walls = pi_rat_get_walls();
    printf("walls: %d", walls);
  }
}
