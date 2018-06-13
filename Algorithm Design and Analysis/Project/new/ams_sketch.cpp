#include "ams_sketch.h"

ams_sketch::ams_sketch(size_t c, size_t c_w, int* s): hash_table(32, cell_width, cells){
    cells = c;
    cell_width = c_w;
    seeds.resize(cells);
    for(int i = 0; i < cells; i += 1){
        seeds[i] = s[i];
    }

}

ams_sketch::insert_element(const string &ele, int freq = 1){
    for(int i = 0; i < cells; i += 1){
        char hash_val[16] = {};
        char mult_fac[16] = {};
        MurmurHash_x64_128(ele.c_str(), ele.size(), seeds[i], hash_val);
        MurmurHash_x64_128(ele.c_str(), ele.size(), seeds[i], mult_fac);
        update_sketch(*(int* )mult_fac, i, *(int* )hash_val, freq);
    }
}

int ams_sketch::update_sketch(int mult, int idx, int hash, int freq){
    if (mult & 1) {
        hash_table.inc(hash, freq, idx);
    }
    else {
        hash_table.dec(hash, freq, idx);
    }
    return 0;
}

int ams_sketch::get_estimateF2(){
    std::vector<int> estimates;
    mult_by_position(hash_table, hash_table, estimates);
    return get_final_estimates(estimates);
}

int ams_sketch::get_final_estimates(std::vector<int> &estimates) {
    vector<int> pv(estimates, p + cells);
	nth_element(pv.begin(), pv.begin() + pv.size() / 2, pv.end());
	return pv[length / 2];
}

void ams_sketch::mult_by_position(Hashtable &s1, Hashtable &s2, std::vector<int> estimates) {
	for (int i = 0; i < cells; i += 1) {
		estimates[i] = 0;
		for (int j = 0; j < cell_width; j += 1) {
			estimates[i] += s1.get(j, i) * s2.get(j, i);
		}
	}
}

int ams_sketch::obj_count(const string & ele) {
	std::vector<int> estimates(cells);
	for (int i = 0; i < cells; i += 1) {
		estimates[i] = 0;
        char hash_val[16] = {};
        char mult_fac[16] = {};
        MurmurHash_x64_128(ele.c_str(), ele.size(), seeds[i], hash_val);
        MurmurHash_x64_128(ele.c_str(), ele.size(), seeds[i], mult_fac);
		get_estimate(*(int* )hash_val, *(int* )mult_fac, i, estimates);
	}
	int ans =  get_final_estimates(estimates);
	return ans;
}

void ams_sketch::get_estimate(int hash, int mult, int idx, std::vector<int> & estimates) {
	if (mult & 1) {
		estimates[idx] += hash_table.get(hash % cell_width, idx);
	} else {
		estimates[idx] -= hash_table.get(hash % cell_width, idx);
	}
}
