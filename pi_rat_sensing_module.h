#ifndef PI_RAT_SENSING_MODULE_H
#define PI_RAT_SENSING_MODULE_H

/* sets up ultrasonic sensor, line follower and panning servo */
void pi_rat_sensing_module_init(unsigned int trigger_pin, unsigned int echo_pin, 
				unsigned int servo_pin);

/* determines if there is a wall to the left, center and right and returns a value
 * corresponding to it
 * The return value is a 3 bit number, with LSB representing left wall and MSB representing right wall
 * 
 * ex1) return of 5 = 101 means there's a left wall, right wall and no center wall
 * ex2) return of 3 = 011 means left and forward walls
 */
int pi_rat_get_walls();

/* gets the current position of the line under the Pi Rat 
 */
int pi_rat_line_position();

/* functions to get physical values about the Pi */
int pi_rat_get_wheelbase();

int pi_rat_get_wheel_circumference();

int pi_rat_get_line_follower_sensor_dist();

/* retuns size of wheelbase in mm */
int pi_rat_get_wheel_base_mm();

/* retursn the wheel circumference in mm */
int pi_rat_get_wheel_circumference_mm();

/* returns in mm the distance between sensors on the line follower */
int pi_rat_get_line_sensor_dist_mm();

void pi_rat_sensing_test(unsigned int trigger_pin, unsigned int echo_pin, unsigned int servo_pin);

#endif
