#include <iostream>
#include <stdio.h>
#include "pagingSys.h"
#include <fcntl.h>

typedef unsigned char BYTE;

PagingSystem* ps;

void print_data(std::vector<BYTE>& data){
	printf("\n");
	for(unsigned int i = 0; i < data.size(); i++){
		printf("%c", (char) data[i]);
	}
	printf("\n");
}

void test_dir(){
	printf("--- testing dir: \n");
	std::vector<std::string> files = ps->dir();
	for(unsigned int i = 0; i < files.size(); i++){
		printf(" * %s\n", files[i].c_str());
	}
	printf("--- finished dir test\n\n");
}

void test_store(){
	printf("--- testing store \n");
	std::vector<BYTE> data;
	data.push_back( (BYTE) 0xde);
        data.push_back( (BYTE) 0xad);
	data.push_back( (BYTE) 0xbe);
	data.push_back( (BYTE) 0xef);
	
	std::string file_name1(".storage/test1.bin");
	printf("   = creating file: %s\n", file_name1.c_str());
	
	ps->store(std::string("test1.bin"), data);

	std::string file_name2(".storage/to_be_delete.txt");
	printf("   = creating file: %s\n", file_name2.c_str());

	std::vector<BYTE> data1;
	data1.push_back( (BYTE) 'A');
	data1.push_back( (BYTE) 'B');
	data1.push_back( (BYTE) 'C');

	ps->store(std::string("to_be_delete.txt"), data1);

	printf("\n--- finished store test\n\n");
}

void test_delete(){
	printf("--- testing delete\n");
	
	printf(" : deleting: to_be_delete.txt\n");
	ps->delete_file(std::string("to_be_delete.txt"));
	
	printf("--- finished testing delete\n\n");
}

void test_read(){
	printf("--- testing read\n");

	//int off = 4888;
	//int num = (5*1024-1) - off;

	
	int off = 0;
	int num = 1023;

	printf("  # reading in at %d, %d\n", off, num);
	
	std::string file_path(".storage/sample.txt");
	std::string file_name("sample.txt");
	std::vector<BYTE> data = ps->read_page(file_name, off, num);
	
	print_data(data);

	printf("--- finished testing read\n\n");
}

int main(){

	ps = new PagingSystem(32, 1024, 4);

	test_dir();

	test_store();

	test_dir();

	test_delete();

	test_dir();
	
	test_read();
}
