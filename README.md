# Pi Rat!

**Take a look at the video_links.md file in the images folder to see some videos 
of the robot in action!**

## Project Description

### Introduction
The Pi Rat is a robot that can solve a maze autonomously. It does this with two abilities: 
sensing and movement. Sensing comes through the form of an ultrasonic sensor (which is mounted
on a servo so it can look around) as well as a line follower module. The ultrasonic sensor
detects walls and the line follower module keeps the robot from veering off path (allows the robot
to go straight over long distances without hitting a side wall). The movement abilities come through 
two continuous rotation servos with hall effect sensors to provide rotational feedback. Each of these
has a wheel mounted to them, to create a differential drive car. Also, as I mentioned previously,
there's a servo which pans around to detect walls in combination with the utlrasonic sensor. 

The idea of the Pi Rat is to know as little information as possible about the maze. So far, I've been able
to eliminate the need for where the exit of the maze is, as well as any information about how the maze 
is set up. The information that I still require, and can be elimiated are the following: 
* The wall length / size of the maze
   * can be eliminated by sampling wall sizes with the ultrasonic sensor
* The dimension of the maze
   * With the basic wandering algorithm this actually isn't required information
* The starting position & bearing of the Pi Rat
* The need for a black line under each square of the maze to keep the Pi Rat from veering off path
   * Can use the ultrasonic sensor to measure where the Rat is relative to walls, in order to correct 
     position. Is more complex, however, than having black lines, as there won't always be walls present.

I had the idea for the Pi Rat during parents weekend, when I was talking with my mom and my
dad. My initial impression of the robot was pretty wrong. I thought that it would be able to 
maintain it's position well enough that I wouldn't have to have a line sensor.

Through this project, I've prided doing as much as I can by myself. It's meant that I've gotten the 
opportunity to learn new, unexpected things, like 3D modelling with CAD, laser cutting and 3D printing. 
I've also learned that I suck at painting (I can't paint straight, clean lines haha).

### The steps

The work I've done in this project can be broken down into bit sized steps. In approcimate cronological 
order for each category of work (I did software and hardware simultaneously): 

**Software**
1. Write a module to interface with the ultrasonic module 
1. Write a module to output pwm signals
1. Write a module to read pwm inputs
1. Write a module to control servos with the pwm output module
1. Write a module to control continuous rotation servos with the pwm output module
1. Write a module to interface with the line detecting sensor
1. Write a module to provide for basic differential drive car controls
1. Write a module to bundle all of the Pi Rat sensing into a single unified location
1. Write a module to bundle all Pi Rat movements into a single unified location, also add 
   movement correction, to ensure the Pi Rat is properly oriented
1. Write a module to use Pi Rat sensing and movement modules as well as maze solving algorithms
   to create functions which provide the robot with it's highest level of functionality
1. Combine everything neatly with a button which can be used to start / stop Pi Rat maze solving.

**Hardware**
1. Design Pi Rat robot chassis & 3D print it
1. Create a maze
   1. For portability, uses laser cut & self designed wall holder posts
   1. For walls, uses cut manilla folders which slot into wall holder posts
   1. For the ground, used a giant sheet of paper with a painted grid. 

### The steps, in more detail

**Software**
1. *Write a module to interface with the ultrasonic module:* This was relatively simple, doesn't 
   use interrupts. I referenced Pat's ultrasonic code. Initially, this function returned values in 
   inches. However, I wanted more accuracy, so I changed that to microseconds. One big issue I've had 
   with the ultrasonic sensor is the power line coming out, and making me think that my maze solving
   algorithms were broken. Make sure everything's plugged in!
1. *Write a module to output pwm signals:* This was one of the more challenging modules to write. I used
   interrupts to do it. My initial strategy was to step a timer up, and have thresholds where if a timer step
   counter was past that threshold, the output would switch. It ended up hogging up all CPU time, but I think
   it may be because I forgot to clear timer events. Regardless, I ended up switching to the current strategy
   of taking up 4 ms of every 20 ms of CPU time for a dedicated PWM output period. Bit banging. 
1. *Write a module to read pwm inputs:* This was an easy module to write initially, but some bad planning came
   back to bite me. The initial problem was that since pwm outputs hogged a big chunk of CPU time, if inputs
   clashed with outputs, bad things would happen. I fixed this by timing input reads around outputs. Another issue
   here was I forgot to ground things.
1. *Write a module to control servos with the pwm output module:* pretty straightforward, just tedious. 
1. *Write a module to control continuous rotation servos with the pwm output module:* A regret that I have is
   not leveraging the servo control module to make this. If I care enough in the future, I may fix this, but it
   functions perfectly right now. 
