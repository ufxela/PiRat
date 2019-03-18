#include "pi_rat_control.h"

/* needs some internal data structure to keep track of the Pi Rat's path */

/* needs another internal data structure to keep track of the known elements of the maze 
 * 
 * Should be a 2d array of ints, with 1 representing a wall, 0 representing no wall
 * and -1 representing unknown. 
 */

void pi_rat_init(){
  
}

void pi_rat_solve_maze(int x_start, int y_start, int bearing, int x_end, int y_end, 
		       int maze_width, int maze_height){
  /* initialize everything */

  /* call recursive helper */

  /* at end, traverse maze solution back and forth */
}

static void recursive_maze_solver(){
  /* base case: we're at x_end, y_end. Then stop. */ 

  /* get all possible moves */

  /* pick one possible move and perform it 
   * This means both physically making the movement as well as recording it into the path
   */

  /* unpick that move by reversing it as well as deleting it from the path */

  /* repeat for all possible moves */
}

/* just wanders around the maze, without solving it yet, epsilon steps */
void pi_rat_maze_wander(){

}
