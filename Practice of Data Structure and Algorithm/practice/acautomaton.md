### AC自动机

Trie树
- 就是树

```c++
void insert(char* s){
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        if(!tr[idx][s[i] - 'a']){
            tr[idx][s[i] - 'a'] = ++tr_top; // starts from zero
        }
        idx = tr[idx][i];
    }
    return ;
}
```

失配指针
```c++
void build(){
    queue<int> q;
    while(q.size()){
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
```

匹配
```c++
int match(char* s){
    int res = 0;
    int idx = 0;
    for(int i = 0; s[i]; i += 1){
        idx = tr[idx][s[i] - 'a'];
        for(int j = idx; j && e[j] != -1; j = fail[j]){
            res += e[j];
            e[j] = -1;
        }
    }
    return res;
}
```
