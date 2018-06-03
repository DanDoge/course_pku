#ifndef STREAMING_LIB_SPACEVING_H
#define STREAMING_LIB_SPACEVING_H

#include <vector>

struct node{
    int value;
    int freq;
    int over_est;
    node(){
        value = 0;
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
            heap[used] = heap(val, freq);
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
    int query_frequent(int m, double support_ratio){
        double sr = support_ratio;
        int guaranteed = 1;

        int idx = 0;
        for(int i = size - 1; i >= 0 && idx < m; i -= 1){
            if(heap[i].freq > sr * n){
                std::cout << heap[i].value << std::endl;
                idx += 1;
                if(heap[i].freq - heap[i].over_est < sr * n){
                    guaranteed = 0;
                }
            }
        }

        return guaranteed;
    }
    int query_top(int m, int k){
        int order = 1;
        int guaranteed = 0;
        int min_guar_freq = INT_MAX;

        for(int i = size - 1; i >= size - k; i -= 1){
            std::cout << heap[i].value << std::endl;
            if(heap[i].freq - heap[i].over_est < min_guar_freq){
                min_guar_freq = heap[i].freq - heap[i].over_est;
            }
            if(heap[i].freq - heap[i].over_est < heap[i - 1].freq){
                order = 0;
            }
        }

        if(heap[size - k - 1].freq <= min_guar_freq){
            guaranteed = 1;
        }else{
            std::cout << heap[size - k - 1].freq << std::endl;
            for(int i = size - k - 2; i >= 0; i -= 1){
                if(heap[i].freq - heap[i].over_est < min_guar_freq){
                    min_guar_freq = heap[i].freq - heap[i].over_est;
                }
                if(heap[i - 1].freq < min_guar_freq){
                    guaranteed = 1;
                    break;
                }
                std::cout << heap[i].freq << std::endl;
            }
        }
        return guaranteed || order;
    }
    void heapify(int idx){
        int left_son = (idx << 1) + 1;
        int right_son = (idx << 1) + 2;
        if(right_son < size){
            int min_val = min(heap[idx].value, heap[left_son].value, heap[right_son].value);
            if(min_val == heap[idx].value){
                return ;
            }else if(min_val == heap[left_son].value){
                swap(heap[idx], heap[left_son]);
                heapify(left_son);
                return ;
            }else{
                swap(heap[idx], heap[right_son]);
                heapify(right_son);
                return ;
            }
        }else if(left_son < size){
            int min_val = min(heap[idx].value, heap[left_son].value);
            if(min_val == heap[idx].value){
                return ;
            }else{
                swap(heap[idx], heap[left_son]);
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
