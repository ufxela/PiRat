# Pi Rat!

**Take a look at the video_links.md file in the images folder to see some videos 
of the robot in action!**

## Project Description

### Introduction
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
