#include <iostream>
#include "thread.h"
#include <vector>
#include <fstream> 
#include <deque> 

using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using std::deque;

mutex the_mutex; 
cv the_cv; 

vector<deque<int>> input; 
vector<int> current_queue; 

size_t max_disk_queue;
int current_track = 0;
int num_requester;
bool queue_is_optimal = false; 

void requester(int a);
void read_input(int argc, char *argv[]);
void server(int argc, char *argv[]);


int main(int argc, char *argv[]) {
	max_disk_queue = atoi(argv[1]); 

	num_requester = argc - 2; 

	read_input(argc, argv); 

	cpu::boot((thread_startfunc_t) server, (void *) 100, 0);

	cout << "main reach end" << endl;
	return 0; 
}

void read_input(int argc, char *argv[]) {
	input.resize(argc - 2); 
	int temp; 

	for (int i = 2; i < argc; i++) {
		ifstream infile (argv[i]);
		while(infile >> temp) {
			input[i - 2].push_back(temp);
		}
	}
	cout << "read reach end" << endl; 
}

void server(int argc, char *argv[]) { //性感荷官在线发牌
	vector<thread*> threads; 

	for (int i = 0; i < num_requester; i++) {
		threads.push_back(new thread((thread_startfunc_t) requester, (void *) i));
	}

	for (int i = 0; i < 3; i++) {
		cout << current_queue[i] << " ";
	}

	for (int i = 0; i < num_requester; i++) {
		threads[i]->join();
	}

	cout << "server reach end" << endl;
}


void requester(int a) {
	while(!input[a].empty()) {
		the_mutex.lock();
		if (current_queue.size() < max_disk_queue){
			current_queue.push_back(input[a].front());
			input[a].pop_front(); 
		}
		the_mutex.unlock();
	}
	cout << "requester " << a << " reach end" << endl; 
}

