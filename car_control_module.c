#include "car_control_module.h"
#include "pwm_input_module.h"
#include "cr_servo_module.h"
#include "timer.h"
#include "math.h"
#include "printf.h"
#include "malloc.h"

/* should I have a wheel1_go_to_throttle() function which compartmentalizezs cr_servo?*/


/* internally data concerning information about the car */
static  int wheel_base_mm; // distance between wheels, measured in  mm
static  int wheel_circumference_mm; //measured in mm

/* position data */
static  unsigned int bearing; // the angle, in degrees, which the car is currently facing
static  int x_pos_cm; //the x position, in cm of the car
static  int y_pos_cm; //the y position, in cm


/* shimmy constants */
static const unsigned int SHIMMY_ANGLE = 30;
static const unsigned int SHIMMY_FORWARD_DISTANCE_CM = 5;
//move back (move back cos(SHIMMY ANGLE) * SHIMMY_FORWARD_DISTANCE
static const unsigned int SHIMMY_BACKWARDS_DISTANCE_CM = 4;

/* for updating angles */
static const unsigned int MAX_ANGLE_TRIES = 20;

static wheel * left_wheel;
static wheel * right_wheel;


/* internally kept motor information
 *
 * Absolute position = motor_rotations*360 + motor_relative_angle
 *
 * negative angle example: -30 degrees would be -1 rotations and position of
 * 330 (-360 + 330 = -30).
 */
struct wheel{
    int rotations; //from -infinity to positive infinity
    int relative_angle; //from 0 to 360.
    unsigned int input_pin; //for reading angle
    cr_servo * motor; //for sending outputs
    int forwards_throttle;
    int backwards_throttle;
    int previous_angle; //to help us update wheel positions
};

/* get the absolute value; simple function, so I wrote it myself */
static int abs(int x){
  if(x < 0){
    return -1*x;
  }else{
    return x;
  }
}

static int get_angle_average(wheel * wheel){
  int total = 0;
  for(int i = 0; i < 5; i++){
    total += pwm_input_get_angle(wheel->input_pin);
  }
  return total / 5;
}

/* internal helper functions which needs to be called very often, whenever there is wheel movement.
 * Ideally, this would be called on a tiemr interrupt schedule, once every 20 ms maybe, but
 * I am not doing that for now...
 *
 * Right now this is not accurate. Not only is it super finnicky, but it doesn't update correctly
 * about half the time. IT won't properly record wheelrotations, as it seems to skip them some of
 * the time.
 *
 * Wheel1's (left wheel) angles are negated, so that it's positive direciton is forwards
 */
static void update_wheel_positions(){
  //update left_wheel
  left_wheel->previous_angle = left_wheel->relative_angle;
  // need to negate in order to keep forwards the positive direction
  left_wheel->relative_angle = -1*get_angle_average(left_wheel);

  /* a safety check
   * need a safety check here that makes sure previus wheel angle and current wheel angle don't
   * differ by too much (otherwise, it's likely an error
   *
   * This occasionally glitches out, so it's a problem. Needs to be fixed.
   * It would work perfectly, if get_wheel_angle worked perfectly.
   */

  int timeout = 0;
  int total_wheel_angle = 0;
  while(abs(left_wheel->relative_angle - left_wheel->previous_angle) > 6 &&
	abs(left_wheel->relative_angle - left_wheel->previous_angle) < 354){
    left_wheel->relative_angle = -1*get_angle_average(left_wheel);
    timeout++;
    total_wheel_angle += left_wheel->relative_angle;
    if(timeout >= MAX_ANGLE_TRIES){
      left_wheel->previous_angle = total_wheel_angle / timeout;
    }
  }

  /* update rotations */
  if(left_wheel->relative_angle < -1*340 && left_wheel->previous_angle > -1*20){
    left_wheel->rotations++;
  }else if(left_wheel->relative_angle > -1*20 && left_wheel->previous_angle < -1*340){
    left_wheel->rotations--;
  }

  //clear these for right_wheel
  timeout = 0;
  total_wheel_angle = 0;

  //update wheel2
  right_wheel->previous_angle = right_wheel->relative_angle;
  right_wheel->relative_angle = get_angle_average(right_wheel);

  /* safety check */
  while(abs(right_wheel->relative_angle - right_wheel->previous_angle) > 6 &&
        abs(right_wheel->relative_angle - right_wheel->previous_angle) < 354){
    right_wheel->relative_angle = get_angle_average(right_wheel);
    timeout++;
    total_wheel_angle += right_wheel->relative_angle;
    if(timeout >= MAX_ANGLE_TRIES){
      right_wheel->previous_angle = total_wheel_angle/timeout;
      }
  }


  if(right_wheel->relative_angle > 340 && right_wheel->previous_angle < 20){
    right_wheel->rotations--;
  }else if(right_wheel->relative_angle < 20 && right_wheel->previous_angle > 340){
    right_wheel->rotations++;
  }
}

