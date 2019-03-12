#include "car_control_module.h"
#include "pwm_input_module.h"
#include "cr_servo_module.h"
#include "timer.h"
#include "math.h"
#include "printf.h"

/* should I make a struct for all of this?
 * yeah probably. It would make the program more flexible and concise. But
 * I'll do it the bad way for now.
 */

/* internally data concerning information about the car */
static int wheel_base_mm; // distance between wheels, measured in  mm
static int wheel_circumference_mm; //measured in mm

/* internally kept positioning data */
static unsigned int bearing; // the angle, in degrees, which the car is currently facing 
static int x_pos; //the x position, in cm of the car
static int y_pos; //the y position, in cm

/* internally kept motor information
 *
 * Absolute position = motor_rotations*360 + motor_relative_angle
 * 
 * negative angle example: -30 degrees would be -1 rotations and position of
 * 330 (-360 + 330 = -30).
 */
static int wheel1_rotations; //from -infinity to positive infinity
static int wheel2_rotations;
static int wheel1_relative_angle;  //from 0 to 360. 
static int wheel2_relative_angle;

/* pin info */
static unsigned int wheel1_input_pin;
static unsigned int wheel2_input_pin;

/* wheels */
static cr_servo* wheel1;
static cr_servo* wheel2;

/* to help us update wheel positions */
static int previous_wheel1_angle;
static int previous_wheel2_angle;

/* throttles */
static int wheel1_throttle;
static int wheel2_throttle;

/* if I had perfectly set up CR servos, this wouldn't be needed */
static int wheel1_backwards_throttle;
static int wheel2_backwards_throttle;

/* shimmy constants */
static const unsigned int SHIMMY_ANGLE = 30;
static const unsigned int SHIMMY_FORWARD_DISTANCE_CM = 5;
//move back (move back cos(SHIMMY ANGLE) * SHIMMY_FORWARD_DISTANCE                                      
static const unsigned int SHIMMY_BACKWARDS_DISTANCE_CM = 4;

/* get the absolute value; simple function, so I wrote it myself */
static int abs(int x){
  if(x < 0){
    return -1*x;
  }else{
    return x;
  }
}

/* internal helper functions which needs to be called very often, whenever there is wheel movement.
 * Ideally, this would be called on a tiemr interrupt schedule, once every 20 ms maybe, but
 * I am not doing that for now... 
 *
 * Right now this is not accurate. Not only is it super finnicky, but it doesn't update correctly
 * about half the time. IT won't properly record wheelrotations, as it seems to skip them some of 
 * the time. 
 */
static void update_wheel_positions(){
  //update wheel1
  previous_wheel1_angle = wheel1_relative_angle;
  wheel1_relative_angle = pwm_input_get_angle(wheel1_input_pin);

  /* a safety check */
  // need a safety check here that makes sure previus wheel angle and current wheel angle don't
  //differ by too much (otherwise, it's likely an error */
  while(abs(wheel1_relative_angle - previous_wheel1_angle) > 10 && 
	abs(wheel1_relative_angle - previous_wheel1_angle) < 350){
    wheel1_relative_angle = pwm_input_get_angle(wheel1_input_pin);
  }

  if(wheel1_relative_angle > 320 && previous_wheel1_angle < 40){
    printf("current: %d previous: %d\n\n", wheel1_relative_angle, previous_wheel1_angle);
    wheel1_rotations--;
  }else if(wheel1_relative_angle < 40 && previous_wheel1_angle > 320){
    wheel1_rotations++;
  }

  //update wheel2
  previous_wheel2_angle = wheel2_relative_angle;
  wheel2_relative_angle = pwm_input_get_angle(wheel2_input_pin);
  
  /* safety check */
  while(abs(wheel2_relative_angle - previous_wheel2_angle) > 10 &&
        abs(wheel2_relative_angle - previous_wheel2_angle) < 350){
    wheel2_relative_angle = pwm_input_get_angle(wheel2_input_pin);
  }

  if(wheel2_relative_angle > 320 && previous_wheel2_angle < 40){
    wheel2_rotations--;
  }else if(wheel2_relative_angle < 40 && previous_wheel2_angle > 320){
    wheel2_rotations++;
  }
}

/* this function is really ugly because some info, like the servo info, I already have set
 * but other info, like wheelbase, I let the user set 
 */
