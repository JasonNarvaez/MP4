#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "reqchannel.h"
#include "semaphore.h"

using namespace std;

struct BoundedBuffer {
	
	
	
	BoundedBuffer(): empty(Semaphore(200)), full(Semaphore(0)), mutex(Semaphore(1)) {}//default constructor
	BoundedBuffer(int b): empty(Semaphore(b)), full(Semaphore(0)), mutex(Semaphore(1)) { 
		//Semaphore empty(b);
		//Semaphore full(0);	//how many full slots
		//Semaphore mutex(1);
	
	}
	~BoundedBuffer(){
		
	}
	void add(string str);
	string remove();
	
	queue<string> data;
	//vector<string> data;//use mutex to protect against race conditions with data vector
	Semaphore empty;//b is the size of the buffer, empty counts how many empty slots 
	Semaphore full;	//how many full slots
	Semaphore mutex;//using the semaphore as a mutex, the binary semaphore. guardes multiple access on shared variable

};
