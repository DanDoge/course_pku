/* Author:  Huang Daoji
 * Date  :  2018-04-08
 */


/* Filename: ams_sketch.cpp
 * implementation of ams_sketch
 * not tested.
 */

/* header files */
#include <iostream.h>

/* class declaration */
class ams_sketch{
    int depth;
    int buckets;
    int count;
    int **sketch;
    int **rand_array;
    ams_sketch(){};
    ams_sketch(int d, int b){};
    int **create_rand_array(int lines, int rows){}; // a better rand function?
    int insert(obj, freq = 1){};
    int update_sketch(mult, idx, hash, freq){};
    int is_compatible(int **other_sketch){};
    int obj_count(const OBJ& obj){};
    void get_estimate(int hash, int mult, int idx, int* estimates){};
    int get_final_estimates(int* estimates){};
    int get_estimateF2(){};
};

/* function implementation */
ams_sketch::ams_sketch(){
    depth = 0;
    buckets = 0;
    count = 0;
    sketch = NULL;
    rand_array = NULL;
}

ams_sketch::ams_sketch(int d, int b){
    depth = d;
    buckets = b;
    count = 0;
    sketch = new int[depth][buckets];
    rand_array = create_rand_array(depth, buckets);
}

int **ams_sketch::create_rand_array(int lines, int rows){
    int p = new int[lines][rows];
    for(int i = 0; i < lines; i += 1){
        for(int j = 0; j < lines; j += 1){
            p[i][j] = rand() % INT_MAX;
        }
    }
    return p;
}

int ams_sketch::insert(const OBJ& obj, int freq = 1){
    for(int i = 0; i < depth; i += 1){
        int hash = get_hash_value(item, i);
        int mult = get_mult_factor(item, i);
        update_sketch(mult, i, hash, freq);
    }
    return SUCCESS;
}

int ams_sketch::update_sketch(int mult, int idx, int hash, int freq){
    if(mult & 1){
        sketch[i][hash] += freq;
    }else{
        sketch[i][hash] -= freq;
    }
}

int ams_sketch::is_compatible(int **other_sketch){
    if(buckets == other_sketch.buckets
       && depth == other_sketch.depth){
        for(int i = 0; i < depth; i += 1){
            for(int j = 0; j < buckets; j += 1){
                if(rand_array[i][j] != other_sketch.rand_array[i][j]){
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

int ams_sketch::obj_count(const OBJ& obj){
    int *estimates = new int[depth];
    for(int i = 0; i < depth; i += 1){
        int hash = get_hash_value(obj, i);
        int mult = get_mult_factor(obj, i);
        get_estimate(hash, mult, i, estimates);
    }
    return get_final_estimates(estimates);
}

int ams_sketch::get_hash_value(const OBJ& obj, int idx){
    // not implemented
}

int ams_sketch::get_mult_factor(const OBJ& obj, int idx){
    // not implemented
}

// void? better return something!
void ams_sketch::get_estimate(int hash, int mult, int idx, int* estimates){
    if(mult & 1){
        estimates[idx] += sketch[idx][hash];
    }else{
        estimates[idx] -= sketch[idx][hash];
    }
}

int ams_sketch::get_final_estimates(int* estimates){
    int count = -1;
    if(depth == 1){
        return estimates[0];
    }else{
        return get_median(estimates, depth);
    }
}

int ams_sketch::get_estimateF2(){
    int *estimates = mult_by_position(sketch, sketch);
    return get_final_estimates(estimates);
}

int* ams_sketch::mult_by_position(int** s1, int** s2){
    int res = new int[depth];
    for(int i = 0; i < depth; i += 1){
        for(int j = 0; j < buckets; j += 1){
            res[i] += s1[i][j] * s2[i][j];
        }
    }
    return res;
}

int ams_sketch::get_innerproduct(int** other_sketch){
    if(!is_compatible(other_sketch)){
        fprintf(stderr, "%s\n", "not compatible! in ams_sketch.cpp get_innerproduct()");
        return INT_MIN;
    }
    int* estimates = mult_by_position(sketch, other_sketch);
    return get_final_estimates(estimates);
}

int ams_sketch::set_zero(){
    for(int i = 0; i < depth; i += 1){
        for(int j = 0; j < buckets; j += 1){
            sketch[i][j] = 0;
        }
    }
}