void car_control_module_init(unsigned int input1, unsigned int input2, unsigned int output1,
			     unsigned int output2, unsigned int whl_base, unsigned int whl_crfnc){
  pwm_input_init();
  cr_servo_module_init(); //wow this inconsistency hurts...

  //setup pwm inputs
  wheel1_input_pin = input1;
  wheel2_input_pin = input2;

  pwm_add_input(wheel1_input_pin);
  pwm_add_input(wheel2_input_pin);

  //setup cr servo outputs
  wheel1 = cr_servo_new(output1);  
  wheel2 = cr_servo_new(output2);

  cr_servo_auto_setup(wheel2, 1509, 1286, 1732); //just some info that I already collected about
  cr_servo_auto_setup(wheel1, 1503, 1265, 1741); //my servos 

  wheel1_throttle = 20; //default throttles
  wheel2_throttle = 24;

  wheel1_backwards_throttle = 20;
  wheel2_backwards_throttle = 21;

  //update internal positioning/info
  wheel_base_mm = whl_base; //current wheelbase is 96 mm
  wheel_circumference_mm = whl_crfnc; //current wheel diameter is 188.5

  bearing = 0;
  x_pos = 0;
  y_pos = 0;

  wheel1_rotations = 0;
  wheel2_rotations = 0;

  wheel1_relative_angle = pwm_input_get_angle(wheel1_input_pin);
  wheel2_relative_angle = pwm_input_get_angle(wheel2_input_pin);

  previous_wheel1_angle = wheel1_relative_angle;
  previous_wheel2_angle = wheel2_relative_angle;
}

int get_x_position(){
  return x_pos;
}

int get_y_position(){
  return y_pos;
}

int get_wheel1_angle(){
  return wheel1_rotations * 360 + wheel1_relative_angle;
}

int get_wheel2_angle(){
  return wheel2_rotations * 360 + wheel2_relative_angle;
}


/* NOTE: WHEEL1 and WHEEL2 Throttles and angles should be negatives of eachother in general
 * because one wheel is flipped. I need to find a fix to this so that wheel1 positive is the oppposite
 * of wheel2 positive
 *
 * right now this program is buggy in that the recorded angle will occasionally just
 * stop short, and result in the motors rotating without stop
 */
void step_forward(int degrees){
  printf("Degrees: %d\n", degrees);
  update_wheel_positions();
  int new_wheel1_angle = get_wheel1_angle() - degrees;
  int new_wheel2_angle = get_wheel2_angle() + degrees;

  printf("Wheel1 anlge: %d, Wheel2 angle: %d\n", get_wheel1_angle(), get_wheel2_angle());
  printf("Desired angles: wheel1: %d, wheel2: %d\n", new_wheel1_angle, new_wheel2_angle); 

  //if forwards movement
  if(degrees > 0){
    //set throttles
    cr_servo_go_to_throttle(wheel1, wheel1_throttle); 
    cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);
    //while we haven't reached the desired angles, keep the throttle there
    while(get_wheel1_angle() > new_wheel1_angle || get_wheel2_angle() < new_wheel2_angle){
      //if a single wheel reaches destination before the other, stop.
      if(get_wheel1_angle() <= new_wheel1_angle){
	cr_servo_go_to_throttle(wheel1, 0);
	//        cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);
      }
      if(get_wheel2_angle() >= new_wheel2_angle){
	cr_servo_go_to_throttle(wheel2, 0);
	//	cr_servo_go_to_throttle(wheel1, wheel1_throttle);
      }
      //update wheel positions
      update_wheel_positions();
      /* this printf is here for debugging purposes */
      printf("Wheel1 anlge: %d, Wheel2 angle: %d\n", get_wheel1_angle(), get_wheel2_angle());
    }
    //stop once position is reached
    cr_servo_go_to_throttle(wheel1, 0);
    cr_servo_go_to_throttle(wheel2, 0);
  }else if(degrees < 0){ //backwards, same except negative throttle.
    //this can probably be combined with the above using some sign multiplications.
    //set throttles                                                                                 
    cr_servo_go_to_throttle(wheel1, -1*wheel1_backwards_throttle);
    cr_servo_go_to_throttle(wheel2, wheel2_backwards_throttle);
    //while we haven't reached the desired angles, keep the throttle there                       
    while(get_wheel1_angle() < new_wheel1_angle || get_wheel2_angle() > new_wheel2_angle){
      //if a single wheel reaches destination before the other, stop.                             
      if(get_wheel1_angle() >= new_wheel1_angle){
        cr_servo_go_to_throttle(wheel1, 0);
	//        cr_servo_go_to_throttle(wheel2, wheel2_backwards_throttle);
      }
      if(get_wheel2_angle() <= new_wheel2_angle){
        cr_servo_go_to_throttle(wheel2, 0);
	//        cr_servo_go_to_throttle(wheel1, -1*wheel1_backwards_throttle);
      } 
      //update wheel positions
      update_wheel_positions();
    }
    //stop once position is reached                                                                  
    cr_servo_go_to_throttle(wheel1, 0); //these can probably be moved out of the if statement. 
    cr_servo_go_to_throttle(wheel2, 0);
  }
}

