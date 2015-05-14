// Evan Thompson, Jack Cusick, Tausif Ahmed

Operating Systems Homework 4

compile with: g++ server.cpp frame.cpp pagingSys.cpp -Wall -pthread

This is our second submission, we spoke to Professor Goldschmidt who, due to several minor mistakes causing 
 significant failure, said we could submit late without penalty.

FILES:
-server.cpp: handles client interaction and commands, important function is CLIENT THREAD
-pagingSys.h: handles paging system
-pagingSys.cpp: cpp of pagingSys.h
-frame.h: handles frame data
-frame.cpp: cpp of frame.h
-fileSys.h: keeps track of all files in .storage and who's accessing them
-file.h: information on each file in .storage

Notes:
-READ returns the correct data but buffer size is offset by 1.  This is because we consider and offset of 0 to start at the beginning
 of a file and an offset of 1 to start 1 byte into the file.
  e.g.: file.txt has data: "abcd\n"
   READ file.txt 0 2 returns: ab
   READ file.txt 1 2 returns: bc 

-We try to catch as many user errors as possible including incorrect number of arguments and incorrect commands, however most instances
 of incorrect arguments themselves (e.g. string when an integer is expected) have not been tests/worried about.

-STORE and READ works with binary files according to our tests.  We used a client to store hello.out and were successfully able to run
 command after it was stored.  READ was also able to access hello.out and output the contents.
