#include "car_control_module.h"
#include "pwm_input_module.h"
#include "cr_servo_module.h"
#include "timer.h"
#include "math.h"

/* all of these variables should probably be static */
/* should I make a struct for all of this?
 * yeah probably. It would make the program more flexible and concise. But
 * I'll do it the bad way for now.
 */
/* internally data concerning information about the car */
unsigned int wheel_base_mm; // distance between wheels, measured in  mm
unsigned int wheel_circumference_mm; //measured in mm

/* internally kept positioning data */
unsigned int bearing; // the angle, in degrees, which the car is currently facing 
int x_pos; //the x position, in cm of the car
int y_pos; //the y position, in cm

/* internally kept motor information
 *
 * Absolute position = motor_rotations*360 + motor_relative_angle
 * 
 * negative angle example: -30 degrees would be -1 rotations and position of
 * 330 (-360 + 330 = -30).
 */
int wheel1_rotations; //from -infinity to positive infinity
int wheel2_rotations;
unsigned int wheel1_relative_angle;  //from 0 to 360. 
unsigned int wheel2_relative_angle;

/* pin info */
unsigned int wheel1_input_pin;
unsigned int wheel2_input_pin;

/* wheels */
cr_servo* wheel1;
cr_servo* wheel2;

/* to help us update wheel positions */
static unsigned int previous_wheel1_angle;
static unsigned int previous_wheel2_angle;

/* throttles */
static int wheel1_throttle;
static int wheel2_throttle;

/* internal helper functions which needs to be called very often, whenever there is wheel movement.
 * Ideally, this would be called on a tiemr interrupt schedule, once every 20 ms maybe, but
 * I am not doing that for now... 
 */
static void update_wheel_positions(){
  //update wheel1
  previous_wheel1_angle = wheel1_relative_angle;
  wheel1_relative_angle = pwm_input_get_angle(wheel1_input_pin);
  if(wheel1_relative_angle > 340 && previous_wheel1_angle < 20){
    wheel1_rotations--;
  }else if(wheel1_relative_angle < 20 && previous_wheel1_angle > 340){
    wheel1_rotaions++;
  }

  //update wheel2
  previous_wheel2_angle = wheel2_relative_angle;
  wheel2_relative_angle = pwm_input_get_angle(wheel1_input_pin);
  if(wheel2_relative_angle > 340 && previous_wheel2_angle < 20){
    wheel2_rotations--;
  }else if(wheel2_relative_angle < 20 && previous_wheel2_angle > 340){
    wheel2_rotaions++;
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

  cr_servo_auto_setup(wheel1, 1509, 1286, 1732); //just some info that I already collected about
  cr_servo_auto_setup(wheel2, 1503, 1265, 1741); //my servos 

  wheel1_throttle = 20; //default throttles
  wheel2_throttle = 20;

  //update internal positioning/info
  wheel_base_mm = whl_base;
  wheel_circumference_mm = whl_crfnc;

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

/* unsure of why I need these */
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

void step_forward(int degrees){
  int new_wheel1_angle = get_wheel1_angle() + degrees;
  int new_wheel2_angle = get_wheel2_angle() + degrees;

  //if forwards movement
  if(degrees > 0){
    //set throttles
    cr_servo_go_to_throttle(wheel1, wheel1_throttle);
    cr_servo_go_to_throttle(wheel2, wheel2_throttle);
    //while we haven't reached the desired angles, keep the throttle there
    while(get_wheel1_angle() < new_wheel1_angle || get_wheel2_angle() < new_wheel2_angle){
      //if a single wheel reaches destination before the other, stop.
      if(get_wheel1_angle() >= new_wheel1_angle){
	cr_servo_go_to_throttle(wheel1, 0);
      }
      if(get_wheel2_angle() >= new_wheel2_angle){
	cr_servo_go_to_throttle(wheel2, 0);
      }
      //update wheel positions
      update_wheel_positions();
    }
    //stop once position is reached
    cr_servo_go_to_throttle(wheel1, 0);
    cr_servo_go_to_throttle(wheel2, 0);
  }else if(degrees < 0){ //backwards, same except negative throttle.
    //this can probably be combined with the above using some sign multiplications.
    //set throttles                                                                                 
    cr_servo_go_to_throttle(wheel1, -1*wheel1_throttle);
    cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);
    //while we haven't reached the desired angles, keep the throttle there                       
    while(get_wheel1_angle() > new_wheel1_angle || get_wheel2_angle() > new_wheel2_angle){
      //if a single wheel reaches destination before the other, stop.                             
      if(get_wheel1_angle() <= new_wheel1_angle){
        cr_servo_go_to_throttle(wheel1, 0);
      }
      if(get_wheel2_angle() <= new_wheel2_angle){
        cr_servo_go_to_throttle(wheel2, 0);
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
  unsigned int angle = distance_in_cm * 100 * 360 / wheel_circumference_mm;
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
    timer_delay_ms(5000/wheel1_throttle); //change this delay time depending on trial and error
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

      //update wheel positions                                                                           
      update_wheel_positions();
    }
    //stop once position is reached                                                                      
    cr_servo_go_to_throttle(wheel1, 0);
    }else if(degrees < 0){ //backwards, same except negative throttle.                               
      //this can probably be combined with the above using some sign multiplications.                  
      //set throttles                                                                                
      cr_servo_go_to_throttle(wheel1, -1*wheel1_throttle);
      
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
	
	//update wheel positions                                                                                  
	update_wheel_positions();
      }
      //stop once position is reached                                                                             
      cr_servo_go_to_throttle(wheel2, 0);
    }else if(degrees < 0){ //backwards, same except negative throttle.                                             
      //this can probably be combined with the above using some sign multiplications.                               
      //set throttles                                                                                             
      cr_servo_go_to_throttle(wheel2, -1*wheel2_throttle);
      
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
}

void turn(int degrees){

}

/* shimmy left one cm */
void shimmy_left();

/* shimmy right 1 cm */
void shimmy_right();

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
