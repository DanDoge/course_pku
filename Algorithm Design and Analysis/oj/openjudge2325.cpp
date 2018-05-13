#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
using namespace std;

int f(int k, int* n, int length){
    int tmp[1024] = {};
    int res = 0;
    for(int i = length - 1; i >= 0; i -= 1){
        res *= 10;
        res += n[i];
        tmp[i] = res / k;
        res = res % k;
    }
    if(res == 0){
        bool valid = 0;
        for(int i = 1023; i >= 0; i -= 1){
            valid = valid | tmp[i];
            n[i] = valid? tmp[i] : 0;
        }
        return 1;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    char in[1024] = {};
    while(cin >> in && in[0] != '-'){
        if(strlen(in) == 1){
            cout << 1 << in << endl;
            continue;
        }
        int length = strlen(in);
        int n[1024] = {};
        for(int i = 0, j = length - 1; i < length; i += 1, j -= 1){
            n[j] = in[i] - '0';
        }
        vector<int> vec;
        int is_div = 0;
        for(int i = 9; i >= 2; i -= 1){
            if(f(i, n, length)){
                vec.push_back(i);
                is_div = 1;
                i += 1;
            }
        }
        int flag = 0;
        for(int i = 1023; i >= 1; i -= 1){
            if(n[i] != 0){
                flag = 1;
            }
        }
        if(vec.empty() || flag){
            cout << "There is no such number." << endl;
        }else{
            sort(vec.begin(), vec.end());
            for(int i = 0; i < vec.size(); i += 1){
                cout << vec[i];
            }
            cout << endl;
        }
    }
    return 0;
}
