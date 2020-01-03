#include <iostream>
#include <queue>
using namespace std;

int tr[120010][26];
int fail[120010];
int e[120010];
int top;
char pat[128];
char sen[1024];

void insert(char* s){
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        if(!tr[idx][s[i] - 'a']){
            tr[idx][s[i] - 'a'] = ++top;
        }
        idx = tr[idx][s[i] - 'a'];
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
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        idx = tr[idx][s[i] - 'a'];
        for(int j = idx; j; j = fail[j]){
            if(e[j]){
                return 1;
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    int n;
    cin >> n;
    while(n--){
        cin >> pat;
        insert(pat);
    }
    build();
    int m;
    cin >> m;
    while(m--){
        cin >> sen;
        if(match(sen)){
            cout << "YES" << endl;
        }else{
            cout << "NO" << endl;
        }
    }
    return 0;
}
