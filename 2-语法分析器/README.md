

# 2-语法分析器

### 2.1 实验要求

​	手动设计实现语法分析器

> 使用**LL(1)分析法**和**LR分析法**设计实现算术表达式的语法分析器
>
> - 算数表达式至少支持加减乘除以及括号操作，即（+，-，*，/，（））。
>
> 1. 实验报告，报告内容必须包含：
>
> - 算术表达式所依据的文法；
>
> - LL(1)和LR分析法所使用的分析表，以及简要分析；
>
> - 程序执行流程；
>
> - 程序运行结果展示。
>
> 2. 语法分析源程序：source.c（源程序包）
> 3. 可执行文件
> 4. 程序测试文件：test.txt（实验输入，将测试案例写入程序的可没有此项） 



### 2.2 实验环境

​	此词法分析器使用C++编写，针对C语言源程序进行词法分析，请保证编译器支持C++11以上标准。	

​	在windows上使用g++编译：

```
g++ -g -Wall -std=c++11 -o syntactic syntactic.cpp
```

​	或者使用clang编译：

````
clang -g -Wall -o syntactic.exe syntactic.cpp
````

​	运行程序：

```
syntactic.exe
```

​	在Linux上的编译并没有什么太大区别，只要编译器支持C++11以上即可，运行时应该使用`./`命令。另外，附上我的编译器版本：

```
gcc version 8.1.0
clang version 13.0.1
```



### 2.3 实验说明

​	实现的功能有：

- 表达式支持`=`, `+`, `-`, `*`, `/`, `%`以及`()`
- 分别使用LL(1)分析法和LR(1)分析法，并展示了每一步的分析流程
- 封装了上次实验使用的词法分析器，对词法分析器的输出直接进行语法分析

​	使用的算术表达式文法如下：

> (1) S -> V=E | E
>
> (2) V -> p
>
> (3) E -> Ew0T | T
>
> (4) T -> Tw1F | F
>
> (5) F -> p | (E)
>
> 其中p为数字或常数，w0代表`+`或`-`，w1代表`*`，`/`或`%`



### 2.4 实验结果

​	对于`demo.c`，`LL(1)`输出为：

```
*************************** Output of LL1 synatactic analyzer:***************************
For expression: a=1
     Analysis stack     Current symbol      Remaining seq                     Operation
                 #S                                  p=p#                         Start
                 #S                  p                =p#              select: S -> V=E
               #E=V                  p                =p#                select: V -> p
               #E=p                  p                =p#                    match: p=p
                #E=                  =                 p#                    match: ===
                 #E                  p                  #              select: E -> TE'
               #E'T                  p                  #              select: T -> FT'
             #E'T'F                  p                  #                select: F -> p
             #E'T'p                  p                  #                    match: p=p
              #E'T'                  #                                  select: T' -> e
                #E'                  #                                  select: E' -> e
                  #                  #                                       match: #=#
Everything fine.

For expression: a=a*b+a
     Analysis stack     Current symbol      Remaining seq                     Operation
                 #S                            p=pw1pw0p#                         Start
                 #S                  p          =pw1pw0p#              select: S -> V=E
               #E=V                  p          =pw1pw0p#                select: V -> p
               #E=p                  p          =pw1pw0p#                    match: p=p
                #E=                  =           pw1pw0p#                    match: ===
                 #E                  p            w1pw0p#              select: E -> TE'
               #E'T                  p            w1pw0p#              select: T -> FT'
             #E'T'F                  p            w1pw0p#                select: F -> p
             #E'T'p                  p            w1pw0p#                    match: p=p
              #E'T'                 w1              pw0p#           select: T' -> w1FT'
           #E'T'Fw1                 w1              pw0p#                  match: w1=w1
             #E'T'F                  p               w0p#                select: F -> p
             #E'T'p                  p               w0p#                    match: p=p
              #E'T'                 w0                 p#               select: T' -> e
                #E'                 w0                 p#           select: E' -> w0TE'
             #E'Tw0                 w0                 p#                  match: w0=w0
               #E'T                  p                  #              select: T -> FT'
             #E'T'F                  p                  #                select: F -> p
             #E'T'p                  p                  #                    match: p=p
              #E'T'                  #                                  select: T' -> e
                #E'                  #                                  select: E' -> e
                  #                  #                                       match: #=#
Everything fine.

For expression: a=a/(b-a)
     Analysis stack     Current symbol      Remaining seq                     Operation
                 #S                          p=pw1(pw0p)#                         Start
                 #S                  p        =pw1(pw0p)#              select: S -> V=E
               #E=V                  p        =pw1(pw0p)#                select: V -> p
               #E=p                  p        =pw1(pw0p)#                    match: p=p
                #E=                  =         pw1(pw0p)#                    match: ===
                 #E                  p          w1(pw0p)#              select: E -> TE'
               #E'T                  p          w1(pw0p)#              select: T -> FT'
             #E'T'F                  p          w1(pw0p)#                select: F -> p
             #E'T'p                  p          w1(pw0p)#                    match: p=p
              #E'T'                 w1            (pw0p)#           select: T' -> w1FT'
           #E'T'Fw1                 w1            (pw0p)#                  match: w1=w1
             #E'T'F                  (             pw0p)#              select: F -> (E)
           #E'T')E(                  (             pw0p)#                    match: (=(
            #E'T')E                  p              w0p)#              select: E -> TE'
          #E'T')E'T                  p              w0p)#              select: T -> FT'
        #E'T')E'T'F                  p              w0p)#                select: F -> p
        #E'T')E'T'p                  p              w0p)#                    match: p=p
         #E'T')E'T'                 w0                p)#               select: T' -> e
           #E'T')E'                 w0                p)#           select: E' -> w0TE'
        #E'T')E'Tw0                 w0                p)#                  match: w0=w0
          #E'T')E'T                  p                 )#              select: T -> FT'
        #E'T')E'T'F                  p                 )#                select: F -> p
        #E'T')E'T'p                  p                 )#                    match: p=p
         #E'T')E'T'                  )                  #               select: T' -> e
           #E'T')E'                  )                  #               select: E' -> e
             #E'T')                  )                  #                    match: )=)
              #E'T'                  #                                  select: T' -> e
                #E'                  #                                  select: E' -> e
                  #                  #                                       match: #=#
Everything fine.

***************************   End of LL1 synatactic analyzer: ***************************
```

