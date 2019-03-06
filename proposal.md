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
the Pi Rat will use a time of flight sensor (either ultrasonic or optical).

With these two abilities, the PiRat can use a depth first search path finder 
algorithm to navigate through a maze, and find a solution. At the same time, 
the PiRat will keep an internal model of the maze, updating the model with each 
measurement it takes with it’s time of flight sensor. 

**Hardware Required:** 
besides a Raspberry Pi,

* 2x parallax feedback 360 servo motors
* 1x time of flight sensor (either optical or ultrasonic)
* 1x 9g servo
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
* *Total*
    * $21.78

**Task Breakdown**

*Software:*

1. Write module to communicate to servo through PWM
1. Write module to communicate to continuous rotation servo through PWM
1. Write module to read PWM communications from Parallax Feedback 360 servos
1. Either:
   1. Communicate through I2C to optical time of flight sensor
   1. Read/write through GPIO to communicate with ultrasonic time of flight sensor
1. Module to move support movement of rat and keep track of position. 
1. Code to use all modules to sense environment and move the rat in response based on
   where it wants to go. 
   1. Internal data structure to represent maze
   1. Algorithm to solve maze
   1. Way to determine when solution is discovered

*Hardware:*

1. Model and 3D print chassis for rat
1. Mount time of flight sensor
1. Find / make wheels which have really good grip
1. Make maze, ideally with configurability. (slot and fit walls)

**Goals for Next Week**

 
Here is a suggested outline for your proposal. You don't need to follow this
outline, but it may help you focus your project.

* Project title
* Team members
* Description (1 pp describing the basic idea)
* Hardware required (itemize what hardware, if any, you need)
* Funds requested (the amount you will want to be reimbursed, remember you have
  a $20 budget per person)
* Major task breakdown and who is responsible for each task
* Goals for next week
* References and resources used

Keep the proposal short and to the point.

We suggest you try to get all the major hardware components working by the
first week. That will leave the following week for putting the components
together into a working system.
