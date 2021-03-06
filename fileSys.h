// Evan Thompson, Jack Cusick, Tausif Ahmed

//Perform LRU on replacement of frames
//
//Allocate frames only on demand
#ifndef FILESYS_H
#define FILESYS_H


#include <map>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <dirent.h>
#include <stdio.h>
#include "file.h"


#define STORAGE_STR ".storage"
#define DIR_PERMISSIONS 0777
#define SLASH_STR "/"
#define CURR_DIR_STR "."
#define PREV_DIR_STR ".."

/* read/delete global mutex variable */
pthread_mutex_t RDMutex = PTHREAD_MUTEX_INITIALIZER;




class FileSystem{
  
 public:
  //constructors
  FileSystem(){
    num_files=0;
    setup();
  }
  
  ~FileSystem(){
    for(int i = 0; i < num_files; i++){
      delete files[i];
    }
    
    //delete [] files;
  }
  
  
  //Functions
  //index or -1 if DNE
  int findFile(char * name){
    //printf("find name: %s\tnum_files: %d\n",name,num_files);
    
    for(int i = 0; i < num_files;i++){
      //printf("files[%d]->file_name: %s\n",i,files[i]->file_name);
      if(strcmp(files[i]->file_name,name) == 0){
	return i;
      }
    }
    return -1;
  }

  //update function
  //0 for sucess 1 for failure
  int setup(){
    DIR *dir;
    struct dirent *dp;
    if((dir = opendir(STORAGE_STR)) != NULL){
      while((dp = readdir(dir)) != NULL){
	std::string f_name(dp->d_name);
	if(f_name != std::string(CURR_DIR_STR) && f_name != std::string(PREV_DIR_STR)){
	  int rc = addFile(dp->d_name);
	  if(rc == -1){
	    return 1;  //fail
	  }
	}
      }
      closedir(dir);
    }
    else{
      return 1;  //fail
    }
    return 0;
  }
  
  
  //-1 if already exists, index if inserted
  //server's responsible for setting file's storing to 0
  int addFile(char * name){
    int index = findFile(name);
    if(index != -1){  //already exists!
      return -1;
    }
    File * f = new File(name);
    files.push_back(f);
    num_files++;
    return num_files-1;
  }

  //could implement waiting for reads to finish here... but probably better in server
  //0: success, 1: failure
  int removeFile(char * name){
    //printf("removeFile--finding file\n");
    int index = findFile(name);
    //printf("found file!: index = %d\n",index);
    if(index == -1){
      return 1;
    }
    
    files[index]->readAble = 0;  //stop new files from reading (avoid starvation)

    while(files[index]->numReads > 0){
      //wait for reads to finish
    }

    //std::cout<<"deleted"<<std::endl;
    delete files[index];
    files.erase(files.begin()+index);
    //std::cout<<"erased"<<std::endl;
    num_files--;
    
    return 0;
  }


  void print(){
    std::cout<<"numfiles: "<<num_files<<std::endl;
    for(int i = 0; i < num_files; i++){
      printf("i: %d file: %s, numR: %d\n",i,files[i]->file_name,files[i]->numReads);
    }
    
  }

  
  
  //Variables
  int num_files;
  
  //filesystem files
  std::vector<File*> files;  //not dynamically allocated
  
};

#endif
