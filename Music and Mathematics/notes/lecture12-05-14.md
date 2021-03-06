### 从一个初始音列出发能得到多少音列?

发现有的音列倒影+逆行和移调是一样的, 所以干掉了一半

定理A

```text
给定音列 P_0 = 0, a_1, ..., a_11,

I_k = R_0 的充要条件是(上述定理中的k必为奇数)

0 + a_11 = a_1 + a_10 = ... = k (mod 12)
```

计数, 从两边往中间推

定理B

```text
P_0 = R_k, 则有a_{11 - i} = a_{i} + 6
```

计数, 数前一半就好了

定义音列之间的等价关系

$X \sim Y \iff X 出现在 Y 为初始音列的音列矩阵里面$

等价类一共有9985920个等价类

276480个音列满足定理A, 46080个音列满足定理B, 他们只有24个音列

12!减一减, 除一除, 算就好了

"轨道"

发现满足定理A, B的音列很少

"要么他是天才, 要么他是数学家"

# 和弦与音网

特里斯坦和弦别人也用过, ich mochte hingehn

和弦之间的关系不应该是线性关系, 而可以是复杂的网络关系

欧拉看到和弦应该是音网, 黎曼(纯律, 不是那个黎曼)拓展了这一个理论, 之后在十二平均律上拓展出了新黎曼理论

新黎曼理论建立在音类上, 所以转位没什么意义,

```text
n元和弦就是n元子集合(pitch class set)
```

在时钟上画, 就是n边形, 升半音就旋转了, 至多转一圈就得到所有的和弦了, 但是减七和弦(3 + 3 + 3), 转两次就够了

定义距离

```text
(a, b) = 劣弧中包含的点数减1
```

距离向量: 把和弦的所有顶点连起来, 得到n(n - 1) / 2个数, 距离向量定义为(d_i | 距离等于i的顶点对数目)

如果两个音类距离为6, 移调6的话全重合了, 如果不是6的话, 怎么移, 最多重合一个

```text
移调k之后, 重合的音类等于d_k, 但是k = 6, 就是2d_6
```

### 轨道与稳定化子

对称群, 置换群

轨道的长度等于G / G_α

音列为什么有的24个有的48个? 稳定化子不同, 稳定化子阶1, 只有单位元 ==> 48, 稳定化子阶2 ==> 24
