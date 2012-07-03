/*
 
Description :
    This is the file which illustrate standard programming style of this term's practial design course.

Author :
    Nicholas Tian

Version :
    v 1.0.0 by Nicholas Tian
    
*/

#include <****.h> // 说明 1, 虽然 说明 9 不在程序代码中, 但是必读哟.


void cmpAdd(); // 说明 8. 函数的申明应该放在头文件中, 为举例方便我把它放在程序中.


struct Container // 说明 8. 自定义的数据结构的申明也应该在头文件中.
{
    int a, b;
}


void main()
{ // 说明 2

    a = Container.a;
    b = Container.b;

    a = 0; // 说明 4
    b = 100; // 说明 4

    cmpAdd(a, b);
 
} // 说明 2


void cmpAdd(int x, int y)
{

    int i;
    for(i=0; a<b; i++) // 说明5
    {
        if ((a+b) > 150) // 说明 5, 6
        {
            a++;
        }
        else
        {
            a++;
            a++;
        } // end of if 说明 7

    } // end of for 说明 7

} // end of cmpAdd


/*
    说明: 
        0. 一行的字符数最好少于 80 个, 太长的单行代码不便阅读.
        1. 不相关段落之间空 两行.
        2. 函数段开始空 一行.
        3. 申明多个变量时, 逗号之后 空格.
        4. 使用二元操作符(例如 =, <, > +, - 等等)时, 等号两边 空格.
            判断二元操作符简便方法: 两边都有数据(变量和常量).
        5. 
            for, while, if 等语句换行之后输入 {, 无论 {} 语句长度.
            对于较长的 for 语句和 if 语句, 为了紧凑可以适当的省略空格, 如 for(i=0; i<10; i++) 和 if ((a<=b) && (c<=d)).
            对于优先级不明了的操作符, 最好的处理方式是将表达式用 () 括起来.

        6. if 后面保留一个空格接 (表达式)
        7. 当代码特别长, 特别是有多重嵌套时, 应在代码结束处加以注释.
        8.
            函数名应使用第一个首字母小写的驼峰式写法.
            结构体应使用首字母大写.
        9. 
            定义变量时, 相关性大的同类型变量放在一行, 不大的则分行书写.
            定义常量, 变量, 结构体, 函数名时, 尽可能使变量名有意义, 且可以直接读出具体含义.
            函数名表意的同时尽可能不要过长, 过长的函数可能带来的负作用: 不方便阅读, 不方便理解, 不方便书写等.
            例如 ds1302 发送数据函数名可定义为 timerSend(), 接收数据函数名可定义为 timerRecv().(为简洁用 Recv 代替 Receive)

    注释:
        1. 所有的缩进都是 4 个空格.
            不用 TAB 键, 用空格键或者利用一些方便的编辑软件.
        2. 不必用英语书写注释.
*/
