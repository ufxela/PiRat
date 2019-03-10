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
void pi_rat_movement_init();

/* goes forward one step in the maze 
 * measures line position before movement and then
 * line position after movement and then at the end
 * uses this information to correct the position of the Pi Rat
 * if needed
 */
void pi_rat_go_forward();

/* steps back one step in the maze 
 * Also implements correctional movement
 */
void pi_rat_go_back();


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
void pi_rat_correct_lateral(int start_line, int end_line);

/* idk how I'll do this. */
void pi_rat_correct_horizontal();

/* flips the Pi rat 180 degrees. May be useful for backtracking. May not */
void pi_rat_turn_180();

#endif
