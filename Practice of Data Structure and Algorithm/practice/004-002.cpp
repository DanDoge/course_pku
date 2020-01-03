#include <iostream>
#include <queue>
#include <string.h>
using namespace std;

int tr[250010][26];
int tr_top = 0;
int e[250010];
int fail[250010];
char sen[5100010];
char tmp[5100010];
char pat[1024];

void init(){
	memset(tr, 0, sizeof(tr));
	memset(e, 0, sizeof(e));
	tr_top = 0;
	memset(fail, 0, sizeof(fail));
}

void insert(char* s){
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        if(!tr[idx][s[i] - 'A']){
            tr[idx][s[i] - 'A'] = ++tr_top;
        }
        idx = tr[idx][s[i] - 'A'];
    }
    e[idx] += 1;
    return ;
}

void build(){
    queue<int> q;
    while(!q.empty()){
    	q.pop();
	}
    for(int i = 0; i < 26; i += 1){
        if(tr[0][i]){
            q.push(tr[0][i]);
        }
    }
    while(q.size()){
        int idx = q.front();
        q.pop();
        for(int i = 0; i < 26; i += 1){
            if(tr[idx][i]){
                fail[tr[idx][i]] = tr[fail[idx]][i];
                q.push(tr[idx][i]);
            }else{
                tr[idx][i] = tr[fail[idx]][i];
            }
        }
    }
    return ;
}

int match(char* s){
    int res = 0;
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        idx = tr[idx][s[i] - 'A'];
        for(int j = idx; j && e[j] != -1; j = fail[j]){
            res += e[j];
            e[j] = -1;
        }
    }

    int len =  strlen(s);
    idx = 0;
    for(int i = len - 1; i >= 0; i -= 1){
        idx = tr[idx][s[i] - 'A'];
        for(int j = idx; j && e[j] != -1; j = fail[j]){
            res += e[j];
            e[j] = -1;
        }
    }
    return res;
}

void input(){
    cin >> tmp;
    int idx_sen = 0;
    for(int i = 0; tmp[i]; ){
        if(tmp[i] != '['){
            sen[idx_sen] = tmp[i];
            idx_sen += 1;
            i += 1;
        }else{
            i += 1;
            int q = 0;
            while(tmp[i] >= '0' && tmp[i] <= '9'){
                q *= 10;
                q += tmp[i] - '0';
                i += 1;
            }
            for(int j = 0; j < q; j += 1){
                sen[idx_sen] = tmp[i];
                idx_sen += 1;
            }
            i += 2;
        }
    }
    sen[idx_sen] = 0;
    return ;
}

int main(int argc, char const *argv[]) {
    int num_cases;
    cin >> num_cases;
    while(num_cases--){
    	init();
        int num_pat;
        cin >> num_pat;
        for(int i = 0; i < num_pat; i += 1){
            cin >> pat;
            insert(pat);
        }
        build();
        input();
        cout << match(sen) << endl;
    }
    return 0;
}
