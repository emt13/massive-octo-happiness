#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <pthread.h>
#include <vector>

typedef unsigned char BYTE;

class Frame{
	
	private:
		//keeps track of the size of the buffer		
		unsigned int size;
		std::vector<BYTE> data;
		//last time the file was read
		unsigned int last_read;
		//the owners name
		std::string file_name;
		//the page number of the owner
		int page_num;
	public: 
		//keeps track of the oldest read
		static unsigned int num_reads;
		
		Frame(int s);

		unsigned int get_time_stamp(){ return last_read; }

		std::vector<BYTE> get_data(int off, int amount);

		void store_data(std::string f_name, int p_num, std::vector<BYTE> input);
		
		int get_size(){ return size; }

		bool older(Frame& other){ return num_reads < other.get_time_stamp(); }
	 
		std::string owner(){ return file_name; }

		void purge();
		
		int get_page_num(){ return page_num; }
};

#endif
