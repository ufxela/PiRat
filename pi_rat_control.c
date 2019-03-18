#include "pi_rat_control.h"

struct maze_node{
  int visited; //0 for no 1 for yes
  int left; //only makes sense if visited is 1. 1 for wall, 0 for no wall
  int up;
  int right;
  int down;
}

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
maze_node** maze[][];

int x_final;
int y_final;
int x_curr;
int y_curr;

void pi_rat_init(){
  
}

void pi_rat_solve_maze(int x_start, int y_start, int bearing, int x_end, int y_end, 
		       int maze_width, int maze_height){
  /* initialize everything */

  /* call recursive helper */

  /* at end, traverse maze solution back and forth */
}

static void recursive_maze_solver(){
  if(x_curr == x_final && y_curr = y_final){
    /* base case: we've solved the maze */
  }else{ /* recursive case: explore, backtracking */
    update_maze();
    

    /* get all possible moves */
    
    /* pick one possible move and perform it 
     * This means both physically making the movement as well as recording it into the path
     */
    
    /* unpick that move by reversing it as well as deleting it from the path */


    
  }
}

/* gets surroundings and updates maze data structure */
static void update_maze(){


/* just wanders around the maze, without solving it yet, epsilon steps */
void pi_rat_maze_wander(){

}
