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
#include <sys/types.h>
#include <sys/uio.h>
#include "frame.h"

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
		std::vector<Frame> frames;
		
		std::map<std::string, std::vector<int> > page_table;

		//Functions
		long get_file_size(std::string file_path);
		bool file_exists(std::string file_path);

		int get_LRU(std::string file_name);

		void store_file_data(std::string file_path, std::string file_name, int page_num, int frame_num, int amount);
		
		void setup_storage();

		int page_already_loaded(std::string file_name, int page_num);
		
		void remove_frame_num_from_page(int frame_num);

		void add_frame_num_to_page(std::string file_name, int frame_num);

		void init_frames();
	public:
	 	PagingSystem(int n_frames, int s_frames, int max_file_frames);

		int get_num_frames(){ return num_frames; }
		int get_size_frames(){ return size_frames; }
		int get_max_frames_per_file(){ return frames_per_file; }

		//bool load_file(std::string file_name);

		std::vector<BYTE> read_page(std::string file_name, int offset, int amount);

		int store(std::string file_name, std::vector<BYTE> data);

		int delete_file(std::string);

		std::vector<std::string> dir();


		void print_stats();

};
