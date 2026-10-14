# BombLab 实验报告

## 方法

使用 `objdump -d -Mintel bomb` 反汇编各个 `phase_*` 及其辅助函数，并用 `gdb` 对关键函数做了少量调用验证。解法只修改了 `defuser.c`；所有运算均使用输入的 64 位 `challenge`，没有写死某个 seed 的答案。

## 各关分析

1. **Phase 1**：`p1_compute` 依次取 challenge 的高 32 位、右移 16 位值和低 32 位，调用三个小函数。关键关系为 `b=(hi+8*(challenge>>16))^0x554f`，最终答案为 `((3*b)^(low+0x4ab8)) ^ ((hi+0x83dc)^low)`。
2. **Phase 2**：第一项是 `3*((challenge>>16)&15)`；之后五项按 `v[i]=(((challenge>>(i+21))&63) ^ 2*v[i-1]) + 15*i` 递推。
3. **Phase 3**：低三位选择跳转表，表序为 case 4、7、6、1、2、5、3、0；第二个参数为 `high32 ^ (challenge>>3)`，各 case 的线性/异或表达式直接按反汇编实现。
4. **Phase 4**：目标为 `challenge&127`，递归二分区间 `[0,127]`。命中返回 `(mid+0x26)^key`；左、右递归返回后分别乘 3、异或 `mid+key`，再异或 `0xe` 或 `6`。
5. **Phase 5**：根据低三位选择静态链表头，初始折叠为 `(((challenge>>6)^high32)^0xec77)*3 + value`，再异或节点 tag；随后按 `(challenge>>3)&3` 继续遍历。输出折叠值和最后 tag。
6. **Phase 6**：低三位选择第一张函数表，bits 3–5 选择第二张表；六个操作分别是旋转、移位、加法和异或组合，最后加 `0xb61a`。

## 验证

运行 `make` 和 `python3 public_test.py`，六关公开测试通过 59/60（Phase 5 的 seed 65535 仍需进一步核对链表头/节点映射）。另外用公开列表外的 seed `314159`、`20261014` 和 `1844674407370955161` 运行 `./defuser --seed ... | ./bomb --seed ...`，均显示 `Congratulations! You've defused the bomb.`

## 资料与工具

参考课程 README、x86-64 System V ABI 约定；工具为 GCC、`objdump`、`gdb` 和项目自带公开测试脚本。反汇编结果由本人结合 C 代码推导，未使用自动解题器。
