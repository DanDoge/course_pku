### Tigger2Riscv

这一部分的报告包括Tigger到Riscv的程序的功能，拓展。

这一部分的代码能够将单个符合Tigger的BNF文法的程序转换成符合Riscv文法的程序。除了下面列出来的功能，其他的数据结构和实现方法和翻译Minic类似。

因为这部分十分直接，并且也有提供辅助材料，所以没有实现任何要求外的功能。
- 对于二目运算符的翻译，观察到很多运算符翻译后的语句共享同样形式，我实现成查表的形式。
- 对于内置函数，实验发现机侧平台的的函数名是没有前缀"f\_"的形式，所以将所有函数的前缀都去掉了。

```python
case OP_ADD:
case OP_SUB:
case OP_MUL:
case OP_DIV:
case OP_MOD:
case OP_GE:
case OP_LE:{
    printf("%s %s,%s,%s\n", op_table[op_type], $1, $3, $5);
    break;
}
```
