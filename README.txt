###########
DAT A403 P1
############

Description Seminar Planner 9000 creates valid time tables that uphold hard constraints
of time and space. In addition, the program can calculate the fitness score of a population 
of time tables and find the best one based on soft constraints.  

Compile with: gcc fitness.c tables.c main.c sp_types.c search.c IO.c -Wall -Wextra -ansi

input_files/    is the folder containing data files to be read from.


main.c		entrypoint - user controls program with keyboard input
IO.c		input/out to/from cmd prompt
search.c	functions relating to array operations
tables.c	creates time tables and validates them
fitness.c	formulas and functions for calculating fitness scores
sp_types	reads from files and creates 'dictionary' of the data

