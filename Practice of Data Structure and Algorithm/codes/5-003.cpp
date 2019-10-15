#include <iostream>
#include <queue>
using namespace std;

int trie[40010][2];
int fail[40010], e[40010];
int instack[40010];
int visited[40010];
int tail = 1;

void insert(char* s){
    int node = 0;
    for(int i = 0; s[i]; i += 1){
        if(!trie[node][s[i] - '0']){
            trie[node][s[i] - '0'] = tail;
            tail += 1;
        }
        node = trie[node][s[i] - '0'];
    }
    e[node] += 1;
}

void build(){
    queue<int> q;
    for(int i = 0; i < 2; i += 1){
        if(trie[0][i]){
            q.push(trie[0][i]);
        }
    }
    while(q.size()){
        int node = q.front();
        q.pop();
        for(int i = 0; i < 2; i += 1){
            if(trie[node][i]){
                fail[trie[node][i]] = trie[fail[node]][i];
                q.push(trie[node][i]);
            }else{
                trie[node][i] = trie[fail[node]][i];
            }
        }
    }
}

void query(int root){
    instack[root] = 1;
    for(int i = 0; i < 2; i += 1){
        if(instack[trie[root][i]]){
            cout << "TAK" << endl;
            exit(0);
        }else if(!visited[trie[root][i]]){
            int is_end = 0;
            for(int j = trie[root][i]; j; j = fail[j]){
                if(e[j]){
                    is_end = 1;
                    break;
                }
            }
            if(!is_end){
                visited[trie[root][i]] = 1;
                query(trie[root][i]);
            }
        }
    }
    instack[root] = 0;
}

char pat[30010];

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int num_pat;

    cin >> num_pat;
    for(int i = 0; i < num_pat; i += 1){
        cin >> pat;
        insert(pat);
    }
    build();
    query(0);
    cout << "NIE" << endl;
    return 0;
}
