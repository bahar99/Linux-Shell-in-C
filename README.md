# Linux-Shell-in-C


## Overview
A shell is special user program which provide an interface to user to use operating system services. Shell accept human readable commands from user and convert them into something which kernel can understand. It is a command language interpreter that execute commands read from input devices such as keyboards or from files. The shell gets started when the user logs in or start the terminal.
In this code we make our own Linux Shell.

## Requirements
Just need a gcc compiler and also install the readline library:
```
sudo apt-get install libreadline-dev
```
## Steps To Run
* ### 0-Cloning
    + First of all clone the project : 
 ```
 git clone https://github.com/bahar99/Linux-Shell-in-C
```
* ### 1-Compile
    + YOU NEED TO LINK THE LIBRARY,PUT -lreadlin IN GCC ARGUMENTS:
 ```
 gcc shell.c -o shell -lreadline
```
## source
* https://www.geeksforgeeks.org/making-linux-shell-c/

## support
Reach out to me at boroomand.bahar@yahoo.co.uk
