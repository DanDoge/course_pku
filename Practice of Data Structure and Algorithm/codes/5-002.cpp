#include <iostream>
#include <queue>
#include <memory.h>
#include <string.h>
using namespace std;

int trie[500100][26];
int fail[500100], e[500100];
int tail = 1;

void insert(char* s){
    int node = 0;
    for(int i = 0; s[i]; i += 1){
    	int ch = s[i] - 'A';
        if(!trie[node][ch]){
            trie[node][ch] = tail;
            tail += 1;
        }
        node = trie[node][ch];
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
        node = trie[node][s[i] - 'A'];
        for(int j = node; j && e[j] != -1; j = fail[j]){
        	res += e[j];
          	e[j] = -1;
        }
    }
    node = 0;
    int len = strlen(s);
    for(int i = len - 1; i >= 0; i -= 1){
        node = trie[node][s[i] - 'A'];
        for(int j = node; j && e[j] != -1; j = fail[j]){
            res += e[j];
            e[j] -= 1;
        }
    }
    return res;
}

void init_var(){
	memset(trie, 0, sizeof(trie));
	memset(fail, 0, sizeof(fail));
	memset(e, 0, sizeof(e));
	tail = 1;
}

char comp[5100100];
char sen[5100100];

void translate(){
	int num = 0;
	int idx = 0;
	for(int i = 0; comp[i]; i += 1){
		if(comp[i] >= 'A' && comp[i] <= 'Z'){
			sen[idx++] = comp[i];
		}
		if(comp[i] >= '0' && comp[i] <= '9'){
			num = num * 10 + comp[i] - '0';
		}
		if(comp[i] == ']'){
			while(--num){
				sen[idx++] = comp[i - 1];
			}
		}
	}
	sen[idx] = 0;
	return ;
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int num_cases;
    cin >> num_cases;
    while(num_cases--){
    	init_var();
        int num_pat;

        cin >> num_pat;
        for(int i = 0; i < num_pat; i += 1){
            char pat[1024];
            cin >> pat;
            insert(pat);
        }
        build();
        cin >> comp;
        translate();
        //cout << sen << endl;
        cout << query(sen) << endl;
    }
    return 0;
}
