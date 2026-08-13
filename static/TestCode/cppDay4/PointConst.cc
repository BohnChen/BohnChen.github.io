#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:
    Point(int ix = 0, int iy = 0)
    : _ix(ix)
    , _iy(iy)
    {
        cout << "Point(int = 0, int = 0)" << endl;
        /* _ix = ix;//error,赋值 */
        /* _iy = iy; */
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    ~Point()
    {
        cout << "~Point()" << endl;
    }
private:
    const int _ix;//常量数据成员,必须在初始化列表中进行
    const int _iy;
};
int main(int argc, char **argv)
{
    cout << "Hello world" << endl;
    return 0;
}

