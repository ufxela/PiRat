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

/* getters for the x and y coordinates that 
 * the car calculates that it is at, using trig and stuff.
 * return values are in cm (I might change later).
 * calculates things based on internal constants for wheel circumference 
 */
int get_x_position();

int get_y_position();

/* returns absolute position/angle of wheel1 */
int get_wheel_angle();


/* steps forward a set amount of degrees, updating the internal 
 * data structure which keeps track of the vehicle's wheel positions 
 *
 * negative values are acceptable and are moving backward
 */
void step_forward(int degrees);

/* another version of the step forward function which continuously 
 * monitors the position of each wheel and ensures that they're at the same angle
 * to provide for true straight forward motion
 * inspired by:
 * http://www.robotc.net/wikiarchive/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Using_encoders_to_drive_straight
 *
 */
void step_forward_2(int degrees);

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

/* yet another strategy for moving forward. 
 * this strategy only moves one wheel at a time. Moves the left wheel a little
 * then the right a little & repeat, until the correct position is reached.
 * who knows, this could work the best. Or not.
 */
void move_forward_3(int distance_in_cm);

/* moves wheel 1 a set number of degrees */
//void move_wheel1(int degrees);

/* wheel to move degrees */
//void move_wheel2(int degrees);

/* calculates the angles each wheel has to move (one forward x degrees, one
 * backward x degrees), based on internally kept constants representing the
 * wheelbase (distance between wheels) and the wheel circumference. 
 * and then moves the wheels to those positions to create turning
 *
 * positive angles are counter-clockwise
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

/* sets the throttles which we move the wheels at throughout the program
 * remember: throttle is from [-100, 100]
 */
//void set_wheel_throttles(int throttle);

/* independent throttle setting, if we need it */
//void set_wheel1_throttle(int throttle);

/* independent throttle setting, if we need it */
//void set_wheel2_throttle(int throttle);

/* should I have separate internal variables and setter functions to 
 * allow for independent and settable backward throttles for each wheel?*/

/* also, should I keep track of the (x,y) position of the car at any given 
 * time? It would require the addition of trig functions, if I turn at angles
 * which are not 90 degrees, but it would also be cool
 */

/* test function to showcase this module off */
void test_car_control_module(unsigned int input1, unsigned int input2, unsigned int output1,
			     unsigned int output2);

#endif
