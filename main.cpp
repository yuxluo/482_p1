#include <iostream>
#include "thread.h"
#include <vector>
#include <fstream> 
#include <deque> 
#include "disk.h"
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using std::deque;

mutex the_mutex; 
cv the_cv; 

vector<deque<int>> input; 
vector<std::pair<int,int>> current_queue; 
//<requester ID, request ID>


size_t max_disk_queue;
int current_track = 0;
int num_requester;
bool queue_is_optimal = false; 
size_t global_requests_left = 1024;

void requester(int a);
void read_input(int argc, char *argv[]);
void server(int argc, char *argv[]);
void check_optimal();
bool cmp(std::pair<int,int> a, std::pair<int,int> b); 

int main(int argc, char *argv[]) {
	max_disk_queue = atoi(argv[1]); 

	num_requester = argc - 2; 

	read_input(argc, argv); 

	cpu::boot((thread_startfunc_t) server, (void *) 100, 0);

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
}

void server(int argc, char *argv[]) { //性感荷官在线发牌
	vector<thread*> threads; 

	for (int i = 0; i < num_requester; i++) {
		threads.push_back(new thread((thread_startfunc_t) requester, (void *)(uintptr_t) i));
	}

	 while(global_requests_left) {
	 	
	 	the_mutex.lock();
	 	check_optimal(); 
	 	if (queue_is_optimal && !current_queue.empty()) {
	 		std::sort(current_queue.begin(), current_queue.end(), cmp);
	 		print_service(current_queue.back().first, current_queue.back().second);
	 		current_track = current_queue.back().second;
	 		current_queue.pop_back();
	 	}
	 	the_mutex.unlock();
	}

	

	for (int i = 0; i < num_requester; i++) {
		threads[i]->join();
	}

}


void requester(int a) {
	while(!input[a].empty()) {
		the_mutex.lock();
		if (current_queue.size() < max_disk_queue){
			current_queue.push_back(std::pair<int,int>(a, input[a].front()));
			print_request(a, input[a].front());
			input[a].pop_front();
		}
		the_mutex.unlock();
	}
}

void check_optimal() {
	size_t requests_left = 0; 
	for (int i = 0; i < num_requester; i++) {
		requests_left += input[i].size();
	}
	global_requests_left = requests_left + current_queue.size(); 
	if (current_queue.size() == max_disk_queue || requests_left == 0) {
		queue_is_optimal = true;
	}
	else {
		queue_is_optimal = false; 
	}
}

//a = current track
bool cmp(std::pair<int,int> a, std::pair<int,int> b){
	int cur = current_track; 
    return abs(a.second - cur) > abs(b.second - cur);
}