void move_forward(int distance_in_cm){
  int angle = distance_in_cm * 100 * 360 / wheel_circumference_mm;
  step_forward(angle);
}

void move_forward_2(int distance_in_cm){
  int steps = distance_in_cm;
  if(steps < 0){
    steps = -1*steps;
  }

  //step 1 cm a bunch of times
  for(int i = 0; i < steps; i++){
    move_forward(distance_in_cm/steps); //+ or - 1 depending on sign of distance_in_cm
    timer_delay(3); //change this delay time depending on trial and error
  }
}

void move_forward_3(int distance_in_cm){
  //only implement this if move_forward/move_forward_2 don't work
}

void move_wheel1(int degrees){
  int new_wheel1_angle = get_wheel1_angle() + degrees;
  
  /* this code was copied from step forward. Bad practice probably. */
  //if forwards movement                                                                                 
  if(degrees > 0){
    //set throttles                                                                                      
    cr_servo_go_to_throttle(wheel1, wheel1_throttle);
    //while we haven't reached the desired angles, keep the throttle there                               
    while(get_wheel1_angle() < new_wheel1_angle){
      //if a single wheel reaches destination before the other, stop.                                    
      if(get_wheel1_angle() >= new_wheel1_angle){
        cr_servo_go_to_throttle(wheel1, 0);
      }
      //update wheel positions                                                                           
      update_wheel_positions();
    }
    //stop once position is reached                                                                      
    cr_servo_go_to_throttle(wheel1, 0);
  }else if(degrees < 0){ //backwards, same except negative throttle.                               
    //this can probably be combined with the above using some sign multiplications.                  
    //set throttles                                                                                
    cr_servo_go_to_throttle(wheel1, -1*wheel1_backwards_throttle);
    
      //while we haven't reached the desired angles, keep the throttle there                          
    while(get_wheel1_angle() > new_wheel1_angle){
      //if a single wheel reaches destination before the other, stop.                                 
      if(get_wheel1_angle() <= new_wheel1_angle){
	cr_servo_go_to_throttle(wheel1, 0);
      }
      update_wheel_positions();
    }
    
    cr_servo_go_to_throttle(wheel1, 0); //can be moved out of the if statement. 
  }
}


/* this was genuinely painful to write. I definitely should have made wheel objects */
void move_wheel2(int degrees){
  int new_wheel2_angle = get_wheel2_angle() + degrees;
  
  /* this code was copied from step forward. Bad practice probably. */
  //if forwards movement                                                                             
  if(degrees > 0){
    //set throttles                                                                               
    cr_servo_go_to_throttle(wheel2, wheel2_throttle);
    //while we haven't reached the desired angles, keep the throttle there                     
    while(get_wheel2_angle() < new_wheel2_angle){
      //if a single wheel reaches destination before the other, stop.                                 
      if(get_wheel2_angle() >= new_wheel2_angle){
        cr_servo_go_to_throttle(wheel2, 0);
      }	
	//update wheel positions                                                               
      update_wheel_positions();
    }
    //stop once position is reached                                                                    
    cr_servo_go_to_throttle(wheel2, 0);
  }else if(degrees < 0){ //backwards, same except negative throttle.                                   
    //this can probably be combined with the above using some sign multiplications.               
    //set throttles                                                                                 
    cr_servo_go_to_throttle(wheel2, -1*wheel2_backwards_throttle);
    //while we haven't reached the desired angles, keep the throttle there                       
    while(get_wheel2_angle() > new_wheel2_angle){
        //if a single wheel reaches destination before the other, stop.                            
      if(get_wheel2_angle() <= new_wheel2_angle){
          cr_servo_go_to_throttle(wheel2, 0);
        }
        update_wheel_positions();
      }
      
      cr_servo_go_to_throttle(wheel2, 0); //can be moved out of the if statement.                    
  }
}

