#include <iostream>
#include <algorithm>
#include <string.h>
using namespace std;

int k[210];
char str[4010][210];
char tmp[210];
int num_str;
int len_tmp, len_pat; 

void get_k(){
    int pos = 0;
    for(int i = 2; i <= len_tmp; i += 1){
        while(pos > 0 && tmp[pos + 1] != tmp[i]){
            pos = k[pos];
        }
        if(tmp[pos + 1] == tmp[i]){
            pos += 1;
        }
        k[i] = pos;
    }
}

int kmp(int idx){
    int res = 0, p = 0;
    for(int i = 1; i <= len_pat; i += 1){
        while(p > 0 && tmp[p + 1] != str[idx][i]){
            p = k[p];
        }
        if(tmp[p + 1] == str[idx][i]){
            p += 1;
        }
        res = max(p, res);
        if(p == len_tmp){
            break;
        }
    }
    return res;
}

int comp(int p, int q, int res){
    for(int i = 0; i < res; i += 1){
        if(str[0][p + i] != str[0][q + i]){
            return str[0][p + i] < str[0][q + i];
        }
    }
    return true;
}

int main(int argc, char const *argv[]) {
    while(cin >> num_str && num_str){
        for(int i = 0; i < num_str; i += 1){
            cin >> str[i] + 1;
        }

        int ans = 0, rec = 0;
        len_pat = strlen(str[0] + 1);
        for(int i = 1; i < len_pat; i += 1){
            strcpy(tmp + 1, str[0] + i);
            len_tmp = strlen(tmp + 1);

            get_k();

            int tmpp = len_pat;
            for(int j = 1; j < num_str; j += 1){
                tmpp = min(tmpp, kmp(j));
            }

            if(tmpp > ans || (tmpp == ans && comp(i, rec, ans))){
                ans = tmpp;
                rec = i;
            }
        }
        if(ans){
            for(int i = 0; i < ans; i += 1){
                cout << str[0][rec + i];
            }
            cout << endl;
        }else{
            cout << "IDENTITY LOST" << endl;
        }
    }
    return 0;
}
