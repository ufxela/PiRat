# Pi Rat

**Team members:** Alex Fu

**Description:**
PiRat is loosely inspired by Tolman and Honzik’s psychology experiments (1930) 
in which a live rat was placed in a maze and given the task to find a reward 
(stereotypically cheese) within the maze. PiRat will model the rat, and attempt 
to solve a maze given little prior information about the structure of the maze.

PiRat will have two primary abilities: the first is the ability to move around.
The second is to sense its environment. To move around, the Pi Rat will utilize
continuous rotation servos and a rotary encoder. To sense its environment,
the Pi Rat will use a dinstance sensor (either ultrasonic or time of flight). The
Pi Rat will also use a line sensor to keep it's position/orientation correct, so
that it does not stray off the path and run into the sides of the maze. 

With these two abilities, the PiRat can use a depth first search path finder 
algorithm to navigate through a maze, and find a solution. At the same time, 
the PiRat will keep an internal model of the maze, updating the model with each 
measurement it takes with it’s ultrasonic distance/time of flight sensor. 

**Hardware Required:** 
besides a Raspberry Pi,

* 2x parallax feedback 360 servo motors
* 1x time of flight sensor / ultrasonic distance sensor
* 1x 9g servo
* 1x Line Follower Module
* 3D printed chassis
* 2 wheels, good grip (no slipping!)
* free wheel / low friction knob
* cardboard / foamboard to build maze

**Funds Requested:**

* Optical Time of Flight Sensor: 
    * https://www.amazon.com/Adafruit-VL6180X-Flight-Distance-Ranging/dp/B01N0ODI3Q/ref=sr_1_5?keywords=time+of+flight+sensor&qid=1551840755&s=gateway&sr=8-5
    * $13.99
* 9g servo
    * https://www.amazon.com/DaFuRui-Compatible-Helicopter-Airplane-Controls/dp/B07H3S2Y8Q/ref=sr_1_25?keywords=9g+servo&qid=1551840768&s=gateway&sr=8-25
    * $7.89
* Line Follower Module 8 Channel Infrared 
    * $12.99
    * https://www.amazon.com/SunFounder-Follower-Infrared-Detection-STM8S105C4/dp/B01HB5YVUG/ref=asc_df_B01HB5YVUG/?tag=hyprod-20&linkCode=df0&hvadid=309791895009&hvpos=1o2&hvnetw=g&hvrand=15345366752874283795&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9031970&hvtargid=aud-643330155750:pla-300463092920&psc=1&tag=&ref=&adgrpid=70160276868&hvpone=&hvptwo=&hvadid=309791895009&hvpos=1o2&hvnetw=g&hvrand=15345366752874283795&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9031970&hvtargid=aud-643330155750:pla-300463092920
* **Total**
    * $34.67 (but I have a $10 amazon credit!)

**Task Breakdown**

*Software:*

1. Write module to communicate to servo through PWM
1. Write module to communicate to continuous rotation servo through PWM
1. Write module to read PWM communications from Parallax Feedback 360 servos
1. Either:
   1. Communicate through I2C to optical time of flight sensor
   1. Read/write through GPIO to communicate with ultrasonic distance sensor
1. Write module to communicate via I2C with the line follower module
   1. Will use libpiextra i2c modules
1. Module to move support movement of rat and keep track of position.
   1. Will exist a forward movement function which adjusts movement based on line follower's output 
1. Code to use all modules to sense environment and move the rat in response based on
   where it wants to go. 
   1. Internal data structure to represent maze
   1. Algorithm to solve maze
   1. Needs to keep orientation correct with line follower sensor
   1. Way to determine when solution is discovered

*Hardware:*

1. Model and 3D print chassis for rat
1. Mount time of flight sensor / ultrasonic distance sensor
1. Find / make wheels which have really good grip
1. Make maze, ideally with configurability. (slot and fit walls)

**Goals for Next Week**
1. Software steps 1-6
1. Hardware steps 1-3
1. Test movement of Pi Rat and see how accurately it can hold its position
1. Test accuracy of ultrasonic sensor

**Reference and Resources**
*another document that goes into more detail about my project*
https://docs.google.com/document/d/10EH3p42lqzU2tbl3T3y5yWcG4yZZ02CRSOTUqFnZRl0/edit?usp=sharing

*line follower*
http://wiki.sunfounder.cc/index.php?title=Line_Follower_Module
https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/i2c.c

*ultrasonic sensor*
https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
https://github.com/cs107e/cs107e.github.io/blob/master/lectures/Sensors/code/sonar/sonar.c