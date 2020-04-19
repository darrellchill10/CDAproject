# MySPIM Simulator
Code written in C, Created by Darrell Hill and Daniel Cruz for CDA3103.

A MySPIM assembly code simulator written in C.

## Running the program
1. Download the repository
3. Place Project.c into the 'program' folder
4. Place the program folder into MobaXterm
5. Execute commands from the following:

| Command | Description |
| :-----: | ----------- |
| **r**     | Dump registers** contents |
| **m**     | Dump memory contents in hexadecimal format |
| **s[n]**  | Step to the next instruction (Step to the next n instruction) |
| **c**     | Continue simulation (until halt) |
| **H**     | Check if program has halted |
| **d**     | Hexadecimal dump from addresses ads1, ads2 |
| **I**     | Inquire memory size |
| **P**     | Print the input file |
| **g**     | Display all control signals |
| **X, q, Q** | Quit simulator |

Credit to Sarah Angell for table commands and supporting .c and .asc files found in the program folder.