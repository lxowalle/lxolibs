## 关于m4处理流程的解释
m4 对输入流是以记号（Token）为单元进行读取的。
一般情况下，m4 会将读取的每个记号直接发送到输出流，但是当 m4 发现某个单词是已定义的宏名时，它会将这个宏展开。
在对宏进行展开的过程中，m4 可能会需要读入更多的文本以获取宏的参数。
宏展开的结果会被插入到输入流剩余部分的前端，也就是说，宏展开后所得到的文本会被 m4 重新读取，解析为记号，继续处理。

## 定义缓冲区为-1(或其他负数)可以不输出一些数据
divert(-1)
Now in -1,not output
divert(0)dnl
Now in 0,output

## 通过宏输出"Hello world!!!"
define(hello_world, Hello world!!!)dnl
Output: hello_world


## 使用带参数宏定义一个结构体(最多支持9个参数)
define(DEF_PAIR_OF,
`typedef struct pair_of_$1 {
    $1  first;
    $1  second;
}pair_of_$1')dnl
DEF_PAIR_OF(int)
DEF_PAIR_OF(double)
DEF_PAIR_OF(other_t)

## 宏的重定义
define(`HELLO', Hello 123)
HELLO
define(`HELLO', Hello abc)
HELLO

## 递归定义宏(m4 -dV xxx可以看到m4解析过程)
define(`definenum', `define(`num', 99)')
definenum
num

## 使用changecom 修改默认注释符(默认为#)
## 使用changequote 修改默认的引号定界符
## 使用dnl可以让注释不回显
dnl 不回显不回显不回显不回显不回显

## ifdef判断宏是否定义
`HELLO' is ifdef(`HELLO',,not)defined.

## ifelse判断分支
define(CHAR, b)
ifelse(CHAR,a,b,c)  # Compare CHAR with a,If TRUE,return b;else if is FALSE,return c

## eval进行数值运算
define(`n', 3)dnl
eval(n < 2)
eval(n == 2)
eval(n + 2)
eval(n * 2)
eval(n / 2)
`n' is ifelse(eval(n < 2),1, less than, eval(n==2), 1, , greater than)2

## 使用esyscmd访问shelless
esyscmd(`echo Hello world')

## 模拟迭代
divert(-1)
define(`FIB_ITER',
`ifelse(`$3', 0,
         $2,
     `FIB_ITER(eval($1 + $2), $1, eval($3 - 1))')')
define(`FIB', `FIB_ITER(1, 0, $1)')
divert(0)dnl
FIB(46)

## 模拟for循环
define(`for',
`ifelse($#,
        0,
        ``$0'',
        `ifelse(eval($2 <= $3),
                1,
               `pushdef(`$1',$2)$4`'popdef(`$1')$0(`$1', incr($2), $3, `$4')')')')
for(`i', 1, 5, `-')

## 使用local作为宏的前缀来定义局部宏
## 使用pushdef,popdef来使用局部宏
define(`USED',1)
USED
define(`proc',
  `pushdef(`USED',10)pushdef(`UNUSED',20)dnl
`'`USED' = USED, `UNUSED' = UNUSED`'dnl
`'popdef(`USED',`UNUSED')')
proc     # -> USED = 10, UNUSED = 20
USED     # -> 1

## 对内建宏添加m4_前缀，执行命令时添加`m4 -P`来处理
m4_define(HA,2)
HA
