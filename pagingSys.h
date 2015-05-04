// Evan Thompson, Jack Cusick, Tausif Ahmed

//Perform LRU on replacement of frames
//
//Allocate frames only on demand

typedef unsigned char BYTE;

class PagingSystem{

	private: int num_frames;
		 int size_frames;
		 int frames_per_file;

	public:
	 	PagingSystem(int n_frames, int s_frames, int max_file_frames);

		int get_num_frames(){ return num_frames; }
		int get_size_frames(){ return size_frames; }
		int get_max_frames_per_file(){ return frames_per_file; }
};
