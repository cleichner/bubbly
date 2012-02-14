This is hardware agnostic firmware for a robot in the style of the IEEE
Micromouse. It depends only on the functions defined in hardware.h for full
control and maze solving. There are currently two simulation programs: softbot,
which visualizes the robot's movements with ncurces and textbot, which logs its
movements to stdout. Both programs read files with ASCII maze drawings to
define their worlds. The size of the expected maze is defined in maze.h.

Hardware controlling code to come.

