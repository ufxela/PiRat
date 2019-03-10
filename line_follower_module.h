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
