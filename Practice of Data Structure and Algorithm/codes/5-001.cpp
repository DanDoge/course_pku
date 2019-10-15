#include <iostream>
#include <queue>
using namespace std;

int trie[10010][26];
int fail[10010], e[10010];
int tail = 1;

void insert(char* s){
    int node = 0;
    for(int i = 0; s[i]; i += 1){
        if(!trie[node][s[i] - 'a']){
            trie[node][s[i] - 'a'] = tail;
            tail += 1;
        }
        node = trie[node][s[i] - 'a'];
    }
    e[node] += 1;
}

void build(){
    queue<int> q;
    for(int i = 0; i < 26; i += 1){
        if(trie[0][i]){
            q.push(trie[0][i]);
        }
    }
    while(q.size()){
        int node = q.front();
        q.pop();
        for(int i = 0; i < 26; i += 1){
            if(trie[node][i]){
                fail[trie[node][i]] = trie[fail[node]][i];
                q.push(trie[node][i]);
            }else{
                trie[node][i] = trie[fail[node]][i];
            }
        }
    }
}

int query(char* s){
    int node = 0, res = 0;
    for(int i = 0; s[i]; i += 1){
        node = trie[node][s[i] - 'a'];
        for(int j = node; j; j = fail[j]){
            if(e[j]){
                return 1;
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int num_pat, num_sen;

    cin >> num_pat;
    for(int i = 0; i < num_pat; i += 1){
        char pat[128];
        cin >> pat;
        insert(pat);
    }
    build();
    cin >> num_sen;
    for(int i = 0; i < num_sen; i += 1){
        char sen[1024];
        cin >> sen;
        if(query(sen)){
            cout << "YES" << endl;
        }else{
            cout << "NO" << endl;
        }
    }
    return 0;
}
