#ifndef STREAMINGLIB_AMS_SKETCH_H
#define STREAMINGLIB_AMS_SKETCH_H

#include <string>
#include <cstdlib>

#include "hash.h"
#include "hashtable.h"
using namespace std;

class ams_sketch{
public:
    int count;
    ams_sketch(size_t c, size_t c_w, int* s);
    int insert_element(const string & ele, int freq = 1);
    int get_estimateF2();
    int obj_count(const string & ele);
private:
    Hashtable hash_table;
    std::vector<int> seeds;
    size_t cells;
    size_t cell_width;

    int update_sketch(int mult, int idx, int hash, int freq);
    void get_estimate(int hash, int mult, int idx, std::vector<int> & estimates);
    int* mult_by_position(Hashtable s1, Hashtable s2);
    int get_final_estimates(std::vector<int> &estimates)
};

#endif // STREAMINGLIB_AMS_SKETCH_H
