#include "i2c.h"
#include "malloc.h"
#include "strings.h"

/* an array which stores the line_follower_data */
int * line_follower_data;

/* LF stands for Line Follower */
const unsigned int NUMBER_LF_READ_HEADS = 8;

const unsigned int sensor_distance_in_mm = 13; //approximately

void line_follower_init(){
  i2c_init();
  
  line_follower_data = malloc (4 * NUMBER_LF_READ_HEADS);
  line_follower_data = memset(line_follower_dataa, 0, 4 * NUMBER_LF_READ_HEADS);
}



int get_line_position(){

}

int set_line_threshold();

int get_line_threshold();

int get_line_follower_value(int sensor_index);

int get_distance_between_sensors();

void line_follower_test(){

}
