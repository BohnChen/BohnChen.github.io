#include <iostream>

class Cube {
	double _x, _y, _z;
public:
	Cube(double x, double y, double z)
	:_x(x)
	,_y(y)
	,_z(z) {

	}
	~Cube() {

	}
	
	void getVolume() {
		std::cout << "The Volume of it is " << _x * _y * _z << std::endl;
	}

	void getArea() {
		std::cout << "The area of it is " << 2*(_x*_y) + 2*(_x*_z) + 2*(_y*_z) << std::endl;
	}

};

int main(int argc, char *argv[]) {
	Cube cube1(3,4,5);
	cube1.getArea();
	cube1.getVolume();
	return 0;
}
