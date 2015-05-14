# build test files
all: pagingSys.cpp frame.cpp test_page_sys.cpp
	g++ -Wall pagingSys.cpp frame.cpp test_page_sys.cpp -o test.out -pthread

clean:
	$(RM) test.out
