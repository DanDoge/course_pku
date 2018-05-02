/* Author:  Huang Daoji
 * Date  :  2018-05
 */


/* Filename: skim_sketch.cpp
 * implementation of skim_sketch
 * not tested.
 */



/* 主要解决的是两个集合的并集的元素个数, 要求给出全集的元素和元素个数
 * 给定两个流, 需要流的大小
 * 需要生成固定的一些hash函数
 * 要求four-wise independent的一个二维数组
 */

// to-do: 没有必要设计三个类出来, 事实上两个流是要共享很多东西的

#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdlib.h>
using namespace std;

#define T int
#define s1 6 // hash函数个数
#define s2 64// 桶的个数

static int inner_product(int* p, int* q, int length){
    int res = 0;
    for(int i = 0; i < length; i += 1){
        res += p[i] * q[i];
    }
    return res;
}

static int median(int* p, int length){
    vector<int> pv(p, p + length);
    // 懒了
    nth_element(pv.begin(), pv.begin() + pv.size() / 2, pv.end());
    return pv[length / 2];
}


int hash_template(int seed, int n){
    srand(seed);
    seed = rand();
    seed += (seed << 1) + (seed << 4) + (seed << 7) + (seed << 8) + (seed << 24);
    return (n ^ seed) % s2;
}

// 实际实现的时候完全可以不用它, 整数用起来挺舒服的, 有必要用map嘛...?
/*
class domain{
public:
    int domain_size;
    map<int, T> domain_value;
    domain(){
        domain_size = 0;
        domain_value.clear();
    }
    domain(int s, map<int, T> d_v){
        domain_size = s;
        domain_value = map<int, T>(d_v); // or domain_value = d_v;
    }
    ~domain(){
        domain_size = 0;
        domain_value.clear();
    }
};
*/

/* only for test */

class domain{
public:
    int domain_size;
    domain(){
        domain_size = 1000;
    }
    int domain_value(int n){
        return n;
    }
};

class FLOW{
public:
	int flow_length;
    int* flow_element;
    FLOW(int f_l){
        flow_length = f_l;
        flow_element = new int[flow_length];
    }
    ~FLOW(){
        delete[] flow_element;
    }
};

class hash{
public:
	int m;
    int hashtable[s1][s2]; // s1 * s2, s1个hash函数, s2项
    int (*epsilon)[s1];// m * s1, 全集有m个元素, 这个数组要求是four-wise independent的
    int* estimate_value;
    hash(){
        m = 0;
    }
    hash(domain d, FLOW flow){
        //to-do:这个函数设计似乎有点问题, 最好不要每一次为一个流构造一个hash实例, 返回一个hashtable就好?
        m = d.domain_size;
        epsilon = new int[m][s1];
        estimate_value = new int[m];
        for(int i = 0; i < flow.flow_length; i += 1){
            for(int j = 0; j < s1; j += 1){
                hashtable[j][hash_template(j, flow.flow_element[i])] += epsilon[j][i];
            }
        }
    }
    ~hash(){
        delete[] epsilon;
        delete[] estimate_value;
    }
};

class skim_sketch{
public:
    int threhold;
    int join_size;
    hash h1;
    hash h2;
    skim_sketch(){
        threhold = join_size = 0;
    }
    skim_sketch(int t, FLOW f1, FLOW f2, domain d){
        threhold = t;
        hash h1 = hash(d, f1);
        hash h2 = hash(d, f2);
        join_size = est_skim_join_size(h1, h2, d);
    }
    void skim_dense(hash h, domain d){
        cout << "in skim_dense..." << endl;
        for(int i = 0; i < d.domain_size; i += 1){
            h.estimate_value[i] = 0;
        }
        set<T> E;
        for(int i = 0; i < d.domain_size; i += 1){
            T u = d.domain_value(i);
            int* f = new int[s1];
            for(int j = 0; j < s1; j += 1){
                f[j] = h.hashtable[j][hash_template(j, u)] * h.epsilon[u][j];
            }
            int est_u = median(f, s1);
            if(est_u > 2 * threhold){
                h.estimate_value[i] = est_u;
                E.insert(u);
            }
            delete[] f;
        }
        for(int i = 0; i < d.domain_size; i += 1){
            for(int j = 0; j < s1; j += 1){
                int q = hash_template(j, d.domain_value(i));
                h.hashtable[j][q] = h.hashtable[j][q] - h.estimate_value[i] * h.epsilon[j][d.domain_value(i)];
            }
        }
        cout << "out skim_dense." << endl;
    }

    int est_sub_join_size(int* v, hash h, domain d){
        cout << "in est_sub_join_size..." << endl;
        int* j = new int[s1];
        for(int p = 0; p < s1; p += 1){
            j[p] = 0;
            for(int i = 0; i < d.domain_size; i += 1){
                if(v[d.domain_value(i)] > 0){
                    int q = hash_template(p, d.domain_value(i));
                    j[p] = j[p] + h.hashtable[p][q] * v[d.domain_value(i)] * h.epsilon[p][d.domain_value(i)];
                }
            }
        }
        int res = median(j, s1);
        delete j;
        cout << "out est_sub_join_size" << endl;
        return res;
    }

    int est_skim_join_size(hash h1, hash h2, domain d){
        cout << "in est_skim_join_size..." << endl;
        skim_dense(h1, d);
        skim_dense(h2, d);
        int j_dd = inner_product(h1.estimate_value, h2.estimate_value, d.domain_size);
        int j_ds = est_sub_join_size(h1.estimate_value, h2, d);
        int j_sd = est_sub_join_size(h2.estimate_value, h1, d);
        int* j_sss = new int[s1];
        for(int p = 0; p < s1; p += 1){
            j_sss[p] = 0;
            for(int q = 0; q < s2; q += 1){
                j_sss[p] += h1.hashtable[p][q] * h2.hashtable[p][q];
            }
        }
        int j_ss = median(j_sss, s1);
        cout << "out est_skim_join_size." << endl;
        return j_dd + j_ds + j_sd + j_ss;
    }
};

int main(){
    ifstream in1;
    in1.open("skimmed-sketch-test1");
    int f_l1 = 0;
    in1 >> f_l1;
    FLOW f1 = FLOW(f_l1);
    for(int i = 0; i < f_l1; i += 1){
        in1 >> f1.flow_element[i];
    }
    ifstream in2;
    in2.open("skimmed-sketch-test2");
    int f_l2 = 0;
    in2 >> f_l2;
    FLOW f2 = FLOW(f_l2);
    for(int i = 0; i < f_l2; i += 1){
        in2 >> f2.flow_element[i];
    }
    domain d;
    skim_sketch ss = skim_sketch(0, f1, f2, d);
    cout << ss.join_size;
    return 0;
}
