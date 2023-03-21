This is a model of a 3x3x3 Rubik's cube that responds to user input to rotate in three dimensions.
This was developed on Linux and may not work on other systems due to the usleep function used to maintain a constant frame rate.
Cube General Demonstration Video: https://youtu.be/6IrivgGvOJQ
Cube Solve Video: https://youtu.be/ruYgizF0AnU


The cube is made up of 27 individual cubes, each of which has its own position.
When the cubes are rotated, their position indices are changed so that they're translated to their new position.

CONTROLS:
Camera movement: 
The camera is not free to move in any direction because it should always be focused on the cube.
Instead, it rotates around the cube on a cylinder where the user can change the radius.
The cube is fixed at the origin and the camera always looks towards it from any position around the circle.
    W: forward
    A: left
    S: backward
    D: right
    Left Alt: down
    Space: up
    H: return camera to start horizontal position (height/zoom don't change)

Camera Speed:
    Up Arrow: increase speed
    Down Arrow: decrease speed

Cube rotations (from perspective of camera home):   
    Cube rotations are clockwise when that side is viewed head on
    Shift modifier does these turns in the opposite direction
    R: right
    L: left
    F: front
    B: back
    U: up
    V: down
    E: rotate equator layer
    Z: rotate entire cube clockwise permanently. This changes the side that the relative rotations above operate on.

M: change view between stickered and not stickered
N: change color scheme (when cube is solved)
1: scramble cube
I: insert left algorithm
O: insert right algorithm
2: make yellow cross algorithm
3: make yellow top algorithm
4: flip front top 2 corners algorithm
5: rotate center top colors clockwise algorithm
6: rotate center top colors counter-clockwise algorithm


These indices correspond to the rotations above. There are 16 different rotations total, with 14 from the different
regular rotations, and 2 additional from the rotations of the entire cube.
Cube Side Indices:
index:   0     1      2    3       4      5
side:  back, front, left, right, bottom, top

multiple rotation indices:
6: rotate middle same direction as default top, E for equator
7: rotate top, equator, bottom at the same time, clockwise from the top

Reverse Regular Rotation Indices:
index:   8    9     10    11      12    13     14 
side:  back, front, left, right, bottom, top, equator

15: rotate entire cube counterclockwise from the top


This is a visualization of how the cube positions are translated into the 3D cube when viewed from the camera start.
Front Layer:
0   1   2
3   4   5  
6   7   8

Middle Layer:
9   10  11
12  13  14      
15  16  17

Back Layer:
18  19  20  
21  22  23  
24  25  26







