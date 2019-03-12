#include "i2c.h"
#include "malloc.h"
#include "strings.h"
#include "printf.h"
#include "timer.h"

/* note: the line follower needs an external power supply: the pi doesn't provide enough. 
 * 
 * also: darker colors lead to smaller values => min will be the line, if there is just a 
 * single line
 */

/* an array which stores the line_follower_data 
 * an array of charcaters, not a string. 
 */
char * line_follower_data;

/* LF stands for Line Follower */
const unsigned int NUMBER_LF_READ_HEADS = 8;

const unsigned int DISTANCE_BETWEEN_SENSORS = 13; //approximately

/* is a wrong address the reason this isn't working? */
const unsigned int LINE_FOLLOWER_ADDRESS = 9;

static int MIDDLE_DISTANCE; 

/* communicates through I2C to update line follower's data */
static void update_line_follower_data(){
  char * raw_data = malloc(2 * NUMBER_LF_READ_HEADS);
  i2c_read(LINE_FOLLOWER_ADDRESS, raw_data, 16);

  /* every other byte is the data we're looking for */  
  for(int i = 0; i < NUMBER_LF_READ_HEADS; i++){
    line_follower_data[i] = raw_data[2*i];
  }

  free(raw_data);

  /* timer delay here? */
}

void line_follower_init(){
  i2c_init();
  
  line_follower_data = malloc (NUMBER_LF_READ_HEADS);
  line_follower_data = memset(line_follower_data, 0, NUMBER_LF_READ_HEADS);

  MIDDLE_DISTANCE = -3 * DISTANCE_BETWEEN_SENSORS; //this is the 4th sensor, indexing from 1.
}

int get_line_index(){
  //update our data
  //this call is a bit sketchy, because it may be time sensitive and we don't want to be calling it
  //too much. Perhaps it's better to allow the user to chose when to call it...
  update_line_follower_data();

  //search for the minimum data value (or maybe the max?)
  int min = line_follower_data[0];
  int index = 0;
  for(int i = 1; i < NUMBER_LF_READ_HEADS; i++){
    if(line_follower_data[i] < min){
      min = line_follower_data[i];
      index = i;
    }
  }
  return index;
}

int get_line_position(){
  int line_index = get_line_index();
  if(line_index >= 0){
    return MIDDLE_DISTANCE + line_index * DISTANCE_BETWEEN_SENSORS;
  }else{
    return -1000000; //obviously invalid value
  }
}

//maybe I'll add this in the future
int set_line_threshold(); //no longer needed, I guess, since I just get the extreme value. 

//might add functionality for in the future
int get_line_threshold();

int get_line_follower_value(int sensor_index){
  return line_follower_data[sensor_index];
}

int get_distance_between_sensors(){
  return DISTANCE_BETWEEN_SENSORS;
}

void line_follower_test(){
  printf("Beginning line follower test \n");
  line_follower_init();
  
  while(1){
    printf("line is at index %d\n", get_line_index()); 
    for(int i = 0; i < NUMBER_LF_READ_HEADS; i++){
      unsigned int data = (unsigned int) line_follower_data[i];
      printf("RH%d: %d;", i, data);
    }
    printf("\n");
    timer_delay_ms(300);
  }
}
