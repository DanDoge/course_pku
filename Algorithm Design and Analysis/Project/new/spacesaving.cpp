#include "spacesaving.h"
#include <cstdlib>
#include <vector>

std::vector<node> counter(10000);

int main(){
	srand(192608);
    space_saving ss(50);
    for(int i = 0; i < 10000; i += 1){
    	counter[i].value = i;
    	counter[i].freq = 0;
	}
    for(int i = 0; i < 100; i += 1){
    	ss.insert(i, 100 - i);
    	counter[i].freq = 100 - i;
	}
	for(int i = 0; i < 50; i += 1){
		std::cout << ss.heap[i].value << ' ' << ss.heap[i].freq << std::endl;
	}
	ss.query_frequent(10, 0);
    return 0;
}
