## 线段树

区间修改 区间求值

建树 -> 1-idxed
```c++
void build_tree(int s, int e, int pos){
    // init data in node[pos]
    if(s >= e - 1){
        return ;
    }
    build_tree(s, mid, pos * 2 + 1);
    build_tree(mid, e, pos * 2 + 1);
    push_up();
    return ;
}
```

更新/求值
```c++
void add(int s, int e, int pos){
    if(s <= node[pos].l && e >= node[pos].r){
        // change every field of this node
        return ;
    }
    push_down(); // modify every field!
    if(s < m){
        add(left_child);
    }
    if(e > m){
        add(right_child);
    }
    // aggregate information
    push_up();
}
```

离散化
```c++
sort(tmp, tmp + tmp_idx);
int length = unique(tmp, tmp + tmp_idx); // consider 111 222 333, poster: 12, 11, 22
int s = lower_bound(tmp, tmp + length, s[i]);
```


例题
- 区间最大子段和
    - 节点维护sum，左端点开始的最大子段和ls，右端点的rs，和区间内最大字段和
    - pushup：考虑跨越中间点的情况，此外疯狂取max
- 四倍节点数
- 线段树解逆序数
    - 从左到右，每看到一个数i，寻找[i, n]当中有多少个数字已经填充了，之后填充i
- 离散化：[l, r] -> 离散化三个点[l, r, r + 1]
- 区间合并？看不懂
