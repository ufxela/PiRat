#include "math.h"
#include "car_control_module.h"
#include "ultrasonic_module.h"
#include "line_follower_module.h"
#include "timer.h"
#include "printf.h"

/* information about the maze */
const unsigned int MAZE_WIDTH = 6;
const unsigned int MAZE_HEIGHT = 6;
const unsigned int BLOCK_SIDE_LENGTH_CM = 20;

/* keeps track of where the COR of the car within each block should be i.e. where
 * the Pi Rat should be within a block 
 */
const unsigned int CENTER_OF_ROTATION_IN_BLOCK_X_MM = 100; //is the middle of block side length
const unsigned int CENTER_OF_ROTATION_IN_BLOCK_Y_MM = 100; //measured in mm

/* information about Pi Rat's position within the maze. Should this module
 * keep track of this?
 */
unsigned int maze_x_coord;
unsigned int maze_y_coord;

