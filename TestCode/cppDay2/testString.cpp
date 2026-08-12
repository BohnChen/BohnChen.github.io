#include <cstdio>
#include <cstring>
#include <cstdlib>

void CStringUse() {
	// 堆空间
	char *pStr = (char *)malloc(11);
	strcpy(pStr, "hello");
	printf("%s\n", pStr);
	strcat(pStr, "world");
	pStr[0] = 'H';
	printf("%s\n", pStr);
	// 手动释放
	free(pStr);
	// 置空防止再次访问
	pStr = NULL;

	// 栈数组
	char str[10];
	strcpy(str, "nice");
	printf("%s\n", str);

}

int main(int argc, char *argv[]) {
	CStringUse();
	return 0;
}
