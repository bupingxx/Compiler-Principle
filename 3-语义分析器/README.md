# 3-语义分析器

### 3.1 实验要求

​	表达式语义分析器的设计与实现，使用**递归下降翻译法**或**LL(1)翻译法**实现高级编程语言的语义分析，将其翻译为**四元式格式**的中间语言，至少支持**算术表达式**的语义分析。

- 算数表达式至少支持加减乘除以及括号操作，即（+，-，*，/，（））。

​	提交内容：

> 1. 实验报告，报告内容必须包括：
>
> - 翻译文法；
>
> - 若采用递归下降翻译法，须给出文法（至少实现算术表达式的文法）的子程序流程图，并在其上标注返回地址；
>
> - 给出一个算术表达式实例的分析表（表项内容参考实验三PPT P17）；
>
> - 运行结果展示；
>
> - 以及其他必要内容
>
> 2. 语义分析源程序：source.c（源程序包）
>
> 3. 可执行文件
>
> 4. 程序测试文件：test.txt（实验输入，将测试案例写入程序的可没有此项）



### 3.2 实验环境

​	此词法分析器使用C++编写，针对C语言源程序进行词法分析，请保证编译器支持C++11以上标准。	

​	在windows上使用g++编译：

```
g++ -g -Wall -std=c++11 -o clang semantic.cpp
```

​	或者使用clang编译：

````
clang -g -Wall -o semantic.exe semantic.cpp
````

​	运行程序：

```
clang.exe
```

​	在Linux上的编译并没有什么太大区别，只要编译器支持C++11以上即可，运行时应该使用`./`命令。另外，附上我的编译器版本：

```
gcc version 8.1.0
clang version 13.0.1
```



### 3.3 实验说明

​	实现的功能有：

- 表达式支持`=`, `+`, `-`, `*`, `/`, `%`以及`()`
- 分别使用LL(1)翻译法，并展示了每一步的分析流程
- 封装了之前实验使用的词法分析器和语法分析器，对词法分析器的输出直接进行语义分析



### 3.4 实验结果

​	对于`demo.c`：

```c++
  int main (void){   
    int a = 1, b = 0b11;
    // test...
    a = a * b + a;
    a = a / (b - a);
    a = (a + b) % (a + a * b);

    /* everything good */
    return 0;
}
```

​	`	LL(1)`翻译法输出为：

![image-20220515023224870](https://github.com/bupingxx/Compiler-Principle/blob/main/3-%E8%AF%AD%E4%B9%89%E5%88%86%E6%9E%90%E5%99%A8/src/image-1.png)

![image-20220515023246248](https://github.com/bupingxx/Compiler-Principle/blob/main/3-%E8%AF%AD%E4%B9%89%E5%88%86%E6%9E%90%E5%99%A8/src/image-2.png)

![image-20220515023910405](https://github.com/bupingxx/Compiler-Principle/blob/main/3-%E8%AF%AD%E4%B9%89%E5%88%86%E6%9E%90%E5%99%A8/src/image-1.png)

### 
