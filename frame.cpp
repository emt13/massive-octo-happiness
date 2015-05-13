#include "frame.h"

unsigned int Frame::num_reads = 0;

Frame::Frame(int s){
	size = (unsigned int) s;
	data = std::vector<BYTE>(size, (BYTE) 0);
	//last_read = num_reads;
	last_read = 0;
	file_name = "";
}

//the offset of the frame and then the amount of bytes from that frame
std::vector<BYTE> Frame::get_data(int off, int amount){

	num_reads++;

	last_read = num_reads;

	std::vector<BYTE> ret_data;

	if(off < 0 || off >= size){ return ret_data; }

	for(unsigned int i = off; i < ret_data.size(); i++){
		ret_data.push_back(data[i]);
		amount--;
		if(amount == 0){ break; }
	}


	return ret_data;
}

void Frame::store_data(std::string f_name, int p_num, std::vector<BYTE> input){
	file_name = f_name;
	page_num = p_num;
	
	//copy over all of the input data to the internal data
	for(unsigned int i = 0; i < input.size() && i < size; i++){
		data[i] = input[i];
	}
}
