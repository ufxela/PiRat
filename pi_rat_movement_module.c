#include "math.h"
#include "car_control_module.h"
#include "timer.h"
#include "printf.h"
#include "pi_rat_sensing_module.h"
#include "pi_rat_movement_module.h"

/* information about the maze */
const unsigned int MAZE_WIDTH = 6;
const unsigned int MAZE_HEIGHT = 6;
const unsigned int MAZE_WALL_LENGTH_CM = 20;

/* keeps track of where the COR of the car within each block should be i.e. where
 * the Pi Rat should be within a block 
 */
const unsigned int CENTER_OF_ROTATION_IN_BLOCK_X_MM = 100; //is the middle of block side length
const unsigned int CENTER_OF_ROTATION_IN_BLOCK_Y_MM = 100; //measured in mm

/* keeps track of where ultrasonic sensor within the block is / should be */
const unsigned int ULTRASONIC_SENSOR_X_MM = 100;
const unsigned int ULTRASONIC_SENSOR_Y_MM = 100;

/* where the center line is */
const unsigned int CENTER_LINE = 3;

/* information about Pi Rat's position within the maze. Should this module
 * keep track of this?
 *
 * (0,0) is the lower right corner, positive x is to the left, positive y is up
 */
unsigned int maze_x_cord;
unsigned int maze_y_cord;

/* a bearing of zero is the facing to the left
 * 1 = forwards
 * 2 = right
 * 3 = backwards
 */
unsigned int bearing;

void pi_rat_movement_init(unsigned int input1, unsigned int input2, unsigned int output1, 
			  unsigned int output2){
  /* assumes pi_rat_sensing_module has already been initialized */
  car_control_module_init(input1, input2, output1, output2, 96, 188); 
  maze_x_cord = 0;
  maze_y_cord = 0;
}

/* assumes our center of rotation is centered */
void pi_rat_go_forward(){
  //measure
  int start_line_position = pi_rat_line_position();

  //move
  move_forward(MAZE_WALL_LENGTH_CM);

  //measure
  int end_line_position = pi_rat_line_position();

  //correct
  pi_rat_correct_line_position_f(start_line_position, end_line_position);

  //update position
  if(bearing == 0){
    maze_x_cord++;
  }else if(bearing == 1){
    maze_y_cord++;
  }else if(bearing == 2){
    maze_x_cord--;
  }else{
    maze_y_cord--;
  }
}

/* very similar to go_forward() */
void pi_rat_go_back(){
  //measure                                                                                         
  int start_line_position = pi_rat_line_position();

  //move                                                                                           
  move_forward(-MAZE_WALL_LENGTH_CM);

  //measure                                                                                       
  int end_line_position = pi_rat_line_position();

  //correct (start/end reversed because going backwards flips things                           
  pi_rat_correct_line_position_b(start_line_position, end_line_position);

  //update position                                                                                     
  if(bearing == 0){
    maze_x_cord--;
  }else if(bearing == 1){
    maze_y_cord--;
  }else if(bearing == 2){
    maze_x_cord++;
  }else{
    maze_y_cord++;
  }
}

/* corrects assumming start_line is the previous point and end is the current point
 * basically, think of it as the forwards motion corrector.
 * with backwards motion, subtle differences.
 */
void pi_rat_correct_line_position_f(int start_line, int end_line){
  //correct angle
  pi_rat_correct_angle(start_line, end_line);

  //correct lateral, this breaks if we're going backwards, so a separate case is needed...
  pi_rat_correct_lateral(end_line);

  //double check shimmy distance (maybe)
  int new_line_position = pi_rat_line_position(); //ideally, this will be CENTER_LINE
  pi_rat_correct_lateral(new_line_position); 
}

void pi_rat_correct_line_position_b(int start_line, int end_line){
  pi_rat_correct_angle(end_line, start_line); //reversed, cuz angles and stuff

  pi_rat_correct_lateral(end_line); //not reversed

  int new_line_position = pi_rat_line_position();
  pi_rat_correct_lateral(new_line_position);
}

void pi_rat_correct_lateral(int current_line){
  int shimmy_number = current_line - CENTER_LINE;

  //correct based on shimmy_number reading                                                               
  if(shimmy_number > 0){
    for(int i = 0; i < shimmy_number; i++){
      shimmy_right();
      timer_delay(100);
    }
  }else{
    for(int i = 0; i < -1*shimmy_number; i++){
      shimmy_left();
      timer_delay(100);
    }
  } 
}

void pi_rat_correct_angle(int start_line, int end_line){
  int lateral_shift = end_line - start_line;
  double sin_of_turn_angle = lateral_shift / MAZE_WALL_LENGTH_CM;

  int angle = (int) asin(sin_of_turn_angle) * 180 / 3.14; //convert from radians to degrees      
  
  turn(angle);
  timer_delay(100);
}
