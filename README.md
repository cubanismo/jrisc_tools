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

On Windows, you can use MSYS/MinGW to build with make, or use the Visual Studio
solution files in the vs2022 directory. Note they can also be used with earlier
versions of Visual Studio by adjusting the "Platform Toolset" of each project in
the "Project Settings" menu's "Configuration Properties-\>General" tab.

JDIS Usage
----------

    jdis [-gdamhv] [-o <offset>] [-b <base address>] <JRISC machine code file>

    Options:
      -g: Parse code as Tom/GPU instructions [default].
      -d: Parse code as Jerry/DSP instructions.
      -a: Print address in hex of each disassembled word.
      -m: Print machine code in hex of each disassembled word.
      -o <offset>: Specify offset into file (0x<hex> or <decimal>)
      -b <base address>: Specify the base load address of the code
      -h: Help. Print this text.
      -v: Version. Print the version and exit.

    Offsets and addresses are parsed as hex if they start '0x',
    octal if they start with '0', or decimal otherwise.
