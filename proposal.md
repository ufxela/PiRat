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

One challenge of this project is keeping the Pi Rat's orientation correct throughout
the maze. Because the Pi Rat senses very little about it's environement, it is difficult
for it to truly know where it is (position and bearing) within the maze. Therefore, small
inaccuracies in positioning can avalanche to throw the entire Pi Rat off track. 

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

1. Write a module to output PWM signals
1. Write a module to read PWM inputs
1. Write module to setup and control a servo through PWM
1. Write module to setup and control continuous rotation servo through PWM
1. Write module to read PWM communications from Parallax Feedback 360 servos, to determine positioning
1. Either:
   1. Communicate through I2C to optical time of flight sensor
   1. Read/write through GPIO to communicate with ultrasonic distance sensor (This is what I have chosen).
1. Write module to communicate via I2C with the line follower module
   1. Will use libpiextra i2c modules
1. Module to move support basic movement of a continuous rotation servo powered car.
   1. With two independent wheels, we will be able to move forward, backward, turn and shimmy to the left and right
1. Module to support advanced movement for Pi Rat
   1. Will keep track of position of Pi Rat
   1. Will have discretized movements so as to follow maze properly
   1. Will have correctional movements that ensure proper orientation within maze (no roaming off track)
1. Code to use all modules to sense environment and move the rat in response based on
   where it wants to go. 
   1. Internal data structure to represent maze
   1. Algorithm to solve maze
   1. Internal data structure to represent current path, and provide ability to backtrack path.
   1. Way to determine when solution is discovered
   1. Traverse solution (back and forth, like a sentry) once found. 
   1. Once maze map is completed, user may tell Pi Rat to go from point A to point B for any two connected
      points in the maze.
   1. User may input instructions to the Pi Rat beforehand to tell it where to go
1. Extensions:
   1. Code to use modules to traverse a new environment that may not be a maze. 
   1. Code to find shortest path from point A to point B within maze. 
   1. Remove any information about the maze at all (step size, dimensions) and use Pi Rat's sensing abilities
      to determine this information itself. 
   1. Add in remote control abilities. 
   
*Hardware:*

1. Model and 3D print chassis for rat
1. Mount time of flight sensor / ultrasonic distance sensor
1. Mount line follower module
1. Find / make wheels which have really good grip
1. Make maze, ideally with configurability. (slot and fit walls)

**Goals for Next Week**
1. Software steps 1-7
1. Hardware steps 1-4
1. Test movement of Pi Rat and see how accurately it can hold its position
1. Test accuracy of ultrasonic sensor
1. Configure servo motors
1. Configure line follower sensor

**Reference and Resources**

*another document that goes into more detail about my project*

https://docs.google.com/document/d/10EH3p42lqzU2tbl3T3y5yWcG4yZZ02CRSOTUqFnZRl0/edit?usp=sharing

*line follower*

http://wiki.sunfounder.cc/index.php?title=Line_Follower_Module
https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/i2c.c
Note: this bad boy can't be powered just through the Pi's GPIO pins. Needs external supply!

*ultrasonic sensor*

https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
https://github.com/cs107e/cs107e.github.io/blob/master/lectures/Sensors/code/sonar/sonar.c

*servos*

https://learn.sparkfun.com/tutorials/pulse-width-modulation/all

*CS107E Libraries*

https://github.com/cs107e/cs107e.github.io/tree/master/cs107e/src
https://github.com/cs107e/cs107e.github.io/tree/master/cs107e/include

Specifically: gpioextra, interrupts, i2c. 

*Wheels*

The wheels that I'm using are borrowed from this kit: https://www.amazon.com/Geekstory-Feetech-Degree-Continuous-Rotation/dp/B079FRHBTR/ref=sr_1_2_sspa?keywords=continuous+rotation+servos&qid=1552198755&s=gateway&sr=8-2-spons&psc=1

**Lessons that I have Learned**
* Have only one ground. Otherwise, you will pull your hairs out.
* Line follower module needs to be powered externally
* At first glance, things will seem definitely doable. Then once you start doing them, they no longer seem doable.
  But once you complete them, they seem very doable.
* Object oriented programming can be very helpful, but not if you don't do it. I should do it more often in my coding.
* There is a lot more to consider besides the software side of things. For example, an uneven weight distribution
  in the chassis will lead to one wheel having more grip than the other. So have an even weight distribution.
* Many things pop up and make other things hard. For example, keeping the orientation of the Pi Rat correct throughout the
  maze. You have to constantly figure solutions out to these challenges. Many of the best solutions come in the shower.
* Oftentimes, things are difficult because you make them so. It can be good to take a step back, and think: is there a 
  simpler, easier solution to things? This is difficult because you can get caught up in the way you currently want to
  do things, and miss out on otherwise obvious things. For example, when writing PWM outputs, initially I picked up Pat's
  idea (which, now that I look back, should work, if I properly used get_and_clear_event()...) with the incrementing interrupt
  counter. But later, I took a simpler, better approach which is currently implemented. Also, with utilizing the line
  follower, my initial thought was to update the orientation as the Pi Rat moved. But then I realized it would be much 
  simpler if I compartmentalized each operation. First move, then correct the movement. 
* This brings me to: compartmentalizing things means you can do so much more. "Stand on the shoulders of giants" except
  In this case, you are the giant. There are so many details of my code that are critical but I've totally forgotten about
  (well, I could recall them, but it's much easier not having the worry of needing to). And by leaking those details from my
  memory, I've allowed myself to use my mind more completely to solve each task. Whereas, if I chose to mesh everything
  together, either I would be able to do much less in coding, or I would have to spend way more effort achieving it. This
  is part of the reason I chose not to use a task queue for control, and instead opted to compartmentalize PWM outputs, even
  if it meant having that 2-3 ms penalty. 
* When you're doing something completely alone, that you really enjoy, there really is nothing else needed. Although I've
  been really frustrated at times (like before I realized you should connect all your grounds--lesson learned!), and I've
  definitely wished that I had someone along side me helping me solve my problems (in a way, I have--through the course staff)
  but at the end of the day, I'm really glad to have embarked on this project. It's definitely overwhelming. But worth it.
  
