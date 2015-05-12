// Evan Thompson, Jack Cusick, Tausif Ahmed

//Perform LRU on replacement of frames
//
//Allocate frames only on demand

#include <map>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <dirent.h>
#include <stdio.h>

#define STORAGE_STR ".storage"
#define DIR_PERMISSIONS 0777
#define SLASH_STR "/"
#define CURR_DIR_STR "."
#define PREV_DIR_STR ".."

typedef unsigned char BYTE;

class PagingSystem{

	private: 
		
		//Variables
		int num_frames;
		int size_frames;
		int frames_per_file;

		//FrameSystem frames;


		//Functions
		long get_file_size(std::string file_path);
		bool file_exists(std::string file_path);

		void init_pages();
	public:
	 	PagingSystem(int n_frames, int s_frames, int max_file_frames);

		int get_num_frames(){ return num_frames; }
		int get_size_frames(){ return size_frames; }
		int get_max_frames_per_file(){ return frames_per_file; }

		bool load_file(std::string file_name);

		int store(std::string file_name, std::vector<BYTE> data);

		int delete_file(std::string);

		std::vector<std::string> dir();

		void setup_storage();

		void print_stats();

};
