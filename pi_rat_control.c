#include "pi_rat_control.h"
#include "timer.h"
#include "pi_rat_movement_module.h"
#include "pi_rat_sensing_module.h"
#include "malloc.h"
#include "strings.h"
#include "printf.h"

/* represents a node in the graph representing the maze */
typedef struct maze_node{
  int visited; //0 for no 1 for yes
  int explored; //0 for no, 1 for yes, useful for backtracking
  int left; //only makes sense if visited is 1. 1 for wall, 0 for no wall
  int up;
  int right;
  int down;
} Maze_Node;

/* path stores as an array the moves made. 0 is go left, 1 is go forwards, 2 go right, 3 go back */
int * path;
int path_length;

/* 2D array of maze_nodes. Very similar to frame buffer 2d array */
void * maze;
int maze_square_dimension;

int x_final;
int y_final;
int x_curr;
int y_curr;

/* a helper function / debugging helper function which prints out the maze to
 * make the program more transparent
 */
static void print_maze(){
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      Maze_Node * current_node = (Maze_Node *) ((int *)maze) +
	sizeof(Maze_Node)*(i*maze_square_dimension + j);
      printf("Node %d%d pointer %p visited: %d explored: %d", i, j, current_node,
	     current_node->visited, current_node->explored);
      printf("walls: left %d up %d right %d down %d\n", current_node->left, current_node->up,
	     current_node->right, current_node->down);
    }
  }
}

/* for transparency */
static void print_path(){
  for(int i = 0; i < path_length; i++){
    printf("Step %d: %d", i, path[i]);
  }
  printf("\n");
}

void pi_rat_init(unsigned int input1, unsigned int input2, unsigned int output1,
		 unsigned int output2, unsigned int trigger, unsigned int echo,
		 unsigned int pan){
  pi_rat_movement_init(input1, input2, output1, output2);
  pi_rat_sensing_module_init(trigger, echo, pan);
  timer_delay_ms(100);
}

/* gets surroundings and updates maze data structure */
/* for some reason, my array indexing isn't working, so I will have to do manual
 * pointer arithmetic
 */
static void update_maze(){
  /* put maze into a workable form (a pointer to each row of the maze) */
  Maze_Node * current_node = (Maze_Node *)((int *)maze) +
    sizeof(Maze_Node)*(y_curr*maze_square_dimension + x_curr);

  /* if we haven't been here before */
  if(current_node->visited != 1){
    current_node->visited = 1;

    /* now update walls */
    int current_bearing = pi_rat_get_bearing();
    int walls = pi_rat_get_walls(); /* this is the time suck of the function, limited physically */

    /* this is super ugly */
    if(current_bearing == 0){
      current_node->right = 0;
      current_node->down = ((walls & 0b1) == 0b1);
      current_node->left = ((walls & 0b10) == 0b10);
      current_node ->up = ((walls & 0b100) == 0b100);
    }
    else if(current_bearing == 1){
      current_node->down = 0;
      current_node->left = ((walls & 0b1) == 0b1);
      current_node->up = ((walls & 0b10) == 0b10);
      current_node->right = ((walls & 0b100) == 0b100);
    }else if(current_bearing == 2){
      current_node->left = 0;
      current_node->up = ((walls & 0b1) == 0b1);
      current_node->right = ((walls & 0b10) == 0b10);
      current_node->down = ((walls & 0b100) == 0b100);
    }else{
      current_node->up = 0;
      current_node->right = ((walls & 0b1) == 0b1);
      current_node->down = ((walls & 0b10) == 0b10);
      current_node->left = ((walls & 0b100) == 0b100);
    }
  }
}