static void motor_set_throttle(wheel * wheel, int throttle){
    cr_servo_go_to_throttle(wheel->motor, throttle);
}

/* this function is really ugly because some info, like the servo info, I already have set
 * but other info, like wheelbase, I let the user set
 */
void car_control_module_init(unsigned int input1, unsigned int input2, unsigned int output1,
			     unsigned int output2, unsigned int whl_base, unsigned int whl_crfnc){
  pwm_input_init();
  cr_servo_module_init(); //wow this inconsistency hurts...
  left_wheel = malloc(sizeof(wheel));
  right_wheel = malloc(sizeof(wheel));

  //setup pwm inputs
  left_wheel->input_pin = input1;
  right_wheel->input_pin = input2;

  pwm_add_input(left_wheel->input_pin);
  pwm_add_input(right_wheel->input_pin);

  //setup cr servo outputs
  left_wheel->motor = cr_servo_new(output1);
  right_wheel->motor = cr_servo_new(output2);

  cr_servo_auto_setup(left_wheel->motor, 1503, 1265, 1741); //may be incorrect values
  cr_servo_auto_setup(right_wheel->motor, 1509, 1286, 1732);

  left_wheel->forwards_throttle = 22; //default throttles
  right_wheel->forwards_throttle = -24;

  left_wheel->backwards_throttle = -21;
  right_wheel->backwards_throttle = 20;

  //update internal positioning/info
  wheel_base_mm = whl_base; //current wheelbase is 96 mm
  wheel_circumference_mm = whl_crfnc; //current wheel diameter is 188.5

  bearing = 0;
  x_pos_cm = 0;
  y_pos_cm = 0;

  left_wheel->rotations = 0;
  right_wheel->rotations = 0;

  left_wheel->relative_angle = get_angle_average(left_wheel);
  right_wheel->relative_angle = get_angle_average(right_wheel);

  left_wheel->previous_angle = left_wheel->relative_angle;
  right_wheel->previous_angle = right_wheel->relative_angle;

}

int get_x_position(){
  return x_pos_cm;
}

int get_y_position(){
  return y_pos_cm;
}

int get_wheel_angle(wheel * wheel){
  return wheel->rotations * 360 + wheel->relative_angle;
}

/* should probably do a check on degrees */
void step_forward(int degrees){ //angle = degrees
  int bias = 0;

  /* I'm basically going to spam this to avoid slipping */
  update_wheel_positions();
  
  int master_throttle = left_wheel->forwards_throttle;
  int slave_throttle = right_wheel->forwards_throttle;

  int left_wheel_final_angle = get_wheel_angle(left_wheel) + degrees;
  int right_wheel_final_angle = get_wheel_angle(right_wheel) + degrees;

  update_wheel_positions();
  update_wheel_positions();
  int left_wheel_start_angle = get_wheel_angle(left_wheel);
  int right_wheel_start_angle = get_wheel_angle(right_wheel);

  //only implementing positive angles for now
  motor_set_throttle(left_wheel, master_throttle);
  motor_set_throttle(right_wheel, slave_throttle);

  update_wheel_positions();

  //until we've reached out destination
  while(get_wheel_angle(left_wheel) < left_wheel_final_angle || get_wheel_angle(right_wheel) < right_wheel_final_angle){
    update_wheel_positions();
    int left_wheel_progress = get_wheel_angle(left_wheel) - left_wheel_start_angle;
    int right_wheel_progress = get_wheel_angle(right_wheel) - right_wheel_start_angle;
    update_wheel_positions();
    //if one wheel has advanced further than the other, make corrections
    if(left_wheel_progress < right_wheel_progress - 3){
      slave_throttle++;
      update_wheel_positions();
      //we don't want slave_throttle to get too high / low
      if(slave_throttle > right_wheel->forwards_throttle + 5){
	slave_throttle = right_wheel->forwards_throttle + 5;
	motor_set_throttle(right_wheel, slave_throttle);
	update_wheel_positions();
      }
    }else if(left_wheel_progress > right_wheel_progress + 3){
      slave_throttle--;
      update_wheel_positions();
      if(slave_throttle < right_wheel->forwards_throttle - 2){
	slave_throttle = right_wheel->forwards_throttle - 2;
	motor_set_throttle(right_wheel, slave_throttle);
	update_wheel_positions();
      }
    }

    update_wheel_positions();

    //if one wheel finishes before the other, stop it. This may be unnecessary
    if(get_wheel_angle(left_wheel) >= left_wheel_final_angle){
      motor_set_throttle(left_wheel, 0);
    }
    if(get_wheel_angle(right_wheel) >= right_wheel_final_angle){
      motor_set_throttle(right_wheel, 0);
    }
    bias += left_wheel_progress-right_wheel_progress;
    update_wheel_positions();
    printf("bias: %d, difference: %d, wheel1 position: %d, wheel2 positon: %d \n", bias, left_wheel_progress-right_wheel_progress,
	   left_wheel_progress, right_wheel_progress);
    update_wheel_positions();
    if(abs(bias) > 10000){
      while(1){
	printf("ERROR");
      } 
    }
  }

  //stop
  motor_set_throttle(left_wheel, 0);
  motor_set_throttle(right_wheel, 0);
}