​	`LR(1)`输出结果为：

```
*************************** Output of LR1 synatactic analyzer:***************************
For expression: a=1
                     Analysis stack  Current symbol      Remaining seq          Operation
                                #0                                p=p#              Start
                                #0                p                =p#            next(p)
                                #0                p                =p#          push: p16
                            #0 p16                =                 p#       reduce: r(9)
                             #0 V2                =                 p#           push: =3
                          #0 V2 =3                p                  #          push: p12
                      #0 V2 =3 p12                #                          reduce: r(7)
                      #0 V2 =3 F11                #                          reduce: r(6)
                       #0 V2 =3 T8                #                          reduce: r(4)
                       #0 V2 =3 E4                #                          reduce: r(1)
                             #0 S1                #                                    ok
Everything fine.

For expression: a=a*b+a
                     Analysis stack  Current symbol      Remaining seq          Operation
                                #0                          p=pw1pw0p#              Start
                                #0                p          =pw1pw0p#            next(p)
                                #0                p          =pw1pw0p#          push: p16
                            #0 p16                =           pw1pw0p#       reduce: r(9)
                             #0 V2                =           pw1pw0p#           push: =3
                          #0 V2 =3                p            w1pw0p#          push: p12
                      #0 V2 =3 p12               w1              pw0p#       reduce: r(7)
                      #0 V2 =3 F11               w1              pw0p#       reduce: r(6)
                       #0 V2 =3 T8               w1              pw0p#          push: w19
                   #0 V2 =3 T8 w19                p               w0p#          push: p12
               #0 V2 =3 T8 w19 p12               w0                 p#       reduce: r(7)
               #0 V2 =3 T8 w19 F10               w0                 p#       reduce: r(5)
                       #0 V2 =3 T8               w0                 p#       reduce: r(4)
                       #0 V2 =3 E4               w0                 p#          push: w06
                   #0 V2 =3 E4 w06                p                  #          push: p12
               #0 V2 =3 E4 w06 p12                #                          reduce: r(7)
               #0 V2 =3 E4 w06 F11                #                          reduce: r(6)
                #0 V2 =3 E4 w06 T7                #                          reduce: r(3)
                       #0 V2 =3 E4                #                          reduce: r(1)
                             #0 S1                #                                    ok
Everything fine.

For expression: a=a/(b-a)
                     Analysis stack  Current symbol      Remaining seq          Operation
                                #0                        p=pw1(pw0p)#              Start
                                #0                p        =pw1(pw0p)#            next(p)
                                #0                p        =pw1(pw0p)#          push: p16
                            #0 p16                =         pw1(pw0p)#       reduce: r(9)
                             #0 V2                =         pw1(pw0p)#           push: =3
                          #0 V2 =3                p          w1(pw0p)#          push: p12
                      #0 V2 =3 p12               w1            (pw0p)#       reduce: r(7)
                      #0 V2 =3 F11               w1            (pw0p)#       reduce: r(6)
                       #0 V2 =3 T8               w1            (pw0p)#          push: w19
                   #0 V2 =3 T8 w19                (             pw0p)#          push: (13
               #0 V2 =3 T8 w19 (13                p              w0p)#          push: p12
           #0 V2 =3 T8 w19 (13 p12               w0                p)#       reduce: r(7)
           #0 V2 =3 T8 w19 (13 F11               w0                p)#       reduce: r(6)
            #0 V2 =3 T8 w19 (13 T8               w0                p)#       reduce: r(4)
            #0 V2 =3 T8 w19 (13 E5               w0                p)#          push: w06
        #0 V2 =3 T8 w19 (13 E5 w06                p                 )#          push: p12
    #0 V2 =3 T8 w19 (13 E5 w06 p12                )                  #       reduce: r(7)
    #0 V2 =3 T8 w19 (13 E5 w06 F11                )                  #       reduce: r(6)
     #0 V2 =3 T8 w19 (13 E5 w06 T7                )                  #       reduce: r(3)
            #0 V2 =3 T8 w19 (13 E5                )                  #          push: )15
        #0 V2 =3 T8 w19 (13 E5 )15                #                          reduce: r(8)
               #0 V2 =3 T8 w19 F10                #                          reduce: r(5)
                       #0 V2 =3 T8                #                          reduce: r(4)
                       #0 V2 =3 E4                #                          reduce: r(1)
                             #0 S1                #                                    ok
Everything fine.

***************************   End of LR1 synatactic analyzer: ***************************
```

