---
title: "算法在做什么?"
date: 2026-08-05
draft: false
categories: ["Algorithm"]
tags: ["StudyRecord", "WeeklySummary"]
---


# 算法在做什么?
总而言之，算法只做一件事情，那就是可以**让我们更省资源的更快达成目的**。

同样一段斐波那契额数列的求解，同样的n为50的规模，一种求解斐波那契数列的算法可以耗时`110306 ms`，另一种可以耗时`0 ms`完成，这就是算法的魅力。

这个差别是巨大的。
```c++
// 以macbook pro M4 MAX芯片为例
// 虽然不精确，但是可以直观的感受算法带来的差异
/*******
    // fibonacci(int i)
    n = 40 : 耗时 385 ms
    n = 50 : 耗时 45251 ms

    // fibonacci_2 (int first, int second, int n)
    n = 40 : 耗时 0 ms
    n = 50 : 耗时 0 ms
********/

#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

// 算法一
int fibonacci(int i) {
	if(i <= 0) return 0;
	if(i == 1) return 1;
	return fibonacci(i - 1) + fibonacci(i - 2);
}

// 算法二
int fibonacci_2 (int first, int second, int n) {
	if (n <= 0) {
		return 0;
	}
	if (n < 3) {
		return 1;
	}
	else if (n == 3) {
		return first + second;
	}
	else {
		return fibonacci_2(second, first + second, n - 1);
	}

}

void time_consumption() {
	int n;
	while (cin >> n) {
		milliseconds start_time = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
		);

		fibonacci(n);


		milliseconds end_time = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
		);
		std::cout << milliseconds(end_time).count() - milliseconds(start_time).count() << " ms" <<  std::endl;
	}
}

int main(int argc, char *argv[]) {
	time_consumption();
	return 0;
}
```


那么为什么会造成如此巨大的差异呢？

`return fibonacci(i - 1) + fibonacci(i - 2)`的时间复杂度是 2<sup>n</sup> 次方，每一个递归都裂变为了两次递归，直到看到递归退出条件;

这样的调用结构可以画出一颗二叉树，二叉树的节点数量最多为 2<sup>k</sup> - 1 ，其中`k`为二叉树的深度，每个节点都表示一次递归，也就是递归复杂度为O(2<sup>n</sup>)；

但是优化后的算法时间复杂度是`O(n)`，递归深度就是常数级别N次而已，所以执行的很快。

