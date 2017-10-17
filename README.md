# HRDPLEX-GUI
HRDPLEX-GUI is a Graphical User Interface (GUI) to the Hercules Mainframe Emulator. To use this application, you must have the Hercules Emulator installed with GUI support. As of Version 4, this should be enabled by default.


### Setup
1. Run Qt's makefile generation command at the root folder
```
$ qmake
```
2. Start the build process
```
$ make
```
3. Install hrdplex-gui
```
$ sudo make install
```

### Functionality Wishlist
1. Built in master console
2. Tape robot system
3. Print to printer from MVS
4. Virtual SYSPLEX/Distributed System

### Assumptions
This software makes a couple of assumptions, I will list them below and you can either update/install
what is needed on your system or make the required changes. 

1. C++ compiler (Ubuntu: sudo apt-get install build-essential)
2. Qt development framework from a release above 5.5.1.
