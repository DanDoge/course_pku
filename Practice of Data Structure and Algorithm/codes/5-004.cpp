#include <iostream>
#include <queue>
#include <string.h>
#include <memory.h>
using namespace std;

const int infty = 10010;

int trie[1024][4];
int fail[1024], e[1024];
int dp[1024][1024];
int tail = 1;

int get_idx(char c){
    if(c == 'A'){
        return 0;
    }else if(c == 'T'){
        return 1;
    }else if(c == 'C'){
        return 2;
    }else if(c == 'G'){
        return 3;
    }
    return -1;
}

void insert(char* s){
    int node = 0;
    for(int i = 0; s[i]; i += 1){
        if(!trie[node][get_idx(s[i])]){
            trie[node][get_idx(s[i])] = tail;
            tail += 1;
        }
        node = trie[node][get_idx(s[i])];
    }
    e[node] += 1;
}

void build(){
    queue<int> q;
    for(int i = 0; i < 4; i += 1){
        if(trie[0][i]){
            q.push(trie[0][i]);
        }
    }
    while(q.size()){
        int node = q.front();
        q.pop();
        for(int i = 0; i < 4; i += 1){
            if(trie[node][i]){
                fail[trie[node][i]] = trie[fail[node]][i];
                q.push(trie[node][i]);
            }else{
                trie[node][i] = trie[fail[node]][i];
            }
        }
    }
}


void init_trie(){
	memset(trie, 0, sizeof(trie));
	memset(fail, 0, sizeof(fail));
	memset(e, 0, sizeof(e));
	memset(dp, infty, sizeof(dp));
	tail = 1;
}

int main(int argc, char const *argv[]) {
    ios::sync_with_stdio(false);
    int case_num = 0;
    int pat_num = 0;
    while(cin >> pat_num && pat_num){
    	init_trie();
        while(pat_num--){
            char pat[32];
            cin >> pat;
            insert(pat);
        }
        build();
        char dna[1024];
        cin >> dna;
        int len = strlen(dna);
        dp[0][0] = 0;
        for(int i = 1; i <= len; i += 1){
            for(int j = 0; j < tail; j += 1){
                if(dp[i - 1][j] != infty){
                    for(int k = 0; k < 4; k += 1){
                    	int is_end = 0;
                    	for(int l = trie[j][k]; l; l = fail[l]){
                    		if(e[l]){
                    			is_end = 1;
							}
						}
                        if(!is_end){
                            dp[i][trie[j][k]] = min(dp[i][trie[j][k]], dp[i - 1][j] + (get_idx(dna[i - 1]) != k));
                        }
                    }
                }
            }
        }
        int ans = infty;
        for(int i = 0; i < tail; i += 1){
            if(!e[i] && ans > dp[len][i]){
                ans = dp[len][i];
            }
        }
        cout << "Case " << ++case_num << ": ";
        ans == infty? cout << "-1" << endl : cout << ans << endl;
    }
    return 0;
}
