#include "math.h"
#include "car_control_module.h"
#include "timer.h"
#include "printf.h"
#include "pi_rat_sensing_module.h"

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

/* information about Pi Rat's position within the maze. Should this module
 * keep track of this?
 *
 * (0,0) is the lower right corner, positive x is to the left, positive y is up
 */
unsigned int maze_x_coord;
unsigned int maze_y_coord;

/* a bearing of zero is the facing to the left
 * 1 = forwards
 * 2 = right
 * 3 = backwards
 */
unsigned int bearing;

void pi_rat_movement_init(){
  /* assumes pi_rat_sensing_module has already been initialized */
  car_control_module_init(input1, input2, output1, output2, 96, 188); 
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
  pi_rat_correct_line_position(start_line_position, end_line_position);

  //update position
  if(bearing == 0){
    maze_x_coord++;
  }else if(bearing == 1){
    maze_y_cord++;
  }else if(bearing == 2){
    maze_x_cord--;
  }else{
    maze_y_cord--;
  }
}
