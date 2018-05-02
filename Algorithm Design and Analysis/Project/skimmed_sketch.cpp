/* Author:  Huang Daoji
 * Date  :  2018-05
 */


/* Filename: skim_sketch.cpp
 * implementation of skim_sketch
 * not tested.
 */

/* 主要解决的是两个集合的并集的元素个数, 要求给出全集的元素
 * 需要生成固定的一些hash函数
 * 要求four-wise independent的一个二维数组
 */

#include <iostream>

class flow{

}

class skimmed_sketch{
    int domain_size;
    map<int, T> domain_value;
    int s1;// s1个hash函数
    int s2;// hash表有s2项
    int** epsilon;// s1 * domain_size, 要求four-wise independent
    int hash_function[](int );// s1个hash函数

    skimmed_sketch(){
        domain_size = 0;
        domain_value.clear();
        s1 = 0;
        s2 = 0;
        epsilon =NULL;
    }
    skimmed_sketch(d_s, s11, s22, map<int, T> d_v){
        domain_size = d_s;
        s1 = s11;
        s2 = s22;
        epsilon = new int[s1][domain_size];
        domain_value = d_v;//拷贝构造
        hash_function = ;//???
    }

    int* hash(FLOW f){

    }


}
