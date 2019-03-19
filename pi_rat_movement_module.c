#include "math.h"
#include "car_control_module.h"
#include "timer.h"
#include "printf.h"
#include "pi_rat_sensing_module.h"
#include "pi_rat_movement_module.h"
#include "ultrasonic_module.h"

/* information about the maze */
unsigned int MAZE_WIDTH = 6;
unsigned int MAZE_HEIGHT = 6;
const unsigned int MAZE_WALL_LENGTH_CM = 28;

/* the angle between each line sensor.
 * calculated as follows: 1.2 cm between sensors, 7 cm from center of rotation to sensors
 * implies arcsin(1.2/7) = 9.8 degrees
 */
const unsigned int ANGLE_PER_SENSOR = 12;

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
 * 1 = forwards (up)
 * 2 = right
 * 3 = backwards (down)
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
/* breaks down into multiple steps to ensure accuracy (hopefully) */
void pi_rat_go_forward(){
  //measure
  int start_line_position = pi_rat_line_position();

  //move
  move_forward(MAZE_WALL_LENGTH_CM/2);

  //measure
  int end_line_position = pi_rat_line_position();

  //correct
  pi_rat_correct_line_position_f(start_line_position, end_line_position);

  //repeat
  start_line_position = pi_rat_line_position();
  move_forward(MAZE_WALL_LENGTH_CM/2);
  end_line_position = pi_rat_line_position();
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
  move_forward(-MAZE_WALL_LENGTH_CM/2);

  //measure                                                                                       
  int end_line_position = pi_rat_line_position();

  //correct (start/end reversed because going backwards flips things                           
  pi_rat_correct_line_position_b(start_line_position, end_line_position);

  //repeat
  start_line_position = pi_rat_line_position();
  move_forward(-MAZE_WALL_LENGTH_CM/2);
  pi_rat_line_position();
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

/* this function should ideally make one movement at a time, then recheck the line position
 * and then make another movement and repeat until the line is at the correct position 
 */
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

//correct two times per forward movement
void pi_rat_correct_angle(int start_line, int end_line){
  int lateral_shift = end_line - start_line;
  double sin_of_turn_angle = (0.0 + lateral_shift) * 2 / MAZE_WALL_LENGTH_CM; //every half step, correct 

  int angle = (int) (asin(sin_of_turn_angle) * 180.0 / 3.14); //convert from radians to degrees 

  turn(angle);
  timer_delay_ms(100);
}

void pi_rat_turn_left(){
  //do the turn
  turn(93); //trimmed, to make sure turns enough
  timer_delay_ms(100);

  //correct. Actually, I don't want this.
  pi_rat_correct_turn();

  //update the direction we're facing
  maze_bearing = (maze_bearing - 1) % 4; 
}

/* very similar to above function */
void pi_rat_turn_right(){
  turn(-93); //isn't turning enough, so added that trim
  timer_delay_ms(100);
  pi_rat_correct_turn();
  maze_bearing = (maze_bearing + 1) % 4;
}

void pi_rat_correct_turn(){
  //ideally, the pi rat's line reader should be centered on the line. 
  int line_position = pi_rat_line_position();
  pi_rat_correct_lateral(line_position);
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

void pi_rat_position_change(int direction){
  int valid_change = 1;
  /* first, make the bearing correct */
  while(pi_rat_get_bearing() != direction){
    pi_rat_turn_right(); //may want to make this smarter
  }
    
  /* then, go forward */
  if(valid_change){
    pi_rat_go_forward();
  }
}

void maze_set_width(int width){
  MAZE_WIDTH = width;
}

void maze_set_height(int height){
  MAZE_HEIGHT = height;
}

void maze_set_bearing(int bearing){
  maze_bearing = bearing;
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
