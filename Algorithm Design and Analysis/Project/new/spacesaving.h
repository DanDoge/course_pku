#ifndef STREAMING_LIB_SPACEVING_H
#define STREAMING_LIB_SPACEVING_H

#include <vector>
#include <iostream>
#include <algorithm>

struct node{
    int value;
    int freq;
    int over_est;
    node(){
        value = -1;
        freq = -1;
        over_est = 0;
    }
    node(int v, int f){
        value = v;
        freq = f;
        over_est = 0;
    }
    friend operator<(const node& obj1, const node& obj2){
        return obj1.freq < obj2.freq;// well...i did not use it!
    }
};

class space_saving{
public:
    std::vector<node> heap; // should be an array
    int size;
    int used;
    int n;
    space_saving(){
        used = 0;
        size = 0;
        heap.resize(size);
    }
    space_saving(int s){
        used = 0;
        size = s;
        heap.resize(size);
    }
    void insert(int val, int freq = 1){
        n += freq;
        if(used < size){
            heap[used] = node(val, freq);
            used += 1;
            if(used == size){
                make_heap();
            }
            return ;
        }
        for(int i = 0; i < size; i += 1){
            if(heap[i].value == val){
                heap[i].freq += freq;
                heapify(i);
                return ;
            }
        }
        heap[0].value = val;
        heap[0].over_est = heap[0].freq;
        heap[0].freq += 1;
        heapify(0);
    }
    void query_top(std::vector<int> & res){
        res.assign(heap.begin(), heap.end());
    }
private:
    void heapify(int idx){
        int left_son = (idx << 1) + 1;
        int right_son = (idx << 1) + 2;
        if(right_son < size){
            int min_val = std::min(heap[idx].freq, std::min(heap[left_son].freq, heap[right_son].freq));
            if(min_val == heap[idx].freq){
                return ;
            }else if(min_val == heap[left_son].freq){
                std::swap(heap[idx], heap[left_son]);
                heapify(left_son);
                return ;
            }else{
                std::swap(heap[idx], heap[right_son]);
                heapify(right_son);
                return ;
            }
        }else if(left_son < size){
            int min_val = std::min(heap[idx].freq, heap[left_son].freq);
            if(min_val == heap[idx].value){
                return ;
            }else{
                std::swap(heap[idx], heap[left_son]);
                heapify(left_son);
                return ;
            }
        }else{
            return ;
        }
        return ;
    }
    void make_heap(){
        for(int idx = (size - 1) / 2; idx >= 0; idx -= 1){
            heapify(idx);
        }
    }
};

#endif // STREAMING_LIB_SPACEVING_H
