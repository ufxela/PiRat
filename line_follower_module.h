/* this is a module which provides a driver for
 * a line follower device. We can read values from the
 * device which is communictaed to via I2C
 */

#ifndef LINE_FOLLOWER_MODULE_H
#define LINE_FOLLOWER_MODULE_H

/* initalizer function, sets everything up */
void line_follower_init();

/* with the line follower module's sensing side facing down, and the screwholes
 * being on the bottom of the rectangular face of the module, then, looking from the top
 * of the module, the left most sensor has a value of 0, and the right most has a value of 7
 * 
 * This function returns, as an int, which value the line is currently at. -1 if it is currently
 * at none of the lines. 
 */
int get_line_index();

/* returns the value in mm of where the line is with respect to the middle of the
 * line sensor. Unsure of whether I should define the middle of the line sensor
 * as the in between of sensors 4 & 5 (indexing from 1) or if it should
 * just be either 4 or 5.
 *
 * For now, it will be 4
 * 
 * if line position is invalid, will return -1000000 (as the distance between sensors surely will
 * be less than 1000000 mm).
 *
 * Maybe this function can be more specific than just a single line sensor lense
 * and use continuous values of line sensor outputs to determine how close the 
 * line sensor is to the line, and calculate a more exact/precise value for the line's
 * position
 */
int get_line_position();

/* sets the threshold for what is considered a line "hit" or not */
int set_line_threshold();

/* gets the threshold for which line detection is measured at */
int get_line_threhsold();

/* gets the line follower's reading at the index sensor_index
 */
int get_line_follower_value(int sensor_index);

/* returns in milimeters the distance between two of the line follower's sensors */
int get_distance_between_sensors();

/* a test function to showcase the module */
void line_follower_test();
#endif
