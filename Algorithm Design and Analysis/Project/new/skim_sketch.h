#ifndef STREAMINGLIB_SKIM_SKETCH_H
#define STREAMINGLIB_SKIM_SKETCH_H

#include <set>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <stdlib.h>

#include "hash.h"
#include "hashtable.h"
using namespace std;

#define s1 = 64;
#define s2 = 128;

class skim_sketch{
public:
    long long join_size;
    skim_sketch();
    skim_sketch(std::vector<int> f1, std::vector<int> f2, int domain_size);
private:
    Hashtable h1;
    Hashtable h2;
    std::vector<int> e1;
    std::vector<int> e2;
    int flow_length1;
    int flow_length2;
    int est_skim_join_size();
    void skim_dense(Hashtable & h, int & flow_length, std::vector<int> & e);
    int est_sub_join_size(std::vector<int> & e, Hashtable & h);
};

void get_hash_table(std::vector<int> & f, Hashtable & h){
    for(int i = 0; i < f.size(); i += 1){
        for(int j = 0; j < s1; j += 1){
            h[j][hash_template(j, f[i])] += ((hash_template(f[i], j) & 1) << 1) - 1;
        }
    }
}


int inner_product(vector<int> &p, vector<int> &q, int length){
    int res = 0;
    for(int i = 0; i < length; i += 1){
        res += p[i] * q[i];
    }
    return res;
}

int median(vector<int> p, int length){
    vector<int> pv(p);
    nth_element(pv.begin(), pv.begin() + pv.size() / 2, pv.end());
    return pv[length / 2];
}

int hash_template(int seed, int n) {
	uint64_t *res = (uint64_t* )malloc(sizeof(uint64_t) * 2);
	MurmurHash3_x64_128(&n, 1, seed, res);

	int ans = (*res) & (s2 - 1);
	free(res);
	return ans;
}

#endif // STREAMINGLIB_SKIM_SKETCH_H
