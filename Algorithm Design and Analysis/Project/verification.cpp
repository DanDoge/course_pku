#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdlib.h>
using namespace std;


vector<int> v1(1000000);
vector<int> v2(1000000);

int main(){
	std::ios::sync_with_stdio(false);
    ifstream in1;
    in1.open("C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project\\skimmed-sketch-test1");
    int size = 0;
    in1 >> size;
    for(int i = 0; i < size; i += 1){
        int tmp = 0;
        in1 >> tmp;
        v1[tmp] += 1;
    }
    ifstream in2;
    in2.open("C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project\\skimmed-sketch-test2");
    in2 >> size;
    for(int i = 0; i < size; i += 1){
        int tmp = 0;
        in2 >> tmp;
        v2[tmp] += 1;
    }
    cout << "!" << endl;
    int ans = 0;
    for(int i = 0; i < 1000000; i += 1){
        ans += v1[i] * v2[i];
    }
    cout << ans << endl;
	ofstream out;
	out.open("C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project\\exact_ans");
	out << ans;
	return 0;
}
