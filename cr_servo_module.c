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
  int throttle; //goes from -100 to 100
  unsigned int max_threshold;
  unsigned int min_threshold;
  unsigned int threshold_range;
  unsigned int center_threshold; //everything should be symmetric around this value. 
};

/* should do same checks as in servo module */
static int threshold_to_throttle(cr_servo *cr_servo, unsigned int threshold){
  int relative_threshold = threshold - servo->min_threshold;
  return (relative_threshold - cr_servo->center_threshold) * 100 / cr_servo->threshold_range;
}

static unsigned int throttle_to_threshold(cr_servo *cr_servo, int throttle){
  return ((100 + throttle) * cr_servo->threshold_range / 200) + cr_servo->min_threshold; 

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
  srvo->throttle = threshold_to_throttle(cr_srvo, cr_srvo->current_threshold);

  pwm_add_output(pin, cr_srvo->current_threshold);

  return cr_srvo;


void cr_servo_setup(cr_servo *cr_servo);

void cr_servo_auto_setup(cr_servo *cr_servo, unsigned int center_threshold, unsigned int min_threshold,
			 unsigned int max_threshold);

int cr_servo_write_throttle(cr_servo *cr_servo, unsigned int angle);

int cr_servo_write_threshold(cr_servo *cr_servo, unsigned int threshold);

unsigned int get_cr_servo_throttle(cr_servo *cr_servo);

unsigned int get_cr_servo_threshold(cr_servo *cr_servo);
