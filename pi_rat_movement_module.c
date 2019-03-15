#include "math.h"
#include "car_control_module.h"
#include "timer.h"
#include "printf.h"
#include "pi_rat_sensing_module.h"
#include "pi_rat_movement_module.h"
#include "ultrasonic_module.h"

/* information about the maze */
const unsigned int MAZE_WIDTH = 6;
const unsigned int MAZE_HEIGHT = 6;
const unsigned int MAZE_WALL_LENGTH_CM = 28;
const unsigned int CORRIDOR_END = 14; //this needs to be adjusted

/* keeps track of where the COR of the car within each block should be i.e. where
 * the Pi Rat should be within a block 
 */
const unsigned int CENTER_OF_ROTATION_IN_BLOCK_X_MM = 100; //is the middle of block side length
const unsigned int CENTER_OF_ROTATION_IN_BLOCK_Y_MM = 100; //measured in mm

/* keeps track of where ultrasonic sensor within the block is / should be */
const unsigned int ULTRASONIC_SENSOR_X_MM = 100;
const unsigned int ULTRASONIC_SENSOR_Y_MM = 100;

/* the angle between each line sensor.
 * calculated as follows: 1.2 cm between sensors, 7 cm from center of rotation to sensors
 * implies arcsin(1.2/7) = 9.8 degrees
 */
const unsigned int ANGLE_PER_SENSOR = 10;

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
unsigned int maze_bearing;

void pi_rat_movement_init(unsigned int input1, unsigned int input2, unsigned int output1, 
			  unsigned int output2){
  /* assumes pi_rat_sensing_module has already been initialized */
  car_control_module_init(input1, input2, output1, output2, 96, 197); 
  maze_x_cord = 0;
  maze_y_cord = 0;
  maze_bearing = 1; //start facing forwards.
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
  if(maze_bearing == 0){
    maze_x_cord++;
  }else if(maze_bearing == 1){
    maze_y_cord++;
  }else if(maze_bearing == 2){
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
  if(maze_bearing == 0){
    maze_x_cord--;
  }else if(maze_bearing == 1){
    maze_y_cord--;
  }else if(maze_bearing == 2){
    maze_x_cord++;
  }else{
    maze_y_cord++;
  }
}

void pi_rat_go_to_corridor_end(){
  int raw_distance_corridor_end = get_ultrasonic_mean(5); //this may not be the best way to get this...
  int corridor_distance_cm = raw_distance_corridor_end / 378; //covert to CM

  int distance_to_move = corridor_distance_cm - CORRIDOR_END;

  if(distance_to_move > 0){
    int start_line_position = pi_rat_line_position();
    move_forward(distance_to_move);
    int end_line_position = pi_rat_line_position();
    pi_rat_correct_line_position_f(start_line_position, end_line_position);
  }else{
    int start_line_position = pi_rat_line_position();
    move_forward(distance_to_move);
    int end_line_position = pi_rat_line_position();
    pi_rat_correct_line_position_b(start_line_position, end_line_position);
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
      shimmy_left();
      timer_delay_ms(100);
    }
  }else{
    for(int i = 0; i < -1*shimmy_number; i++){
      shimmy_right();
      timer_delay_ms(100);
    }
  } 
}

void pi_rat_correct_angle(int start_line, int end_line){
  int lateral_shift = end_line - start_line;
  double sin_of_turn_angle = (0.0 + lateral_shift) / MAZE_WALL_LENGTH_CM;

  int angle = (int) (asin(sin_of_turn_angle) * 180.0 / 3.14); //convert from radians to degrees 

  turn(angle);
  timer_delay_ms(100);
}

void pi_rat_turn_left(){
  //do the turn
  turn(-90);
  timer_delay_ms(100);

  //correct. Actually, I don't want this.
  //  pi_rat_correct_turn();

  //update the direction we're facing
  maze_bearing = (maze_bearing - 1) % 4; 
}

/* very similar to above function */
void pi_rat_turn_right(){
  turn(90);
  timer_delay_ms(100);

  //  pi_rat_correct_turn();

  maze_bearing = (maze_bearing + 1) % 4;
}

void pi_rat_correct_turn(){
  //ideally, the pi rat's line reader should be centered on the line. 
  int line_position = pi_rat_line_position();
  int correctional_rotation = (line_position - CENTER_LINE) * ANGLE_PER_SENSOR; //sign may be off
  turn(correctional_rotation);
}

int pi_rat_get_x_coord(){
  return maze_x_cord;
}

int pi_rat_get_y_coord(){
  return maze_y_cord;
}

int pi_rat_get_bearing(){
  return maze_bearing;
}

int pi_rat_get_maze_height(){
  return MAZE_HEIGHT;
}

int pi_rat_get_maze_width(){
  return MAZE_WIDTH;
}

int pi_rat_get_wall_length(){
  return MAZE_WALL_LENGTH_CM;
}

void test_pi_rat_movement(unsigned int input1, unsigned int input2, unsigned int output1, 
			  unsigned int output2, unsigned int trigger, unsigned int echo,
			  unsigned int pan){
  printf("testing pi rat movement \n");
  printf("you have 5 seconds to place the Pi on the line\n");

  pi_rat_movement_init(input1, input2, output1, output2);
  pi_rat_sensing_module_init(trigger, echo, pan);

  timer_delay(5);

  //line correction test
  /*
  while(1){
    int line_position = pi_rat_line_position();
    printf("line detected at %d \n", line_position);
    pi_rat_correct_lateral(line_position);
    timer_delay(3);
  }
  */

  /* test forwards/backwards line following */
  /*
  while(1){
    pi_rat_go_forward();
    timer_delay(1);
    pi_rat_go_back();
    timer_delay(1);
  }
  */

  /* turning and line following on turns */
  /*
  while(1){
    pi_rat_turn_left();
    timer_delay(1);
    pi_rat_turn_right();
    timer_delay(1);
  }
  */

  /* go in a square pattern */
  while(1){
    pi_rat_go_forward();
    timer_delay(1);
    pi_rat_turn_right();
    timer_delay(1);
  }
}
