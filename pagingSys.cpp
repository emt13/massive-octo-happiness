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

	init_frames();
}

//initializes the frames and the page table for this system
void PagingSystem::init_frames(){
	
	//Frame::num_reads = 0;

	//setup vectors for each frame.  Could use arrays possibly since they won't resize
	for(unsigned int i = 0; i < num_frames; i++){
		//BYTE tmp_byte = 0;
		//std::vector<BYTE> tmp_vec(tmp_byte, size_frames);
		//files.push_back(tmp_vec);
		
		Frame tmp(num_frames);
		frames.push_back(tmp);
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
/*
bool PagingSystem::load_file(std::string file_name){
	
	//perform LRU on the frames
	
	return true;
}*/

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


int PagingSystem::get_LRU(std::string file_name){

	std::map<std::string, std::vector<int> >::iterator itr = page_table.find(file_name);
	//if file is already maxed out on frames, use its oldest (index 0)
	if(itr->second.size() == frames_per_file){
		int oldest = 0;
		for(unsigned int i = 1; i < itr->second.size(); i++){
			if(frames[itr->second[i]].older(frames[oldest])){
				oldest = i;
			}
		}
		return itr->second[oldest];
	}else{
		int oldest = 0;
		for(unsigned int i = 1; i < frames.size(); i++){
			if(frames[i].older(frames[oldest])){
				oldest = i;
			}
		}	
		return oldest;
	}
}

void PagingSystem::remove_frame_num_from_page(int frame_to_replace){
	std::string file_name = frames[frame_to_replace].owner();
	std::map<std::string, std::vector<int> >::iterator itr = page_table.find(file_name);
	for(unsigned int i = 0; i < itr->second.size(); i++){
		if(itr->second[i] == frame_to_replace){
			itr->second.erase(itr->second.begin() + i);
			return;
		}
	}
}

void PagingSystem::add_frame_num_to_page(std::string file_name, int frame_num){
	page_table.find(file_name)->second.push_back(frame_num);
}

void PagingSystem::store_file_data(std::string file_path, std::string file_name, int page_num, int frame_num){
	//open file
	//lseek to 1024 * (page_num - 1)
	//read in 1024, store the bytes in the frame
	int fd = open(file_path.c_str(), O_RDONLY);
	lseek(fd, (page_num-1) * size_frames, SEEK_SET);
	BYTE file_data[size_frames];

	for(unsigned int i = 0; i < size_frames; i++){
		file_data[i] = (BYTE) 0;
	}

	read(fd, file_data, size_frames);

	std::vector<BYTE> byte_vec(size_frames, (BYTE) 0);

	for(unsigned int i = 0; i < size_frames; i++){
		byte_vec[i] = file_data[i];
	}

	frames[frame_num].store_data(file_name, page_num, byte_vec);
}

int PagingSystem::page_already_loaded(std::string file_name, int page_num){
	std::map<std::string, std::vector<int> >::iterator itr = page_table.find(file_name);
	for(unsigned int i = 0; i < itr->second.size(); i++){
		int val = itr->second[i];
		if(frames[val].get_page_num() == page_num){
			return val;
		}
	}
	return -1;
}	


std::vector<BYTE> PagingSystem::read_page(std::string file_name, int offset, int amount){
	
	std::string file_path(STORAGE_STR);
	file_path.append(SLASH_STR);
	file_path.append(file_name);

	//if it is not in the page table, add it
	if(page_table.count(file_name)){
		std::vector<int> tmp;
		page_table.insert( std::pair<std::string, std::vector<int> >(file_name, tmp) );
	}
	

	//get the file size
	//calculate page number (file_size / frame_size)
	int page_num = 0;
	if(offset != 0){
		page_num = offset / size_frames + 1;
	}

	offset = offset % size_frames;

	//if that page was already loaded, use it
	int rc = page_already_loaded(file_name, page_num);
	if(rc != -1){
		return frames[rc].get_data(offset, amount);
	}

	//get the next available frame to replace according to LRU
	int frame_to_replace = get_LRU(file_name);

	//remove the frame number from that entry
	remove_frame_num_from_page(frame_to_replace);

	//add the frame number to the page table for this entry
	add_frame_num_to_page(file_name, frame_to_replace);

	//replace the frame information with the file information
	//make sure that the storing of data guarantees that the buffer maintains size	
	store_file_data(file_path, file_name, page_num, frame_to_replace);

	//display server output	

	//pull the information from the frame, return it
	return frames[frame_to_replace].get_data(offset, amount);

}








