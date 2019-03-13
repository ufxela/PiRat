#include "line_follower_module.h"
#include "ultrasonic_module.h"
#include "servo_module.h"

/* 20cm to inches = 7.4 inches * 149 us/inch = 1173 */
const unsigned int WALL_THRESHOLD_US = 1173; //threshold to determine if there is a wall or not. 
const unsigned int NUM_LINE_READINGS = 3;
void pi_rat_sensing_module_init(unsigned int trigger_pin, unsigned int echo_pin, 
				unsigned int servo_pin){
  ultrasonic_init(trigger_pin, echo_pin);
  line_follower_init();
  servo_module_init();
  
  servo * ultrasonic_pan = servo_new(servo_pin);
  servo_auto_setup(ultrasonic_pan, 500, 2300); //I found these values beforehand
  servo_go_to_angle(ultrasonic_pan, 90);
}

static int is_there_a_wall(){
  //if the distance is less than / equal to the wall threshold, return 1 for true
  if(get_ultrasonic_mean(10) <= WALL_THRESHOLD_US){
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
  if(is_there_a_wall()){
    walls_result += 1;
  }
  servo_go_to_angle(ultrasonic_pan, 90);
  timer_delay_ms(500);
  if(is_there_a_wall()){
    walls_result += 2;
  }
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
  //make sure we get 5 of the same reading in a row for accuracy
  while(in_a_row < NUM_LINE_READINGS){
    //if we're at an invalid line position
    if(line_pos = -1){
      line_pos = get_line_index();
      in_a_row = 1;
    }else if(get_line_index = line_pos){ //good value, increment in a row
      in_a_row++;
    }else{ //inconsistent reading; reset
      line_pos = -1;
    }
    }
  }
}

/* probably want getter functions for physical data about Pi, like distance between line sensors,
 * wheelbase, etc. 
 */
