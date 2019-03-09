#ifndef CAR_CONTROL_MODULE_H
#define CAR_CONTROL_MODULE_H

/* initializer function, starts everything up */
void car_control_module_init();

/* returns through the passed parameters, the absolute angle each wheel is
 * at, one representation of the vehicle's current position (though not the
 * absolute position of the vehicle...)
 */
void get_absolute_position(int * wheel1, int * wheel 2);

/* returns absolute position of wheel1 */
int get_wheel1_position();

/* returns absolute position of wheel2 */
int get_wheel2_position();

/* steps forward a set amount of degrees, updating the internal 
 * data structure which keeps track of the vehicle's wheel positions 
 *
 * negative values are acceptable and are moving backward
 */
void step_forward(int degrees);

/* calculates, based on constants which define the wheel radius,
 * the number of degrees which must be moved forward to move foward a set
 * distance, measured in cm.
 *
 * Then, while the wheels aren't at that angle, it will move them.
 * i.e. make a single call to step_forward
 *
 * Negative values are acceptable, and are moving backward
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
void move_wheel1(int degrees);

/* wheel to move degrees */
void move_wheel2(int degrees);

/* calculates the angles each wheel has to move (one forward x degrees, one
 * backward x degrees), based on internally kept constants representing the
 * wheelbase (distance between wheels) and the wheel circumference. 
 * and then moves the wheels to those positions to create turning
 *
 * positive angles are counter-clockwise
 */
void turn(int degrees);

/* shimmies left (lateral motion). Does this by turning x degrees left
 * then moving forward distance of y, then turning x degrees to the right
 * and then moving backward distance of f(y) where f is some trigonometric
 * function which I have yet to determine 
 * Will shimmy left a total of 1 cm (subject to change depending on
 * what I want)
 * Or maybe it will accept a parameter which represents the distance
 * to shimmy left / right
 */
void shimmy_left();

/* same as shimmy left, except to the right. */
void shimmy_right();
#endif
