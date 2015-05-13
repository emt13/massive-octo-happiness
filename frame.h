#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <vector>

typedef unsigned char BYTE;

class Frame{
	
	private:
		
		unsigned int size;
		std::vector<BYTE> data;

		unsigned int last_read;

		std::string file_name;

		int page_num;
	public: 

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

/*bool operator<(Frame &f1, Frame &f2){
	return f1.get_time_stamp() < f2.get_time_stamp();
}*/

#endif
