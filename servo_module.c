#include "pwm_output_module.h"
#include "timer.h"
#include "malloc.h"
#include "printf.h"

const unsigned int DEFAULT_THRESHOLD = 1500;

struct servo_motor{
  unsigned int pin;
  unsigned int current_threshold;
  unsigned int current_angle;
  unsigned int max_threshold;
  unsigned int min_threshold;
  unsigned int threshold_range;
}

void servo_init();

servo *servo_new(unsigned int pin){
  servo * srvo = malloc(sizeof(struct servo_motor));
  srvo->pin = pin;
  srvo->current_threshold = DEFAULT_THRESHOLD;
  pwm_add_output(pin, srvo->current_threshold);
}

void servo_setup(servo *servo);

/* check that we have valid values? */
void servo_auto_setup(servo *servo, unsigned int min_threshold,
                      unsigned int max_threshold){
  servo->threshold_range = max_threshold - min_threshold;
  unsigned int center_threshold = servo->threshold_range / 2 + min_threshold;
  servo->current_threshold = center_threshold;
  servo->max_threshold = max_threshold;
  servo->min_threshold = min_threshold;
  servo->current_angle = threshold_to_angle(servo, servo->current_threshold);
  servo_write_angle(servo, servo->current_angle);
}

/* add in checks */
int servo_write_angle(servo *servo, unsigned int angle){
  servo->current_angle = angle;
  servo->current_threshold = angle_to_threshold(servo, angle);
  return servo_write_threshold(servo, servo->current_threshold); /* a lot of jumping through hoops. */
  /* could simplify by just calling pwm_output_module directly here */
}

/* add in checks */
int servo_write_threshold(servo *servo, unsigned int threshold){
  servo->current_threshold = threshold;
  servo->current_angle = threshold_to_angle(servo, threshold);
  pwm_change_threshold(servo->pin, servo->current_threshold);
  return 1;
}

unsigned int get_servo_position(servo *servo){
  return servo->current_angle;
}

unsigned int get_servo_threshold(servo *servo){
  return servo->current_threshold;
}

/*should do a check to make sure threshold > servo->min_threshold? */
static unsigned int threshold_to_angle(servo * servo, unsigned int threshold){
  unsigned int relative_threshold = threshold - servo->min_threshold;
  return relative_threshold * 180 / servo->threshold_range;
}

static unsigned int angle_to_threshold(servo * servo, unsigned int angle){
  return servo->min_threshold + servo->threshold_range * angle / 180;
}