/* likewise should probably do a check on degrees */
/* I really just don't want to touch my step_forward function lol */
void step_backward(int degrees){
  update_wheel_positions();
  int bias = 0;

  update_wheel_positions();
  int master_throttle = left_wheel->backwards_throttle;
  int slave_throttle = right_wheel->backwards_throttle;

  int left_wheel_final_angle = get_wheel_angle(left_wheel) + degrees;
  int right_wheel_final_angle = get_wheel_angle(right_wheel) + degrees;

  update_wheel_positions();
  update_wheel_positions();
  int left_wheel_start_angle = get_wheel_angle(left_wheel);
  int right_wheel_start_angle = get_wheel_angle(right_wheel);

  //only implementing positive angles for now
  motor_set_throttle(left_wheel, master_throttle);
  motor_set_throttle(right_wheel, slave_throttle);

  update_wheel_positions();

  //until we've reached out destination
  while(get_wheel_angle(left_wheel) > left_wheel_final_angle || get_wheel_angle(right_wheel) > right_wheel_final_angle){
    update_wheel_positions();
    int left_wheel_progress = -1*get_wheel_angle(left_wheel) + left_wheel_start_angle; //measured in
    int right_wheel_progress = -1*get_wheel_angle(right_wheel) + right_wheel_start_angle; //+ direction
    update_wheel_positions();

    //if one wheel has advanced further than the other, make corrections
    if(left_wheel_progress < right_wheel_progress - 3){
      slave_throttle--;
      update_wheel_positions();
      //we don't want slave_throttle to get too high / low
      if(slave_throttle > right_wheel->backwards_throttle - 2){
	slave_throttle = right_wheel->backwards_throttle - 2;
	motor_set_throttle(right_wheel, slave_throttle);
	update_wheel_positions();
      }
    }else if(left_wheel_progress > right_wheel_progress + 3){
      slave_throttle++;
      update_wheel_positions();
      if(slave_throttle < right_wheel->backwards_throttle + 2){
	slave_throttle = right_wheel->backwards_throttle + 2;
	motor_set_throttle(right_wheel, slave_throttle);
	update_wheel_positions();
      }
    }

    update_wheel_positions();
    //if one wheel finishes before the other, stop it. This may be unnecessary
    if(get_wheel_angle(left_wheel) <= left_wheel_final_angle){
      motor_set_throttle(left_wheel, 0);
    }
    if(get_wheel_angle(right_wheel) <= right_wheel_final_angle){
      motor_set_throttle(right_wheel, 0);
    }
    update_wheel_positions();
    bias += left_wheel_progress-right_wheel_progress;
    printf("bias: %d, difference: %d, wheel1 position: %d, wheel2 positon: %d \n", bias, left_wheel_progress-right_wheel_progress,
	   left_wheel_progress, right_wheel_progress);
    update_wheel_positions();

    if(abs(bias) > 10000){
      while(1){
	printf("ERROR");
      }
    }
  }

  //stop
  motor_set_throttle(left_wheel, 0);
  motor_set_throttle(right_wheel, 0);
}

