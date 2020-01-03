### 强连通分量

tarjan
- maintain dfn[u], low[u]
- dfn strictly increases, low strictly non-decreases
- three cases
    - not visited: tarjan it!
    - in stack: update low
    - not in stack: done, no need to do anything

```c++
void tarjan(int idx){
    low[idx] = dfn[idx] = ++depth;
    ins[idx] = 1;
    stack[top++] = idx;
    for(int i = 0; i < graph[idx].size(); i += 1){
        int j = graph[idx][i];
        if(!dfn[j]){
            tarjan(j);
            low[idx] = min(low[idx], low[j]);
        }else if(ins[j]){
            low[idx] = min(low[idx], dfn[j]);
        }
    }
    if(low[idx] == dfn[idx]){
        while(1){
            int tmp = stack[--top];
            ins[tmp] = 0;
            if(tmp == idx){
                break;
            }
        }
        num_group += 1;
    }
    return ;
}
```

spf
- similar to tarjan but
    - all root nodes with more than two children are spf nodes
    - other nodes should have children with low[children] <= low[parent]

```c++
void tarjan(int idx, int fa){
    dfn[idx] = low[idx] = ++depth;
    int num_child = 0;
    for(int i = 0; i < graph[idx].size(); i += 1){
        int j = graph[idx][i];
        if(!dfn[j]){
            tarjan(j, idx);
            num_child += 1;
            low[idx] = min(low[idx], low[j]);
            if(low[j] >= dfn[idx]){
                cut[idx] += 1;
            }
        }else if(dfn[j] < dfn[idx] && j != fa){
            low[idx] = min(low[idx], dfn[j]);
        }
    }
    if(fa < 0){ // root
        if(num_child >= 2){
            res.push_back(idx);
        }
    }else{
        if(cut[idx] >= 1){
            res.push_back(idx);
            cut[idx] += 1;
        }
    }
}
```
