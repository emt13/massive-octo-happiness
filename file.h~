// Evan Thompson, Jack Cusick, Tausif Ahmed

//Perform LRU on replacement of frames
//
//Allocate frames only on demand
#ifndef FILE_H
#define FILE_H

#include <map>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <dirent.h>
#include <stdio.h>

class File{
  
 public:
  //Functions
  File(char * fileName){  //,int fileSize
    file_name = (char *)calloc(strlen(fileName), sizeof(char));
    strcpy(file_name, fileName);
    //file_size = fileSize;
    storing = 1;  //if you make a new file, should automatically be storing
    numReads = 0; //just made file, no one reading
    readAble = 1;
  }

  ~File(){
      free(file_name);
  }


  //Variables
  char* file_name;  //should allocate new data
  //int file_size;  //leaving out for now
  int numReads;
  bool storing;
  bool readAble;


  //filesystem files
  std::vector<File> files;
  
};

#endif
