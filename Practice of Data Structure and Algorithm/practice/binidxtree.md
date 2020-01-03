### 树状数组

单点修改区间求值

```c++
int lowbit(int x){
    return x & -x;
}

int add(int pos, int val){
    for(; pos <= size; pos += lowbit(pos)){
        c[pos] += val;
    }

    return 0;
}

int sum(int pos){
    int sum = 0;
    for(; pos > 0; pos -= lowbit(pos)){
        sum += c[pos];
    }

    return sum;
}
```

区间加，区间求和
- 维护差分数组 -> sum a[i] = sum b[j] * (i + 1) - sum jb[j]
- 区间加只需要修改区间端点的差分数值
- 点查询只需要存b[j]就好
