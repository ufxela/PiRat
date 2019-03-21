# Pi Rat!

**Take a look at the video_links.md file in the images folder to see some videos 
of the robot in action!**

## Project Description

### Introduction
The Pi Rat

I had the idea for the Pi Rat during parents weekend, when I was talking with my mom and my
dad. My initial impression of the robot was pretty wrong. I thought that it would be able to 
maintain it's position well enough that I wouldn't have to have a line sensor 

### The steps

**Software**

**Hardware**

### The steps, in more detail

**Software**

**Hardware**

### The product
Position correcting (robust)
Modular (can be adjusted to any size maze, dimensions, configuration & wall length)
Two modes: basic wandering, depth first search.

## Challenges
-pwm outputs weren't working
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
