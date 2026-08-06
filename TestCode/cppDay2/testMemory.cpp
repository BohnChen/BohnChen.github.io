#include <iostream>

using std::cout;
using std::endl;

int globalint;//全局变量，位于全局区,初始化为0
char *globalpointer_1;//全局变量，位于全局区,初始化为nullptr
const int globalConstInt = 100;

int main(int argc, char **argv)
{
    const int LocalConstInt = 0;//局部常量位于栈上
    int localInt;//局部变量，位于栈区,初始化为随机值
    char *localPointer_2;//localPointer_2本身也是位于栈区
    char localStr_1[] = "hello";//localStr_1位于栈上
    static int LocalStaticInt = 10;//静态变量位于静态区

    int *localPointerToHeap = new int(10);//localPointerToHeap本身位于栈上， localPointerToHeap指向堆区
    const char *stringToText = "helloworld";//stringToText本身位于栈上，stringToText指向变量位于文字常量区的

    printf("\n打印变量的地址\n");
    printf("&globalint = %p\n", &globalint);
    printf("&globalpointer_1 = %p\n", &globalpointer_1);
    printf("globalpointer_1 = %p\n", globalpointer_1);
    printf("&localInt = %p\n", &localInt);
    printf("&localPointer_2 = %p\n", &localPointer_2);
    printf("localPointer_2 = %p\n", localPointer_2);
    printf("&localStr_1 = %p\n", &localStr_1);
    printf("localStr_1 = %p\n", localStr_1);
    printf("&LocalStaticInt = %p\n", &LocalStaticInt);
    printf("&localPointerToHeap = %p\n", &localPointerToHeap);
    printf("localPointerToHeap = %p\n", localPointerToHeap);
    printf("&stringToText = %p\n", &stringToText);
    printf("stringToText= %p\n", stringToText);
    printf("\"helloworld\"= %p\n", &"helloworld");//文字常量区
    printf("&main = %p\n", &main);//程序代码区
    printf("main = %p\n", main);
    printf("&globalConstInt = %p\n", &globalConstInt);
    printf("&LocalConstInt = %p\n", &LocalConstInt);

    printf("\n打印变量的值\n");
    printf("globalint = %d\n", globalint);
    printf("localInt = %d\n", localInt );

    delete localPointerToHeap;
    localPointerToHeap = nullptr;

    return 0;
}


