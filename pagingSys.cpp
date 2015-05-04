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
}