/* returns true (1) if maze path is found */
static int recursive_maze_solver(){
  /* update position */
  x_curr = pi_rat_get_x_cord();
  y_curr = pi_rat_get_y_cord();

  /* what do we do if the x or y coordinate go out of bounds? Just stop? In a properly
   * setup maze, this will never happen, so I will just not do anything
   */

  /* base case: we've solved the maze */
  if(x_curr == x_final && y_curr == y_final){
    return 1;
  }else{ /* recursive case: explore, backtracking */
    update_maze();

    /* get access to possible moves at current state */
    x_curr = pi_rat_get_x_cord();
    y_curr = pi_rat_get_y_cord();
    Maze_Node * current_node = (Maze_Node * )((int *)maze) +
      sizeof(Maze_Node)*(y_curr*maze_square_dimension + x_curr);

    /* mark the node as explored */
    current_node->explored = 1;

    /* pick one possible move and perform it
     * This means both physically making the movement as well as recording it into the path
     */
    if(current_node->left == 0){
      /* first check if we've explored the node before */
      x_curr = pi_rat_get_x_cord();
      y_curr = pi_rat_get_y_cord();
      Maze_Node * next_node = (Maze_Node * )((int *)maze) +
	sizeof(Maze_Node)*(y_curr*maze_square_dimension + (x_curr + 1)); //this pointer arithmetic could be dangerous, in the case that it goes out of the bounds of the maze dimesnion...

      if(next_node->explored != 1){
	/* execute move */
	pi_rat_position_change(0);

	/* add move to path */
	path[path_length] = 0;
	path_length++; //should this check if path is too big?

	/* recurse */
	if(recursive_maze_solver()){
	  return 1;
	}

	/* backtrack (undo move, remove move from path) */
	pi_rat_position_change(2); //0 + 2 (mod 4)
	path_length--;
      }
    }
    if(current_node->up == 0){
      x_curr = pi_rat_get_x_cord();
      y_curr = pi_rat_get_y_cord();
      Maze_Node * next_node = (Maze_Node * )((int *)maze) +
	sizeof(Maze_Node)*((y_curr+1)*maze_square_dimension + (x_curr));

      if(next_node->explored != 1){
	pi_rat_position_change(1);

	path[path_length] = 1;
	path_length++;

	if(recursive_maze_solver()){
	    return 1;
	}

	pi_rat_position_change(3);
	path_length--;
      }
    }

    if(current_node->right == 0){
      x_curr = pi_rat_get_x_cord();
      y_curr = pi_rat_get_y_cord();
      Maze_Node * next_node = (Maze_Node * )((int *)maze) +
	sizeof(Maze_Node)*(y_curr*maze_square_dimension + (x_curr - 1));

      if(next_node->explored != 1){

	pi_rat_position_change(2);

	path[path_length] = 2;
	path_length++;

	if(recursive_maze_solver()){
	  return 1;
	}

	pi_rat_position_change(0);
	path_length--;
      }
    }
    if(current_node->down == 0){
      x_curr = pi_rat_get_x_cord();
      y_curr = pi_rat_get_y_cord();
      Maze_Node * next_node = (Maze_Node * )((int *)maze) +
	sizeof(Maze_Node)*((y_curr-1)*maze_square_dimension + (x_curr));

      if(next_node->explored != 1){
	pi_rat_position_change(3);

	path[path_length] = 3;
	path_length++;

	if(recursive_maze_solver()){
	  return 1;
	}

	pi_rat_position_change(1);
	path_length--;
      }
    }
    return 0;
  }
  return 0; //just in case....
}

/* does nothing for now */
static void traverse_path(){
}

void pi_rat_solve_maze(int x_start, int y_start, int bearing, int x_end, int y_end,
		       int maze_width, int maze_height){
  /* initialize everything */
  path = malloc(4 * maze_width*maze_height); //max size of path is maze_width*maze_height
  path_length = 0;

  maze = malloc(sizeof(struct maze_node) * maze_width * maze_height * 4); //malloc extra space just in case
  memset(maze, 0, sizeof(struct maze_node) * maze_width * maze_height * 4); //same

  x_curr = x_start;
  y_curr = y_start;
  x_final = x_end;
  y_final = y_end;

  maze_set_bearing(bearing);
  maze_set_width(maze_width);
  maze_set_height(maze_height);

  /* assumes maze is a square
   * This is actually not necessary, since PiRat movement module has getter functions for maze dimensions
   */
  maze_square_dimension = maze_width;

  /* call recursive helper */
  int path_found = recursive_maze_solver();

  /* at end, traverse maze solution back and forth
   * I'll do this later. For now, just spin in circles
   */
  while(1){
    pi_rat_turn_left();
  }

  if(path_found == 1){
    traverse_path();
  }
}

void pi_rat_wander(int x_start, int y_start, int x_end, int y_end){
  while(x_start != x_end || y_start != y_end){
    int walls = pi_rat_get_walls();
    if((walls & 0b1) == 0){
      pi_rat_turn_left();
    }else if((walls & 0b10) == 0){
      /*do nothing, we're already facing forward */
    }else if((walls & 0b100) == 0){
      pi_rat_turn_right();
    }else{ //no path, need to backtrack, turn 180
      pi_rat_turn_right();
      pi_rat_turn_right();
    }
    timer_delay_ms(100);
    //execute the forward movement.
    pi_rat_go_forward();
    timer_delay_ms(100);
    x_start = pi_rat_get_x_cord();
    y_start = pi_rat_get_y_cord();
  }

  //once done, spin to indicate success
  while(1){
    pi_rat_turn_right();
  }
}

void pi_rat_control_test_maze_nodes(){
  printf("beginning maze node test in 3 seconds \n");

  timer_delay(3);

  /* setup a 4x4 maze */
  maze = malloc(sizeof(struct maze_node) * 4 * 4);
  memset(maze, 0, sizeof(struct maze_node) * 4 * 4);

  maze_square_dimension = 4;

  x_curr = 0;
  y_curr = 0;
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      maze_set_bearing(j);
      x_curr = j;
      y_curr = i;
      printf("you have 2 seconds to put up the wall(s) \n");
      timer_delay(2);
      update_maze();
      print_maze();
    }
  }
}
