/* this module provides for pi rat specific movement given information about the maze
 * The module uses the line follower module to provide for correctional movement
 * to ensure the Pi Rat maintains a proper orentiation. 
 *
 * Also, uses the ultrasonic sensor to ensure that the Pi Rat goes to the end of each
 * corridor. Also, may use ultrasonic sensor to keep the Pi Rat centered in the corridor,
 * however, the line follower module makes this kinda irrelevant 
 */

#ifndef PI_RAT_MOVEMENT_MODULE_H
#define PI_RAT_MOVEMENT_MODULE_H

/* sets everything up */
void pi_rat_movement_init(unsigned int input1, unsigned int input2, unsigned int output1,
                          unsigned int output2);

/* goes forward one step in the maze 
 * measures line position before movement and then
 * line position after movement and then at the end
 * uses this information to correct the position of the Pi Rat
 * if needed
 *
 * If the position needed to be corrected, this function will then go forward
 * a minute ammount to adjust for the forward motion which the off center direction of 
 * movement caused the Pi Rat to lack. Will do this with trig (maybe a switch statement lookup table
 * since angles will be discrete (only 7 of them possible, for the 8 line sensors). 
 */
void pi_rat_go_forward();

/* steps back one step in the maze 
 * Also implements correctional movement
 */
void pi_rat_go_back();

void pi_rat_go_to_corridor_end();

/* turns left 90 degrees to setup exploration of a left corridor 
 * May or may not utilize correctional movement to ensure that turn
 * is true to 90 degrees (can do this by centering the line after the turn
 * has occured, which works assuming that the starting position is correct, 
 * so I guess it needs to ensure it's starting position is correct first i.e.
 * it's facing the right orientation and it's in the right positon in the square of
 * the maze that it's turning at)
 */
void pi_rat_turn_left();

/* turns right, same potential for correctional movement */
void pi_rat_turn_right();

/* correctional movement for move_forward/move_backward
 * Takes the following steps to correct movement:
 * based on angle that the Pi Rat calculates that it is at, determines the angle to 
 * turn left or right to correct the orientation of the Pi.
 * Then:
 * based on the average of start_line and end_line, shimmys either
 * left or right to center the line on the line reader
 */
void pi_rat_correct_line_position_f(int start_line, int end_line);

/* the backwards flavor of the above function */
void pi_rat_correct_line_position_b(int start_line, int end_line);

/* Assumes the Pi Rat was in the correct position before the turn
 * Aligns the robot with the line after the turn to make sure
 * it did not turn too far / not far enough
 */
void pi_rat_correct_turn();

/* a bunch of positioning / maze spec functions */
int pi_rat_get_x_cord();

int pi_rat_get_y_cord();

int pi_rat_get_bearing();

int pi_rat_get_maze_height();

int pi_rat_get_maze_width();

int pi_rat_get_wall_length();

/* a function which decomposes line position correcting 
 *
 * correct's rat's lateral error on the line
 *
 * in the future, I may want to do this solely with the ultrasonic sensor and remove
 * the line sensor all together.
 */
void pi_rat_correct_lateral(int current_line);

/* similar to correct_lateral, except corrects the angular error */
void pi_rat_correct_angle(int start_line, int end_line);

void test_pi_rat_movement(unsigned int input1, unsigned int input2, unsigned int output1,
                          unsigned int output2, unsigned int trigger, unsigned int echo,
                          unsigned int pan);

/* will move pi rat up (1), right (2), down (3) or left (0) in the maze
 * depending on what change (int) is. 
 */
void pi_rat_position_change(int direction);

/* setter functions */
void maze_set_width(int width);

void maze_set_height(int height);

void maze_set_bearing(int bearing);

#endif
