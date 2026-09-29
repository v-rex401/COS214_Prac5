# COS214_Prac5

- Kayla Falconer – u25006747
- Vashti Pillay – u25041887
- Lindokuhle Skosana – u24891968

## 1. Docker Instructions

1. Run `docker build -t campusguard . ` in the terminal

2. Run `docker run -it campusguard bash`

- -i - interactive, keeps stdin open to write commands in container
- -t - Get a command prompt
- bash - tell docker to launch Bash shell

3. When your done to exit container run `exit`

## 2. Makefile Instructions & Running Program

After setting up docker

1. Type `make`
2. After is compiles type `make run` to run it
3. Run `make valgrind` to check for memory leaks
4. Run `gdb ./taskforge` after compiling with `make` to use debugger
