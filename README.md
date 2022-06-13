Atari Jaguar RISC Tools
=======================

Currently the only tool here is jdis, a minimal disassembler for Jaguar RISC
machine code. However, I've attempted to structure the code such that the core
routines could be used to build other tools such as assemblers, optimizers,
hazard warning generators, etc.

Building
--------

On a POSIX system with a C compiler and GNU make installed:

    $ make

You're on your own in Windows or other operating systems for now, but patches
for Visual Studio or Xcode support are welcome.

JDIS Usage
----------

    jdis [-gdhv] <JRISC machine code file>

    Options:
      -g: Parse code as Tom/GPU instructions [default].
      -d: Parse code as Jerry/DSP instructions.
      -h: Help. Print this text.
      -v: Version. Print the version and exit.