/* needs to rotate one wheel degrees * pi * wheelbase / wheel_circumference 
 * and the other wheel the same amount in the opposite direciton
 *
 * May want to implement this in a different way than I currently am
 */
void turn(int degrees){
  int angle_to_move_wheels = degrees * wheel_base_mm * 314 / 100 / wheel_circumference_mm;
  
  int new_wheel1_angle = get_wheel1_angle() - angle_to_move_wheels; //assuming wheel 1 is to the left
  int new_wheel2_angle = get_wheel2_angle() + angle_to_move_wheels; //may need to chance one or both signs

  if(degrees > 0){
    //set throttles
    cr_servo_go_to_throttle(wheel1, wheel1_throttle); //may need to change signs
    cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);

    //move until we've acheived the desired position
    while(get_wheel1_angle() >= new_wheel1_angle || get_wheel2_angle() <= new_wheel2_angle){
      update_wheel_positions();
      if(get_wheel1_angle() >= new_wheel1_angle){
        cr_servo_go_to_throttle(wheel1, 0);
        cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);
      }
      if(get_wheel2_angle() <= new_wheel2_angle){
        cr_servo_go_to_throttle(wheel2, 0);
        cr_servo_go_to_throttle(wheel1, -1*wheel1_throttle);
      }
    }
    cr_servo_go_to_throttle(wheel1, 0);
    cr_servo_go_to_throttle(wheel2, 0);
  }else if(degrees < 0){
    cr_servo_go_to_throttle(wheel1, -1*wheel1_throttle);
    cr_servo_go_to_throttle(wheel2, wheel2_throttle);

    while(get_wheel1_angle() <= new_wheel1_angle || get_wheel2_angle() >= new_wheel2_angle){
      update_wheel_positions();
      if(get_wheel1_angle() <= new_wheel1_angle){
        cr_servo_go_to_throttle(wheel1, 0);
        cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);
      }
      if(get_wheel2_angle() >= new_wheel2_angle){
        cr_servo_go_to_throttle(wheel2, 0);
        cr_servo_go_to_throttle(wheel1, -1*wheel1_throttle);
      }
    }
    cr_servo_go_to_throttle(wheel1, 0);
    cr_servo_go_to_throttle(wheel2, 0);
  } 
}

/* shimmy left one cm */
/* values of rotation and forward movement need to be adjusted to give proper behavior */
void shimmy_left(){
  //rotate left
  turn(SHIMMY_ANGLE);

  //move forward
  move_forward(SHIMMY_FORWARD_DISTANCE_CM);

  //rotate right
  turn(-SHIMMY_ANGLE);

  //move back (move back cos(SHIMMY ANGLE) * SHIMMY_FORWARD_DISTANCE
  move_forward(SHIMMY_BACKWARDS_DISTANCE_CM);

}
/* shimmy right 1 cm */
void shimmy_right(){
  turn(-SHIMMY_ANGLE);
  move_forward(SHIMMY_FORWARD_DISTANCE_CM);
  turn(SHIMMY_ANGLE);
  move_forward(-SHIMMY_BACKWARDS_DISTANCE_CM);
}

void set_wheel_throttles(int throttle){
  wheel1_throttle = throttle;
  wheel2_throttle = throttle;
}

void set_wheel1_throttle(int throttle){
  wheel1_throttle = throttle;
}

void set_wheel2_throttle(int throttle){
  wheel2_throttle = throttle;
}

void test_car_control_module(unsigned int input1, unsigned int input2, unsigned int output1,
                             unsigned int output2){
  printf("Beginning car control test in 2 s\n");

  timer_delay(2);
  car_control_module_init(input1, input2, output1, output2,96, 188); //estimated wheelbase/circumfrence

  printf("move forward 20\n");
  move_forward(20);

  timer_delay(2);

  printf("wheel positions: %d, %d\n", get_wheel1_angle(), get_wheel2_angle());
 
  printf("move forward -10\n");
  move_forward(-10);

  timer_delay(2);

  printf("wheel positions: %d, %d\n", get_wheel1_angle(), get_wheel2_angle());

  //these don't currently work... or do they?

  printf("move forward_2, 10\n");
  move_forward_2(10);

  printf("wheel positions: %d, %d\n", get_wheel1_angle(), get_wheel2_angle());

  printf("move forward_2, -10\n");
  move_forward_2(-10);

}

