# General Information
This is a cute little note-organizer that I wrote last semester!
The purpose of this program is to organize my notes for me, create notes for me, and allow easy editing/viewing of past notes.

It check the time of the day and day of the week against every entry in the time file.
If there is a hit, the program tries to create a directory for the class (if one does not already exist) and then opens a file for editing.


# Program Specifics
This project was built on a Linux system (specifically Ubuntu 18)
I have not tested it on Mac or Windows systems, I doubt it works on those platforms (definitely not the install script) but you never know.

All of the code is contained in the src/ folder.
All header files are in src/include/.
To install, simply run the install script.

The install script will:  
- Set up the directory used to house data for this program (\~/.note)
- Create needed files for this program to use (\~/.note/data/time and \~/.note/data/path)
- Compile the source code
- Create a symbolic link to the executable (bin/note) in /usr/local/bin/

If you just want to compile, you can run 'make'
If you want to create a debug build, you can run 'make Test', but I haven't tested that in a while so no promises it works.

There are some bugs I have seen floating around, minor things like verbose data being printed without the -v switch.