1. *Write a module to interface with the line detecting sensor:* Pretty straight forward, as I had the reference 
   from the manufacturer which included Arduino code. 
1. *Write a module to provide for basic differential drive car controls* This was one of the harder modules to
   write. The issue was that going straight means you need to move each wheel the same amount at the same speed.
   This is difficult when you use continuous rotation servos, which by nature are very inaccurate. Luckily, my
   continuous rotation servos had rotary encoders which made straight forward motion actually possible. However,
   I had to change my strategy for moving forward to a more complex one in order to get things working well.
1. *Write a module to bundle all of the Pi Rat sensing into a single unified location* Pretty straight forward. Uses
   a couple of different methods to filter through data from sensors, to make sure they're accurate. A problem is
   if a line is right in between two sensors of the line detector, it will hold indefinitely, or at least for a noticable
   time. This can be fixed with a better data getter function.
1. *Write a module to bundle all Pi Rat movements into a single unified location, also add* 
   *movement correction, to ensure the Pi Rat is properly oriented:* This module works really well in its current form.
   To correct forwards motions, it corrects any lateral error as well as angular error based on a measurement it makes 
   before movement as well as a measurement it makes after movement. To correct turns, the Pi Rat will turn, then fix the
   lateral error. These corrections are really nice because it allows for a lot of leeway in the maze design. For example,
   walls don't all have to be the perfect size, as after each turn, any error should be corrected. Furthermore, the turns
   don't have to be perfectly 90 degrees, as the forwards motion correction will soften offsets.
1. *Write a module to use Pi Rat sensing and movement modules as well as maze solving algorithms*
   *to create functions which provide the robot with it's highest level of functionality:* This was the final step
   in my mind, and used my cs106b maze solving knowledge.
1. *Combine everything neatly with a button which can be used to start / stop Pi Rat maze solving:* icing on the cake; to
   the in class demonstration a lot more approacable to interaction. 

**Hardware**
1. *Design Pi Rat robot chassis & 3D print it:* Went through three iterations of design. The first did not account
   for the line follower, as I didn't expect to need it. The second fixed this and the third added additional supports
   between the top and bottom pieces to make the fit cleaner. I really liked the way my chassis turned out. My idea
   behind it was to make it as small as possible, so I could make the maze size as small as possible. I also didn't want
   anything permanent, so there's no glue holding anything together: everthing is friction fit (well, wires are taped down).
   Some things just worked out in the design, like routing the continuous rotation servo's wires, or making sure the 
   wheels fit in the chassis. Also, I got to learn (well, sorta...) fusion 360. I don't say that confidently because all
   I did was make a bunch of rectangles and extrude them to various heights. 
1. *Create a maze:* In fashion with the rest of my project, I wanted to complete this myself. I also wanted it
   to be portable, for convenience. 
   1. *For portability, uses laser cut & self designed wall holder posts:* I designed these in fusion 360, and discovered
   that laser cutters are really scary. I also had to assemble the flat pieces into non-flat structures. I used a lot
   of hot glue for this.
   1. *For walls, uses cut manilla folders which slot into wall holder posts:* Manilla folders are actually an amazing
   building material. I got a bunch super cheap for their conventional purpose, but I had extras, so why not put them to use?
   Fun fact: I prototyped a chassis with only manilla folders, tape and hot glue. 
   1. *For the ground, used a giant sheet of paper with a painted grid:* I painted this, using long manilla folder
   strips to make sure I didn't paint in the wrong places. That ended up not really working because paint got on the
   bottom of those strips, and ended up spreading paint all over, including on my body. As a result, I had to use white paint
   to cover all the black blotches that weren't supposed to be there (on the maze, not my body... thankfully the paint was
   easily washable). 

### The product
The end result of all of this work was
Position correcting (robust)
Modular (can be adjusted to any size maze, dimensions, configuration & wall length)
Two modes: basic wandering, depth first search.
Usage description

## Notable Challenges
-pwm outputs weren't working
   (1) first strategy didn't work (but I think I know where it went wrong). 
   (2) servos weren't responding to pwm outputs, even though with a logic analyzer, I knew they were good. Ended up being that they weren't grounded. 
