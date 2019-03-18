#ifndef PI_RAT_CONTROL_H
#define PI_RAT_CONTROL_H

/* initializer function, sets up pi rat */
void pi_rat_init();

/* place the Pi at a given location and bearing, and ask it to solve a maze */
void pi_rat_solve_maze(int x_start, int y_start, int bearing, int x_end, int y_end,
		       int maze_width, int maze_height){

}

#endif
