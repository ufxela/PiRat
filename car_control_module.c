#include "car_control_module.h"
#include "pwm_input_module.h"
#include "cr_servo_module.h"
#include "timer.h"
#include "math.h"
#include "printf.h"
#include "malloc.h"

/* internally data concerning information about the car */
static  int wheel_base_mm; // distance between wheels, measured in  mm
static  int wheel_circumference_mm; //measured in mm

/* shimmy constants, configures shimmy unit to be equal to distance betwee
 * line sensors
 * 
 * These values need to be calibrated for each car
 */
static const unsigned int SHIMMY_ANGLE = 15;
static const unsigned int SHIMMY_FORWARD_DISTANCE_CM = 4;
//move back (move back cos(SHIMMY ANGLE) * SHIMMY_FORWARD_DISTANCE
static const unsigned int SHIMMY_BACKWARDS_DISTANCE_CM = 4;

/* for updating angles */
/* I don't think I use this */
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

/* a helper to filter through angles */
static int get_angle(wheel * wheel){
  int raw = pwm_input_get_angle(wheel->input_pin);
  while(raw >= 400){ //make sure it's not the invalid 10000 data point                                  
    raw = pwm_input_get_angle(wheel->input_pin);
  }
  return raw;
}

/* internal helper functions which needs to be called very often, whenever there is wheel movement.
 * Ideally, this would be called on a tiemr interrupt schedule, once every 20 ms maybe, but
 * I am not doing that for now...
 *
 * Wheel1's (left wheel) angles are negated, so that it's positive direciton is forwards                 
 *
 * does a small check to make sure the angles are valid
 */