-pwm inputs worked, but then stopped working (reason: wasn't grounded). 
-going straight is harder than I thought
-Accurately reading wheel positions (and avoiding rotation counting skips!)
-DFS maze solver (it's one thing to simulate it on a computer, and another to do it in real life)
Wasn't as simple as 106b, unfortunately, because (1) maze was slightly more complex (walls are
not the same width as paths) and (2) you have to physically move. Ideally, (2) should be solved
with a layer of abstraction. I guess I got lazy, though, and though I could deal with it. It bit
me in the end, but I got over it. (Describe in more detail the problem here)
-By far the hardest part of this assignment was visibility & understadning the hardware
Ex w/ visibility: for the past 5 hours, I've been trying to figure out why my backtracking wasn't working. I finally ifugred it out. It was an easy fix. But discovering what was going on was extremeely difficult because when the rat is solving the maze, it's very difficult to know what it's thinking. What I ended up doing was getting a few extra jumper wires and connecting the rat to my computer while it was solving the maze and walking along with the rat as it was solving the maze. super tiring on my arms. but it worked.

## Things learned
-planning is super critical, and possibly the most difficult part of it all. 
(many modules were generally easy (aside from some road bumps talked about above, but 
finding the right order to write them in / determining what modules to even right in 
the first place was not as trivial).
-do things in the right order: you may find yourself in a situation where you can either
write tests to debug / make information more transparent and make the mechanisms behind
things more visible, or you can try to brute force things and try to solve things without 
that information helping your strategy. Sometimes the latter is faster. However, other times 
it is not, and it can get very frustrating, because you just feel like you don't know what is
going on. I have more appreciation for the advice that it's worth your time to write tests
and make code more visible. Even if it takes 30 minutes, it will save your butt later. 
-include some of the things I talked about in the proposal

## Extensions

## Files
1. Images is a folder which includes photos & a markdown file video_links.md to video links
1. 3d wall plate pi rat maze.stl is a stl file for my wall holder plates. I cut 200 of these out
   (25 wall holders * 8 plates per holder)
1. 8x pi rat maze wall plates.dxf is the file I opened with inkscape on Lab 64's laser cutter computer
   Note that you might need to delete some lines, as some are doubled up (my bad).
1. Chassis_Bottom.stl is the part I 3D printed to make the bottom half of the chassis. I'm not currently
   using this one. 
1. Chassis_Top.stl is the final part I 3D printed to make the top half of the chassis
1. Makefile is self explanatory
1. README is this!
1. better_bottom_chassis.stl is the final part I 3D printed to make the bottom half of chassis
1. car_control_module.c/h provide for basic, non corrective car movements for a differential drive car.
   They use cr_servo_module.c/h and pwm_input_module.c/h
1. cr_servo_module.c/h control continuous rotation servos. Uses pwm_output_module.c/h
1. cstart.c is some code that sets a few things up and calls main
1. libmypi.a is the library I wrote for the first 8 weeks of cs107e
1. line_follower_module.c/h is the module which interfaces with the line follower hardware over i2c
1. main.c is the centerpiece of the program. Contains a lot of test code. 
1. memmap is for linking
1. pi rat maze bottom:top plate 3d.stl is the 3d version of the top/bottom plates of wall holder
1. pi rat maze top:bottom plate.dxf is the inkscape openable file I used to laser cut the wall holder 
   plates
1. pi_rat_control.c/h implement different control functions. Uses pi_rat_movement.c/h and 
   pi_rat_sensing.c/h 
1. pi_rat_movement_module.c is the corrective movement module which moves the car in a way which is 
   specific to the setup of a given maze. Uses car_control_module.c/h and pi_rat_sensing.c/h
1. pi_rat_sensing_module.c/h combines ultrasonic_module.c/h and line_follower_module.c/h into one
   centralized module
1. proposal.md was the description that I wrote up on the first lab for this project, and added to
   as I was working in the first week.
1. pwm_input_module.c/h read in pwm signals through interrupts. Uses pwm_output_module.c/h for proper
   timing
1. pwm_output_module.c/h output pwm signals using timer interrupts
1. servo_module.c/h use pwm_output_module.c/h to make a more intuitive servo control library
1. start.s is the first file that gets called when running
1. ultrasonic_module.c/h interface with the ultrasonic sensor. 
1. updated_bottom_chassis.stl was an internediary between my original bottom chassis and the one I used

## Team member work outline
I did it all myself (with the advice of others, listed below, of course)!

## Resources/Citations

Pat helping me solve pwm problems

Julie / Jennifer / Ashwin for helping me in office hours and lab

Michal for giving me some EE advice. 

Roommate, for getting excited about the project, and experienceing successes/falls w/ me.

3d printing/laser cutting
      -lab64

Pat's ultrasonic sensor
      -just for conversion for inches / reference. Didn't use any of his code

Pseudo code for forward motion with wheel angle feedback
       -just followed the pseudo code, adding in limits for wheel throttles and fine tuning for my car

Line follower example code
     -ported the code pretty much directly
     
Notes: it would be super easy for me to make a line follower robot from this. 
Also it wouldn't be impossible for me to make a room rover robot (like a roomba) from here too.
