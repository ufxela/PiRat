#ifndef PI_RAT_CONTROL_H
#define PI_RAT_CONTROL_H

/* initializer function, sets up pi rat */
void pi_rat_init(unsigned int input1, unsigned int input2, unsigned int output1,
		 unsigned int output2, unsigned int trigger, unsigned int echo,
		 unsigned int pan);

/* place the Pi at a given location and bearing, and ask it to solve a maze */
/* if x_end and y_end are invalid values, this will search for an opening in the maze which will 
 * indicate the exit
 * otherwise, it will go to the position of x_end and y_end and spin in place there
 */
void pi_rat_solve_maze(int x_start, int y_start, int bearing, int x_end, int y_end,
                       int maze_width, int maze_height);
		       
/* just wanders around until end position is reached */
void pi_rat_wander(int x_start, int y_start, int x_end, int y_end);

/* to help make the program more transparent so I can see what's going on */
void pi_rat_control_test_maze_nodes();

#endif