static void update_wheel_positions(){
  left_wheel->previous_angle = left_wheel->relative_angle;
  left_wheel->relative_angle = -1*get_angle(left_wheel);

  if(left_wheel->relative_angle < -1*350 && left_wheel->previous_angle > -1*10){
    left_wheel->rotations++;
  }else if(left_wheel->relative_angle > -1*10 && left_wheel->previous_angle < -1*350){
    left_wheel->rotations--;
  }

  right_wheel->previous_angle = right_wheel->relative_angle;
  right_wheel->relative_angle = get_angle(right_wheel);

  if(right_wheel->relative_angle > 350 && right_wheel->previous_angle < 10){
    right_wheel->rotations--;
  }else if(right_wheel->relative_angle < 10 && right_wheel->previous_angle > 350){
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
  right_wheel->forwards_throttle = -24; //these values need to be calibrated for each car

  left_wheel->backwards_throttle = -23; //prevous -21, worked to go backwards
  right_wheel->backwards_throttle = 23; //previous 20

  //update internal positioning/info
  wheel_base_mm = whl_base; //current wheelbase is 96 mm
  wheel_circumference_mm = whl_crfnc; //current wheel diameter is 188.5

  left_wheel->rotations = 0;
  right_wheel->rotations = 0;

  timer_delay_ms(100);

  left_wheel->relative_angle = get_angle(left_wheel); //used get_angle_average before, screwed thigns up
  right_wheel->relative_angle = get_angle(right_wheel);

  left_wheel->previous_angle = left_wheel->relative_angle;
  right_wheel->previous_angle = right_wheel->relative_angle;
}

int get_wheel_angle(wheel * wheel){
  return wheel->rotations * 360 + wheel->relative_angle;
}


/* should probably check if degrees is positive */
void step_forward(int degrees){ 
  /* variable to see if one wheel is favored over the other */
  /* doesn't do anything for now except record data */
  int bias = 0;

  update_wheel_positions();
  
  /* master throttle doesn't change, slave_throttle adjusts based on wheel positions */
  int master_throttle = left_wheel->forwards_throttle - 2; //trim
  int slave_throttle = right_wheel->forwards_throttle;

  /* the angles where we want to end up */
  int left_wheel_final_angle = get_wheel_angle(left_wheel) + degrees;
  int right_wheel_final_angle = get_wheel_angle(right_wheel) + degrees;

  /* the start angles, for keeping track of progress */
  int left_wheel_start_angle = get_wheel_angle(left_wheel);
  int right_wheel_start_angle = get_wheel_angle(right_wheel);

  /* start movement */
  motor_set_throttle(left_wheel, master_throttle);
  motor_set_throttle(right_wheel, slave_throttle);

  update_wheel_positions();

  /* loop until we've reached the destination */
  while(get_wheel_angle(left_wheel) < left_wheel_final_angle || get_wheel_angle(right_wheel) < right_wheel_final_angle){
    /* get the current progress */
    int left_wheel_progress = get_wheel_angle(left_wheel) - left_wheel_start_angle;
    int right_wheel_progress = get_wheel_angle(right_wheel) - right_wheel_start_angle;

    /* if one wheel has advanced further than the other, change throttles to combat this */
    if(left_wheel_progress < right_wheel_progress - 3){
      slave_throttle++;
      /* to bound the slave throttle */
      if(slave_throttle > right_wheel->forwards_throttle + 5){
	slave_throttle = right_wheel->forwards_throttle + 5;
	motor_set_throttle(right_wheel, slave_throttle);
      }
    }else if(left_wheel_progress > right_wheel_progress + 3){
      slave_throttle--;
      if(slave_throttle < right_wheel->forwards_throttle - 2){
	slave_throttle = right_wheel->forwards_throttle - 2;
	motor_set_throttle(right_wheel, slave_throttle);
      }
    }

    update_wheel_positions();

    /* if one wheel finishes before the other, stop it. */
    if(get_wheel_angle(left_wheel) >= left_wheel_final_angle){
      motor_set_throttle(left_wheel, 0);
    }
    if(get_wheel_angle(right_wheel) >= right_wheel_final_angle){
      motor_set_throttle(right_wheel, 0);
    }

    /* print some debugging information to console */
    bias += left_wheel_progress-right_wheel_progress;
    //    printf("bias: %d, difference: %d, wheel1 position: %d, wheel2 positon: %d \n", bias, left_wheel_progress-right_wheel_progress,
    //	   left_wheel_progress, right_wheel_progress);
  }
  /* once the loop finishes, stop */
  motor_set_throttle(left_wheel, 0);
  motor_set_throttle(right_wheel, 0);
}

/* likewise should probably do a check on degrees */
/* essentially same function as step_forward, but I didn't want to touch the 
 * working step_forward function because it seemed fragile at the time 
 */
void step_backward(int degrees){
  update_wheel_positions();

  int bias = 0;

  int master_throttle = left_wheel->backwards_throttle;
  int slave_throttle = right_wheel->backwards_throttle;

  int left_wheel_final_angle = get_wheel_angle(left_wheel) + degrees;
  int right_wheel_final_angle = get_wheel_angle(right_wheel) + degrees;

  int left_wheel_start_angle = get_wheel_angle(left_wheel);
  int right_wheel_start_angle = get_wheel_angle(right_wheel);

  motor_set_throttle(left_wheel, master_throttle);
  motor_set_throttle(right_wheel, slave_throttle);

  update_wheel_positions();

  while(get_wheel_angle(left_wheel) > left_wheel_final_angle || get_wheel_angle(right_wheel) > right_wheel_final_angle){
    int left_wheel_progress = -1*get_wheel_angle(left_wheel) + left_wheel_start_angle;
    int right_wheel_progress = -1*get_wheel_angle(right_wheel) + right_wheel_start_angle;

    if(left_wheel_progress < right_wheel_progress - 3){
      slave_throttle--;
      if(slave_throttle > right_wheel->backwards_throttle - 2){
	slave_throttle = right_wheel->backwards_throttle - 2;
	motor_set_throttle(right_wheel, slave_throttle);
      }
    }else if(left_wheel_progress > right_wheel_progress + 3){
      slave_throttle++;
      if(slave_throttle < right_wheel->backwards_throttle + 2){
	slave_throttle = right_wheel->backwards_throttle + 2;
	motor_set_throttle(right_wheel, slave_throttle);
      }
    }

    update_wheel_positions();

    if(get_wheel_angle(left_wheel) <= left_wheel_final_angle){
      motor_set_throttle(left_wheel, 0);
    }
    if(get_wheel_angle(right_wheel) <= right_wheel_final_angle){
      motor_set_throttle(right_wheel, 0);
    }

    bias += left_wheel_progress-right_wheel_progress;
    //    printf("bias: %d, difference: %d, wheel1 position: %d, wheel2 positon: %d \n", bias, left_wheel_progress-right_wheel_progress,
    //	   left_wheel_progress, right_wheel_progress);
  }

  motor_set_throttle(left_wheel, 0);
  motor_set_throttle(right_wheel, 0);
}

/* I should probably have a more desrciptive name for this, like move laterally */
void move_forward(int distance_in_cm){
  /* calculate angle to move */
  int angle = distance_in_cm * 10 * 360 / wheel_circumference_mm;
  if(angle > 0){
    step_forward(angle);
  }else{
    step_backward(angle);
  }
}

/* moves forward, in steps */
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

/* needs to rotate one wheel degrees * pi * wheelbase / wheel_circumference
 * and the other wheel the same amount in the opposite direciton
 */
void turn(int degrees){
  update_wheel_positions();

  /* get angles to move wheels */ 
  int angle_to_move_wheels = degrees * wheel_base_mm * 314 / 100 / wheel_circumference_mm;
  int left_wheel_final_angle = get_wheel_angle(left_wheel) + angle_to_move_wheels;
  int right_wheel_final_angle = get_wheel_angle(right_wheel) - angle_to_move_wheels;

  /*left wheel moves forwards, right wheel moves backwards */
  if(degrees > 0){
    //set throttles
    motor_set_throttle(left_wheel, left_wheel->forwards_throttle);
    motor_set_throttle(right_wheel, right_wheel->backwards_throttle); //need to sync f and b

    update_wheel_positions();

    //move until we've acheived the desired position
    while(get_wheel_angle(left_wheel) <= left_wheel_final_angle || get_wheel_angle(right_wheel) >= right_wheel_final_angle){
      /* if one wheel finishes before the other, stop it */
      if(get_wheel_angle(left_wheel) >= left_wheel_final_angle){
        motor_set_throttle(left_wheel, 0);
      }
      if(get_wheel_angle(right_wheel) <= right_wheel_final_angle){
        motor_set_throttle(right_wheel, 0);
      }
      update_wheel_positions();
    }
    /* stop wheels at end */
    motor_set_throttle(left_wheel, 0);
    motor_set_throttle(right_wheel, 0);
  }else if(degrees < 0){ /* left wheel backwards right wheel forwards */
    motor_set_throttle(left_wheel, left_wheel->backwards_throttle);
    motor_set_throttle(right_wheel, right_wheel->forwards_throttle);

    update_wheel_positions();

    while(get_wheel_angle(left_wheel) >= left_wheel_final_angle || get_wheel_angle(right_wheel) <= right_wheel_final_angle){
      if(get_wheel_angle(left_wheel) <= left_wheel_final_angle){
        motor_set_throttle(left_wheel, 0);
      }
      if(get_wheel_angle(right_wheel) >= right_wheel_final_angle){
        motor_set_throttle(right_wheel, 0);
      }
      update_wheel_positions();
    }

    motor_set_throttle(left_wheel, 0);
    motor_set_throttle(right_wheel, 0);
  }
}

/* shimmy left one unit */
/* values of rotation and forward movement need to be adjusted to give proper behavior */
void shimmy_left(){
  //rotate left
  turn(SHIMMY_ANGLE); 

  //move forward
  move_forward(SHIMMY_FORWARD_DISTANCE_CM);

  //rotate right
  turn(-SHIMMY_ANGLE - 1);

  //move back (move back cos(SHIMMY ANGLE) * SHIMMY_FORWARD_DISTANCE
  move_forward(-SHIMMY_BACKWARDS_DISTANCE_CM);
}

/*I think my left and right are mixed up lol */
/* shimmy right 1 unit */
void shimmy_right(){
  turn(-SHIMMY_ANGLE); 
  //should probably check out throttles later
  move_forward(SHIMMY_FORWARD_DISTANCE_CM);
  turn(SHIMMY_ANGLE - 3); //comphensate for over turning
  move_forward(-SHIMMY_BACKWARDS_DISTANCE_CM);
}

int get_wheel_base(){
  return wheel_base_mm;
}

int get_wheel_circumference(){
  return wheel_circumference_mm;
}
/* a test module */
void test_car_control_module(unsigned int input1, unsigned int input2, unsigned int output1,
                             unsigned int output2){

  printf("beginning car control test in 5s\n");
  timer_delay(5);

  car_control_module_init(input1, input2, output1, output2, 98, 197); //estimated wheelbase/circumfrence

  /* for calibrating turns */
  while(1){
        turn(90);
    //    turn(-90);
    timer_delay(2);
  }


  /* for calibrating distances */
  /*
  move_forward(15);
  timer_delay(5);
  */
  /* a test to go in a L shaped path continually, with a bit of inconsequential shimmying */
  while(1){
    shimmy_left();
    timer_delay_ms(500);
    shimmy_right();
    timer_delay_ms(500);
    /*
    move_forward(20);
    timer_delay_ms(500);
    turn(90);
    timer_delay_ms(500);
    move_forward(20);
    timer_delay_ms(500);
    turn(180);
    timer_delay_ms(500);
    move_forward(20);
    timer_delay_ms(500);
    turn(-90);
    timer_delay_ms(500);
    move_forward(20);
    timer_delay_ms(500);
    turn(180);
    timer_delay_ms(500);
    */
    }
}

