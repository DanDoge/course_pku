#include <iostream>
#include <queue>
using namespace std;

int tr[120010][26];
int tr_end = 0;
int fail[120010];
int e[120010];
int n, m;
char tmp[1024];

void add_string(char* s){
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        if(!tr[idx][s[i] - 'a']){
            tr[idx][s[i]-  'a'] = ++tr_end;
        }
        idx = tr[idx][s[i]-  'a'];
    }
    e[idx] += 1;
    return ;
}

void build(){
    queue<int> q;
    for(int i = 0; i < 26; i += 1){
        if(tr[0][i]){
            q.push(tr[0][i]);
        }
    }
    while(!q.empty()){
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
        idx = tr[idx][s[i] - 'a'];
        for(int j = idx; j && e[j] != -1; j = fail[j]){
            res += e[j];
            //e[j] = -1;
        }
    }
    return res;
}

int main(int argc, char const *argv[]) {
    cin >> n;
    while(n--){
        cin >> tmp;
        add_string(tmp);
    }
    build();
    cin >> m;
    while(m--){
        cin >> tmp;
        if(match(tmp)){
            cout << "YES" << endl;
        }else{
            cout << "NO" << endl;
        }
    }
    return 0;
}
