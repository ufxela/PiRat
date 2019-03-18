#include "pi_rat_control.h"
#include "timer.h"
#include "pi_rat_movement_module.h"
#include "pi_rat_sensing_module.h"

struct maze_node{
  int visited; //0 for no 1 for yes
  int left; //only makes sense if visited is 1. 1 for wall, 0 for no wall
  int up;
  int right;
  int down;
};

/* needs some internal data structure to keep track of the Pi Rat's path */
/* path stores as an array the moves made. 0 is go left, 1 is go forwards, 2 go right, 3 go back */
int * path;
int path_length;

/* needs another internal data structure to keep track of the known elements of the maze 
 * 
 * Should be a 2d array of ints, with 1 representing a wall, 0 representing no wall
 * and -1 representing unknown. 
 */

/* 2D array of maze_nodes. Very similar to frame buffer 2d array */
struct maze_node** maze;

int x_final;
int y_final;
int x_curr;
int y_curr;

void pi_rat_init(unsigned int input1, unsigned int input2, unsigned int output1,
		 unsigned int output2, unsigned int trigger, unsigned int echo,
		 unsigned int pan){
  pi_rat_movement_init(input1, input2, output1, output2);
  pi_rat_sensing_module_init(trigger, echo, pan);
  timer_delay_ms(100);
}

/* gets surroundings and updates maze data structure */
static void update_maze(){

}

static void recursive_maze_solver(){
  if(x_curr == x_final && y_curr == y_final){
    /* base case: we've solved the maze */
  }else{ /* recursive case: explore, backtracking */
    update_maze();
    

    /* get all possible moves */
    
    /* pick one possible move and perform it 
     * This means both physically making the movement as well as recording it into the path
     */
    
    /* unpick that move by reversing it as well as deleting it from the path */    
  }
  return;
}

void pi_rat_solve_maze(int x_start, int y_start, int bearing, int x_end, int y_end, 
		       int maze_width, int maze_height){
  /* initialize everything */

  /* call recursive helper */
  recursive_maze_solver();

  /* at end, traverse maze solution back and forth */
}

/* just wanders around the maze, without solving it yet, epsilon steps */
void pi_rat_maze_wander(){

}

void pi_rat_wander(int x_start, int y_start, int x_end, int y_end){
  while(x_start != x_end || y_start != y_end){
    int walls = pi_rat_get_walls();
    if(!(walls & 0b1)){
      pi_rat_turn_left();
    }else if(!(walls & 0b10)){
      /*do nothing, we're already facing forward */
    }else if(!(walls & 0b100)){
      pi_rat_turn_right();
    }else{ //no path, need to backtrack, turn 180
      pi_rat_turn_right();
      pi_rat_turn_right();
    }
    timer_delay_ms(100);
    //execute the forward movement. 
    pi_rat_go_forward();
    timer_delay_ms(100);
    x_start = pi_rat_get_x_coord();
    y_start = pi_rat_get_y_coord();
  }
  
  //once done, spin to indicate success
  while(1){
    pi_rat_turn_right();
  }
}
