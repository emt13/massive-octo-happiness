/*class PagingSystem{

	private int num_frames;
	private int size_frames;

	PagingSystem(int n_frames, int f_size){
		num_frames = n_frames;
		size_frames = f_size;
	}

}*/


#include "pagingSys.h"

PagingSystem::PagingSystem(int n_frames, int f_size, int max_file_frames){
	num_frames = n_frames;
	size_frames = f_size;
	frames_per_file = max_file_frames;

	init_pages();
}

//initializes the frames and the page table for this system
void PagingSystem::init_pages(){
	//setup vectors for each frame.  Could use arrays possibly since they won't resize
	for(unsigned int i = 0; i < num_frames; i++){
		BYTE tmp_byte = 0;
		std::vector<BYTE> tmp_vec(tmp_byte, size_frames);
		files.push_back(tmp_vec);
	}		
}

//sets up the storage file to hold all of the saved information
void PagingSystem::setup_storage(){
	//check if the storage file exists
	struct stat stats;
	if(lstat(STORAGE_STR, &stats) == -1){
		mkdir(STORAGE_STR, DIR_PERMISSIONS);
		std::cout<<" -- created storage file"<<std::endl;
	}	
}

void PagingSystem::print_stats(){
	printf("# frames: %d\nsize of frames: %d\nframes per file: %d\ntotal created frames: %lu\n", num_frames, size_frames, frames_per_file, files.size());
}

//return the size of the file at the given path, -1 if it cannot be found
long PagingSystem::get_file_size(std::string file_path){
	struct stat info;
	int rc = stat(file_path.c_str(), &info);
	return rc == 0 ? info.st_size : -1;
}

//checks if there is a file at the given path
bool PagingSystem::file_exists(std::string file_path){
	struct stat info;
	int rc = stat(file_path.c_str(), &info);
	return rc == 0;
}


bool PagingSystem::load_file(std::string file_name){
	
	std::string file_path(STORAGE_STR);
	file_path.append(SLASH_STR);
	file_path.append(file_name);

	if(!file_exists(file_path)){
		printf(" ERROR: file, %s, does not exist!\n", file_path.c_str());
		return false;
	}

	long file_size = get_file_size(file_path);

	printf(" - file size: %ld\n", file_size);

	std::cout<<"file path: "<<file_path<<std::endl;
	return true;


}


