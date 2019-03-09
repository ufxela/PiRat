#include "servo_module.h"
#include "pwm_output_module.h"
#include "malloc.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"

const unsigned int DEFAULT_THRESHOLD = 1500;   //settings which gave full motion on my servo:
const unsigned int MAX_SERVO_THRESHOLD = 2000; //2400
const unsigned int MIN_SERVO_THRESHOLD = 1000; //500

struct servo_motor{
  unsigned int pin;
  unsigned int current_threshold;
  unsigned int current_angle;
  unsigned int max_threshold;
  unsigned int min_threshold;
  unsigned int threshold_range;
};


/*should do a check to make sure threshold > servo->min_threshold? */
static unsigned int threshold_to_angle(servo * servo, unsigned int threshold){
  unsigned int relative_threshold = threshold - servo->min_threshold;
  return relative_threshold * 180 / servo->threshold_range;
}

static unsigned int angle_to_threshold(servo * servo, unsigned int angle){
  return servo->min_threshold + servo->threshold_range * angle / 180;
}

void servo_module_init(){
  pwm_output_init(); //is it bad that I call this like 50 times?
}

servo *servo_new(unsigned int pin){
  servo * srvo = malloc(sizeof(struct servo_motor));
  srvo->pin = pin;

  /* default servo setup */
  srvo->current_threshold = DEFAULT_THRESHOLD;
  srvo->max_threshold = 1600;
  srvo->min_threshold = 1400;
  srvo->threshold_range = 200;
  srvo->current_angle = threshold_to_angle(srvo, srvo->current_threshold);

  pwm_add_output(pin, srvo->current_threshold);

  return srvo;
}

/* this can probably be functionally decomposed, but I am also lazy */
void servo_setup(servo *servo){
  printf("let's find the 0 degree mark.\n");

  servo->current_threshold = servo->min_threshold;
  servo_write_threshold(servo);  

  printf("are we there yet? (Y/N)");
  printf("before uart");
  char user_response = 'N';
  //  user_response = uart_getchar();

  unsigned int new_threshold = servo->current_threshold;
  
  /* while user doesn't think we're at zero degrees, move towards zero degrees */
  while(user_response != 'Y'){
    new_threshold = new_threshold - 10;
    if(new_threshold <= MIN_SERVO_THRESHOLD){
      new_threshold = MIN_SERVO_THRESHOLD;
      user_response = 'Y';
    }
    servo->current_threshold = new_threshold;
    printf("setting threshold to %d \n", new_threshold);
    
    servo_write_threshold(servo);
    
    printf("are we at the zero degree mark? (Y/N)\n");
    //user_response = uart_getchar();
    timer_delay_ms(100);
  }

  /* update servo info */
  servo->min_threshold = new_threshold;
  
  /* now find max angle */
  printf("\nnow, let's go for the 180 mark \n");

  servo->current_threshold = servo->max_threshold;
  servo_write_threshold(servo); 

  printf("are we there yet? (Y/N)");
  //  user_response = uart_getchar();

  new_threshold = servo->current_threshold;

  user_response = 'n';

  /* while user doesn't think we're at 180 degrees, move towards 180 degrees */
  while(user_response != 'Y'){
    new_threshold = new_threshold + 10;
    if(new_threshold >= MAX_SERVO_THRESHOLD){
      new_threshold = MAX_SERVO_THRESHOLD;
      user_response = 'Y';
    }
    servo->current_threshold = new_threshold;
    printf("setting threshold to %d \n", new_threshold);

    servo_write_threshold(servo);

    printf("are we at the 180 degree mark? (Y/N)\n");
    //user_response = uart_getchar();
    timer_delay_ms(100);
  }

  /* update servo info */
  servo->max_threshold = new_threshold;

  /* now, complete the setup */
  printf("setup complete, going to 90 degrees \n");
  servo->threshold_range = servo->max_threshold - servo->min_threshold;
  servo->current_threshold = angle_to_threshold(servo, 90);
  servo->current_angle = 90;
  printf("For reference, here's your servo setup info: \n");
  printf("Max_threshold: %d\n", servo->max_threshold);
  printf("Min_threshold: %d\n", servo->min_threshold);
}
/* check that we have valid values? */
void servo_auto_setup(servo *servo, unsigned int min_threshold, unsigned int max_threshold){
  servo->threshold_range = max_threshold - min_threshold;
  servo->max_threshold = max_threshold;
  servo->min_threshold = min_threshold;
  servo_go_to_angle(servo, 90);
}

/* add in checks */
int servo_write_threshold(servo *servo){
  pwm_change_threshold(servo->pin, servo->current_threshold);
  return 1;
}

/* add in checks */
int servo_go_to_angle(servo *servo, unsigned int angle){
  servo->current_angle = angle;
  servo->current_threshold = angle_to_threshold(servo, angle);
  return servo_write_threshold(servo);
  /* ok this is terrible ordering: what if servo_write_threshold fails. Then the angles will be off 
     still, as you set them before you knew if it failed 
  */
}

unsigned int get_servo_position(servo *servo){
  return servo->current_angle;
}

unsigned int get_servo_threshold(servo *servo){
  return servo->current_threshold;
}
