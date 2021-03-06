/* This function provides basic movement for a car powered by two
 * parallax feedback 360 servos. 
 */

#ifndef CAR_CONTROL_MODULE_H
#define CAR_CONTROL_MODULE_H

typedef volatile struct wheel wheel;

/* initializer function, starts everything up */
void car_control_module_init(unsigned int input1, unsigned int input2, unsigned int output1,
			     unsigned int output2, unsigned int whl_base,
			     unsigned int wheel_crfnc);

/* returns absolute position/angle of wheel1 */
int get_wheel_angle();

/* forward movement function which continuously 
 * monitors the position of each wheel and ensures that they're at the same angle
 * to provide for true straight forward motion
 * inspired by:
 * http://www.robotc.net/wikiarchive/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Using_encoders_to_drive_straight
 *
 * since it's better, I made it step_forward, and not step_forward_2
 */
void step_forward(int degrees);

/* same as step_forward, but in other direction 
 * Degrees is positive, and measures in the negative direction 
 *
 */
void step_backward(int degrees);

/* calculates, based on constants which define the wheel radius,
 * the number of degrees which must be moved forward to move foward a set
 * distance, measured in cm.
 *
 * Then, while the wheels aren't at that angle, it will move them.
 * i.e. make a single call to step_forward
 *
 * Negative values are acceptable, and are moving backward
 *
 * Also, every time a move_forward function is called, we should update the internal
 * positions, based on the distance moved forward as well as the bearing.
 */
void move_forward(int distance_in_cm);

/* another strategy of moving forward that I may potentially do
 * Will move forward by using step_forward, but calling it n times, instead
 * of just 1, and moving an angle of angle/n times in each call, with 
 * a potential delay between calls. 
 * who knows, this way could work better
 */
void move_forward_2(int distance_in_cm);

/* calculates the angles each wheel has to move (one forward x degrees, one
 * backward x degrees), based on internally kept constants representing the
 * wheelbase (distance between wheels) and the wheel circumference. 
 * and then moves the wheels to those positions to create turning
 *
 * positive angles are clockwise
 *
 * Should also update internally kept variable which tracks bearing.
 */
void turn(int degrees);

/* shimmies left (lateral motion). Does this by turning x degrees left
 * then moving forward distance of y, then turning x degrees to the right
 * and then moving backward distance of f(y) where f is some trigonometric
 * function which I have yet to determine. Hopefully I will just be able to
 * calculate a constant to multiply y by, so I don't need to implement 
 * trig functions. 
 * Will shimmy left a total of 1 cm (subject to change depending on
 * what I want)
 * Or maybe it will accept a parameter which represents the distance
 * to shimmy left / right
 *
 * Should also update internal variables which keep track of (x,y) positions
 * by utilizing bearing, as well as the distance travelled. 
 */
void shimmy_left();

/* same as shimmy left, except to the right. */
void shimmy_right();

/* returns value, in mm, of wheelbase */
int get_wheel_base();

/* returns value, in mm of wheel circumference */
int get_wheel_circumference();

/* test function to showcase this module off */
void test_car_control_module(unsigned int input1, unsigned int input2, unsigned int output1,
			     unsigned int output2);

#endif
