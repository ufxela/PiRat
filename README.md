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

My junior year of high school, I tried to control servos with a Raspberry Pi running an OS. I quickly discovered
that this was not as straightforward as I would wish: apparently, the Raspberry Pi wasn't able perform time
sensitive actions well. I'm really glad that I've been able to come back to that and be able to control servos
with the Raspberry Pi with no extraneous hardware helping me.

### The Steps

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

### The Steps, in More Detail

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

### The Product
The end result of all of this work was that I had a Pi Rat that can solve a maze in two ways!

The first way is with a really basic algorithm which follows this procedure:
`
for each direction:
  if open:
    go that direction
    recur
`
The nice thing is that this algorithm needs very little information about the maze. The bad thing is that it doesn't work
always. To use this, just call the maze wander function, place the rat in the maze, and watch it solve the maze!

The second way is with a depth first search. This algorithm needs a little more information about the maze, like
where the Pi Rat starts in the maze, and the dimensions of the maze. It always works when there is a path, however. To use 
this, update some constants in main.c (as well as wall lengths in the car control module), then call the maze solver
function and watch as the Pi Rat solves the maze!

The program is for the most part modular: by chaning a few constants, the maze solving is generalizable to 
any rectangular maze of any dimensions, configuration and wall length!

[Video Links!](https://github.com/cs107e/ufxela-project/blob/master/images/video_links.md)

### Pictures! (A Lot More in Images Folder)
The Maze:
![The Maze](/images/4x4_maze_1.jpeg)
Another Maze:
![Another Maze](/images/4x4_maze_2.jpeg)
The Maze, deconstructed
![Deconstructed Maze](/images/maze_compact.jpeg)
The Pi Rat: 
![Pi Rat 1](/images/new_chassis_1.jpeg)
![Pi Rat 2](/images/new_chassis_2.jpeg)
![Pi Rat 3](/images/new_chassis_3.jpeg)
![Pi Rat 4](/images/new_chassis_4.jpeg)
![Pi Rat 5](/images/new_chassis_5.jpeg)


## Notable Challenges
1. *PWM Outputs:* This was a big hurdle. My first implementation of PWM outputs failed. It seemed to hog all the CPU time.
  I now relaize that I probably wrote it wrong (forgot to clear timer events). I haven't had the effort to check if this is
  why, since the current version works. However, the reason I think this is because, when set to have cycle length 20ms, 
  the actual cycle length turned out to be ~7ms (measured with a logic analyzer). I think this is because the interrupt 
  handler was just being called nonstop. Once I did switch to the current implementation of the code, however, the servos
  still weren't working, even though the logic analyzer showed valid PWM signals. Initially, I thought this was because
  the signals weren't precise enough. However, it turned out that it was because I didn't connect all of my grounds. 
  Throughout the process of solving PWM outputs, Pat helped me a lot. One suggestion he gave me was to use a task queue, which
  although I didn't end up implementing, inspired me to switch to the current implementation of PWM. 
1. *PWM Inputs:* While solving PWM outputs, I reached a point where I felt I was in too deep and couldn't think freshly any
  more, so I switched to solving PWM inputs. My initial implementation of these worked, and I was super excited! Then,
  fifteen minutes later, with no changes to the code, I tried again, and it had stopped working! This was heartbreaking,  
  especially since I wasn't able to get PWM outputs to work. I spent an hour digging through the code, seeing what might
  have gone wrong. This didn't lead anywhere. Then had the idea to physically reconstruct my successful setup exactly,
  to see if that would change anything. It did! It turned out that I didn't connect my ground lines properly. That was
  one of the most confusing hours of this project, and one of the most frustrating.
1. *Going straight & accurately reading wheel positions* These two problems were highly entangled. My inital wheel position
  reader was very imperfect. I regularly got very wonky data from the pwm input reader. My inital strategy to solve this
  was to just deal with it by adding in data filters. This gave limited success, but every once in a while, there was an edge
  case of bad data which would break everything. This meant the wheels would track well for maybe a minute of movement, but
  break after that. I could have added more filters to cover more and more edge cases. Maybe I could have even gotten a 
  perfect filter. However, my filtering code was only getting longer and more convoluted. The right way to do things was to
  dig into the PWM inputs and see why they were so wonky. Once I solved that, I didn't need any filters, and my code worked
  better than ever, with more simplicity than ever. Inaccurate wheel positions also meant that my movements were all wrong.
  At the time, I was simply trying to perfect straight forward movement, so we'll focus on that. My initial implementation of
  forward movement was to spin each wheel independently to a set angle, and assume that each wheel was spinning at the same 
  speed. This failed initially because the poor wheel tracking meant I couldn't consistently bring a wheel to a given angle, 
  without "rotation count slipping" which caused wheels to turn a few rotations too much or too little. Once I solved
  the wheel angles, I thought that would solve the issues with my forward motions. They did not. Although each wheel was set
  to spin a different amount, each wheel span at a different rate, which meant at the end of the motion, one wheel would stop
  and the other would continue spinning. This lead to turning. So I decided to switch to a different, more complex forward
  motion function, which made the wheel throttles dependent on eachother's angles. This worked, after some fine tuning.
  It goes to show that not everything is as simple/straightforward as you may initially think.
1. *DFS maze solver:* Unfortunately, this was not as simple as 106b, mostly because of my own fault. In 106b, our mazes
  had walls being area containing squares, instead of one dimensional lines. We also had better data structures. We also
  didn't have to physically move an object and worry about that. Nor did we have to sense the environment as we went. Now,
  I know I could have written more layers of abstraction early on to resolve these differences. But I am naive and didn't
  anticipate these issues coming up. As a result, my code is very verbose, has some arms length recursion, and is pretty ugly
  overall. Furthermore, I had one big bug in my first draft of the code that took me a while to figure out. The bug was that
  I didn't update the position of the Pi Rat enough, which meant it often thought it was in the wrong spot. As a result,
  when testing, the Pi Rat would seemingly ignore certain paths (and presumably, I'd see it run into walls, if I tested it on
  more mazes). This bug would be simple to fix if everything was simulated on my computer. However, since the Pi Rat was 
  physically solving the maze, visibility of the code and program was terrible while the Pi Rat was in the process of solving
  the maze. This is because while the Pi Rat was solving the maze, it couldn't communicate with my computer and therefore,
  I could only speculate as to what it was thinking. To solve this, I spent a couple of hours playing around with the code
  and then re testing, true to a trial and error approach. However, since I had no idea what the Pi Rat was thinking, this
  was very difficult and ultimately unsuccessful (it did get me closer to the end solution, however). Eventually, I 
  succumbed to using extra long jumper wires to connect the Pi Rat to my computer, and then I basically "walked" the Pi Rat
  by following it through the maze with my computer that was connected to it. It was a funny sight to see, but it worked. I 
  was able to get the data I needed to tell me what I was doing wrong (not updating enough). 

In summary, the hardest part of this assignment was definitely visibility & understanding the hardware.

## Things learned
1. Planning is critical, and possibly the most difficult part of it all. Many modules were easy to implement, aside from
  a few roadbumps (like the ones I talked about above). The hard part was finding the right order to write them in and 
  determining what modules to even have, and what to have inside them. 
1. Do things in the right order: you may find yourself in a situation where you can either
  write tests to debug and make information more transparent and make the mechanisms behind
  things more visible, or you can try to brute force things and try to solve things without 
  that information helping your strategy. Sometimes the latter is faster. However, other times 
  it is not, and it can get very frustrating, because you just feel like you don't know what is
  going on. I have more appreciation for the advice that it's worth your time to write tests
  and make code more visible. Even if it takes 30 minutes, it will save your butt later. 
1. Some other things I learned are in my proposal

## Extensions
There were a few things that I had ideas for, and wanted to implement if I had the time, but I never ended up getting to them

Here are a few of them:
1. Remove more information about the maze. I talked more about this above.
1. Make a better interface to the Pi Rat. What if the user could tell the Pi Rat where it's start position should be, 
  with just buttons or something? What if the Pi Rat could display information on a 7 segment display or screen?
1. More maze traversing options: go from point A to point B once a maze is mapped out; after a maze is solved, continually
  traverse the optimal path from start to finish.
1. Remove the line follower sensor completely. Instead use the ultrasonic sensor for all orientation corrections.
1. Move past just mazes and into rooms. Explore a room, kind of line a Roomba. Maybe even keep track of
  data about the room. A simple version of this would be really easy to make. 
1. Remote control abilities, maybe over WiFi?
1. Make a white board robot. I have all of the hardware setup to do this.

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

Pat's ultrasonic sensor [sample code](https://github.com/cs107e/cs107e.github.io/blob/master/lectures/Sensors/code/sonar/sonar.c) 

-I used this to get the conversion from microseconds to inches.

[Pseudo code](http://www.robotc.net/wikiarchive/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Using_encoders_to_drive_straight) for forward motion with wheel angle feedback.
 
 -just followed the pseudo code, adding in limits for wheel throttles and fine tuning for my car

Line follower [example code](http://wiki.sunfounder.cc/index.php?title=Line_Follower_Module)

-borrowed the code pretty much directly

[i2c code](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/i2c.c) to communicate to line follower module
