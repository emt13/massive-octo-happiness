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
		//BYTE tmp_byte = 0;
		//std::vector<BYTE> tmp_vec(tmp_byte, size_frames);
		//files.push_back(tmp_vec);
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
	printf("# frames: %d\nsize of frames: %d\nframes per file: %d\n", num_frames, size_frames, frames_per_file);
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
	return true;
}

int PagingSystem::store(std::string file_name, std::vector<BYTE> data){
	
	struct stat info;
	std::string file_path(STORAGE_STR);
	file_path.append(SLASH_STR);
	file_path.append(file_name);

	int rc = stat(file_path.c_str(), &info);

	//file doesn't exist
	if(rc != 0){
		int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, DIR_PERMISSIONS);
		std::cout<<"got fd - "<<fd<<std::endl;
		int total = 0;
		for(unsigned int i = 0; i < data.size(); i++){
			BYTE arr[1] = {data[i]};
			total += write(fd, arr, sizeof(BYTE));
		}
		close(fd);
		return total;
	}else{//file does exist
		return -1;
	}
}	


std::vector<std::string> PagingSystem::dir(){
	
	std::vector<std::string> file_names;

	DIR *dir;
	struct dirent *dp;
	if((dir = opendir(STORAGE_STR)) != NULL){
		while((dp = readdir(dir)) != NULL){
			std::string f_name(dp->d_name);
			if(f_name != std::string(CURR_DIR_STR) && f_name != std::string(PREV_DIR_STR)){
				file_names.push_back(f_name);
			}
		}
		closedir(dir);
	}

	return file_names;


}


int PagingSystem::delete_file(std::string file_name){
	
	std::string file_path(STORAGE_STR);
	file_path.append(SLASH_STR);
	file_path.append(file_name);

	//go through the page table and remove all of the files contents from the frames
	
	//delete the actual file from the storage
	return remove(file_path.c_str());	
}




