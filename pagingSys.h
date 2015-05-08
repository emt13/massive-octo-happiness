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

#define STORAGE_STR ".storage"
#define DIR_PERMISSIONS 0777
#define SLASH_STR "/" 

typedef unsigned char BYTE;

class PagingSystem{

	private: 
		
		//Variables
		int num_frames;
		int size_frames;
		int frames_per_file;

		//map of file name to the frames that the portions exist in
		std::map<std::string, std::vector<int> > page_table;

		std::vector<std::vector<BYTE> > files;

		//function
		void init_pages();
		void setup_storage();
		
		long get_file_size(std::string file_path);
		bool file_exists(std::string file_path);

	public:
	 	PagingSystem(int n_frames, int s_frames, int max_file_frames);

		int get_num_frames(){ return num_frames; }
		int get_size_frames(){ return size_frames; }
		int get_max_frames_per_file(){ return frames_per_file; }

		void print_stats();

		bool load_file(std::string file_name);
};