void move_forward(int distance_in_cm){
  int angle = distance_in_cm * 10 * 360 / wheel_circumference_mm;
  if(angle > 0){
    step_forward(angle);
  }else{
    step_backward(angle);
  }
}

/* maybe I should write this differently
 * the idea is that wheel1 and wheel2 should stop at the same exact time every time.
 */
void move_forward_2(int distance_in_cm){
  int steps = distance_in_cm;
  if(steps < 0){
    steps = -1*steps;
  }

  //step 1 cm a bunch of times
  for(int i = 0; i < steps; i++){
    move_forward(distance_in_cm/steps); //+ or - 1 depending on sign of distance_in_cm
    timer_delay_ms(300); //change this delay time depending on trial and error
  }
}

void move_forward_3(int distance_in_cm){
  //only implement this if move_forward/move_forward_2 don't work
}

/* needs to rotate one wheel degrees * pi * wheelbase / wheel_circumference
 * and the other wheel the same amount in the opposite direciton
 *
 * May want to implement this in a different way than I currently am
 */
void turn(int degrees){
  int angle_to_move_wheels = degrees * wheel_base_mm * 314 / 100 / wheel_circumference_mm;

  int left_wheel_final_angle = get_wheel_angle(left_wheel) - angle_to_move_wheels;
  int right_wheel_final_angle = get_wheel_angle(right_wheel) + angle_to_move_wheels;

  if(degrees > 0){
    //set throttles
    motor_set_throttle(left_wheel, left_wheel->forwards_throttle); //may need to change signs
    motor_set_throttle(right_wheel, right_wheel->backwards_throttle); //need to sync f and b

    //move until we've acheived the desired position
    while(get_wheel_angle(left_wheel) >= left_wheel_final_angle || get_wheel_angle(right_wheel) <= right_wheel_final_angle){
      update_wheel_positions();
      if(get_wheel_angle(left_wheel) >= left_wheel_final_angle){
        motor_set_throttle(left_wheel, 0);
      }
      if(get_wheel_angle(right_wheel) <= right_wheel_final_angle){
        motor_set_throttle(right_wheel, 0);
      }
    }
    motor_set_throttle(left_wheel, 0);
    motor_set_throttle(right_wheel, 0);
  }else if(degrees < 0){
    motor_set_throttle(left_wheel, left_wheel->backwards_throttle);
    motor_set_throttle(right_wheel, right_wheel->forwards_throttle);

    while(get_wheel_angle(left_wheel) <= left_wheel_final_angle || get_wheel_angle(right_wheel) >= right_wheel_final_angle){
      update_wheel_positions();
      if(get_wheel_angle(left_wheel) <= left_wheel_final_angle){
        motor_set_throttle(left_wheel, 0);
      }
      if(get_wheel_angle(right_wheel) >= right_wheel_final_angle){
        motor_set_throttle(right_wheel, 0);
      }
    }
    motor_set_throttle(left_wheel, 0);
    motor_set_throttle(right_wheel, 0);
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

/* a test module */
void test_car_control_module(unsigned int input1, unsigned int input2, unsigned int output1,
                             unsigned int output2){

  printf("beginning car control test in 2s\n");
  timer_delay(2);
  
  car_control_module_init(input1, input2, output1, output2,96, 188); //estimated wheelbase/circumfrence  

  while(1){
    
    printf("move forward 20\n");
    move_forward(20);
    
    timer_delay(2);
    
    printf("wheel positions: %d, %d\n", get_wheel_angle(left_wheel), get_wheel_angle(right_wheel));
    
    printf("move forward -20\n");
    move_forward(-20);
    
    timer_delay(2);
    
    printf("wheel positions: %d, %d\n", get_wheel_angle(left_wheel), get_wheel_angle(right_wheel));
    
    printf("move forward_2, 20\n");
    move_forward_2(20);
    
    printf("wheel positions: %d, %d\n", get_wheel_angle(left_wheel), get_wheel_angle(right_wheel));
    
    printf("move forward_2, -20\n");
    move_forward_2(-20);
    
  }

  /*
  while(1){
    move_forward_2(20);
    move_forward_2(-20);
  }
  */

  //  step_forward(1000);
  /*
  while(1){
    update_wheel_positions();
    printf("wheel1 angle: %d, wheel 2 angle : %d\n", get_wheel_angle(left_wheel),
	   get_wheel_angle(right_wheel));
    timer_delay_ms(100); //this messes things up
  }
  */
  /*  step_forward_2(10000);*/

}

