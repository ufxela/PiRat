#include "cr_servo_module.h"
#include "pwm_output_module.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "malloc.h"

const unsigned int CR_DEFAULT_THRESHOLD = 1500;  
const unsigned int MAX_CR_SERVO_THRESHOLD = 2000;
const unsigned int MIN_CR_SERVO_THRESHOLD = 1000;

struct cr_servo_motor{
  unsigned int pin;
  unsigned int current_threshold;
  int current_throttle; //goes from -100 to 100
  unsigned int max_threshold;
  unsigned int min_threshold;
  unsigned int threshold_range;
  unsigned int center_threshold; //everything should be symmetric around this value. 
};

/* should do same checks as in servo module */
static int threshold_to_throttle(cr_servo *cr_servo, unsigned int threshold){
  int relative_threshold = threshold - cr_servo->min_threshold;
  return (relative_threshold - cr_servo->center_threshold) * 100 / cr_servo->threshold_range;
}

static unsigned int throttle_to_threshold(cr_servo *cr_servo, int throttle){
  return ((100 + throttle) * cr_servo->threshold_range / 200) + cr_servo->min_threshold; 
}

void cr_servo_init(){
  pwm_output_init(); //is it bad that I call this like 50 times?
}

cr_servo *cr_servo_new(unsigned int pin){
  cr_servo * cr_srvo = malloc(sizeof(struct cr_servo_motor));
  cr_srvo->pin = pin;

  /* default cr_servo setup */
  cr_srvo->current_threshold = CR_DEFAULT_THRESHOLD;
  cr_srvo->center_threshold = CR_DEFAULT_THRESHOLD;
  cr_srvo->max_threshold = 1600;
  cr_srvo->min_threshold = 1400;
  cr_srvo->threshold_range = 200;
  cr_srvo->current_throttle = threshold_to_throttle(cr_srvo, cr_srvo->current_threshold);

  pwm_add_output(pin, cr_srvo->current_threshold);

  return cr_srvo;
}

/* should probably functionally decompose, but I'm also lazy */
void cr_servo_setup(cr_servo *cr_servo){
  printf("Beginning continuous rotation servo setup\n");

  /* first find servo center */
  printf("First, let's find the center of our throttle\n");
  unsigned int min_center_threshold = CR_DEFAULT_THRESHOLD;
  cr_servo->current_threshold = min_center_threshold;
  cr_servo_write_threshold(cr_servo);

  printf("Is the servo moving? (y/n)\n");

  char user_response = uart_getchar();

  /*loop until we've found the min_center_threshold */
  while(user_response != 'y'){
    min_center_threshold = min_center_threshold - 1;
    cr_servo->current_threshold = min_center_threshold;
    printf("setting threshold to %d\n", min_center_threshold);
    cr_servo_write_threshold(cr_servo);
    printf("Is the servo moving? (y/n)\n");
    user_response = uart_getchar();
  }

  printf("ok now, again: \n");

  unsigned int max_center_threshold = CR_DEFAULT_THRESHOLD;
  cr_servo->current_threshold = max_center_threshold;
  cr_servo_write_threshold(cr_servo);

  printf("Is the servo moving? (y/n)\n");
  user_response = uart_getchar();

  /* loop until we've found the max_center_threshold */
  while(user_response != 'y'){
    max_center_threshold = max_center_threshold + 1;
    cr_servo->current_threshold = max_center_threshold;
    printf("setting threshold to %d\n", max_center_threshold);
    cr_servo_write_threshold(cr_servo);
    printf("Is the servo moving? (y/n)\n");
    user_response = uart_getchar();
  }

  cr_servo->center_threshold = (max_center_threshold + min_center_threshold)/2;

  /* now find max / min throttle (since symmetric, only need to find one */
  printf("Now we will find the max/min throttles \n");

  unsigned int min_threshold = min_center_threshold;
  cr_servo->current_threshold = min_threshold;
  cr_servo_write_threshold(cr_servo);

  user_response = 'n';

  while(user_response != 'y'){
    min_threshold = min_center_threshold - 10;
    if(min_threshold <= MIN_CR_SERVO_THRESHOLD){
      min_threshold = MIN_CR_SERVO_THRESHOLD;
      user_response = 'y';
    }
    cr_servo->current_threshold = min_threshold;
    cr_servo_write_threshold(cr_servo);
    printf("setting threshold to %d \n", min_threshold);
    cr_servo_write_threshold(cr_servo);
    printf("have we reached the max/min throttle speed?\n");
    user_response = uart_getchar();
  }

  /* set cr_servo values */
  cr_servo->min_threshold = min_threshold;
  unsigned int threshold_range = (cr_servo->center_threshold - min_threshold)*2;
  cr_servo->threshold_range = threshold_range;
  cr_servo->max_threshold = cr_servo->center_threshold + threshold_range/2;
  cr_servo_go_to_throttle(cr_servo, 0);

  /* print results */
  printf("For reference: \n");
  printf("Center threshold: %d\n", cr_servo->center_threshold);
  printf("Min threshold: %d\n", cr_servo->min_threshold);
  printf("Max threshold: %d\n", cr_servo->max_threshold);
}

void cr_servo_auto_setup(cr_servo *cr_servo, unsigned int center_threshold, unsigned int min_threshold,
			 unsigned int max_threshold){
  cr_servo->center_threshold = center_threshold;
  cr_servo->min_threshold = min_threshold;
  cr_servo->max_threshold = max_threshold;
  cr_servo->threshold_range = max_threshold - min_threshold;
  cr_servo_go_to_throttle(cr_servo, 0);
}

/* sets fields internally and also calls to write threshold to set the throttle */
int cr_servo_go_to_throttle(cr_servo *cr_servo, int throttle){
  unsigned int threshold = throttle_to_threshold(cr_servo, throttle);
  cr_servo->current_throttle = throttle;
  cr_servo->current_threshold = threshold;
  return cr_servo_write_threshold(cr_servo);
  /* same problem with this implementation as with servo_go_to_angle */
}

/* should probably do error checking */
int cr_servo_write_threshold(cr_servo *cr_servo){
  pwm_change_threshold(cr_servo->pin, cr_servo->current_threshold);
  return 1;
}

int get_cr_servo_throttle(cr_servo *cr_servo){
  return cr_servo->current_throttle;
}

unsigned int get_cr_servo_threshold(cr_servo *cr_servo){
  return cr_servo->current_threshold;
}
