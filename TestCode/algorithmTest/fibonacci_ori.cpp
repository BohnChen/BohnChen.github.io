#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;


long long fibonacci(long long i) {
	if(i <= 0) return 0;
	if(i == 1) return 1;
	return fibonacci(i - 1) + fibonacci(i - 2);
}

long long fibonacci_2 (long long first, long long second, long long n) {
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

		cout << fibonacci_2(0,1,n) <<  endl;
		// cout << fibonacci(n) <<  endl;


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

