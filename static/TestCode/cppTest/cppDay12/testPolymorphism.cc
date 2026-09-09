#include <iostream>
#include <vector>
using std::cout;
using std::endl;

/* ============ 方案一：不使用多态（紧耦合） ============ */
// 每种动物各写一套接口完全不同的"发声"函数
class OldDog { public: void bark() const { cout << "汪汪" << endl; } };
class OldCat { public: void meow() const { cout << "喵喵" << endl; } };

// 想"统一叫一遍"？只能搞一个类型标记 + switch 来分发：
enum class Kind { OldDog, OldCat };
void letSpeakByKind(const void *obj, Kind k) {
  switch (k) {
    case Kind::OldDog: static_cast<const OldDog *>(obj)->bark(); break;
    case Kind::OldCat: static_cast<const OldCat *>(obj)->meow(); break;
    // ← 想加 Duck/Bird？这个函数必须改：加 case、改枚举，旧代码被牵连
  }
}

/* ============ 方案二：使用多态（松耦合） ============ */
class Animal {                         // 抽象基类 = "统一的接口"
public:
  virtual void makeSound() const = 0;  // 纯虚函数：具体怎么发声由子类实现
  virtual ~Animal() {}
};
class Dog : public Animal {
public:
  void makeSound() const override { cout << "汪汪" << endl; }
};
class Cat : public Animal {
public:
  void makeSound() const override { cout << "喵喵" << endl; }
};
class Duck : public Animal {
public:
  void makeSound() const override { cout << "嘎嘎" << endl; }
};

void letSpeakByBase(Animal *a) { a->makeSound(); }  // 只认抽象基类，不认识具体子类

int main() {
  cout << "--- 方案一：不使用多态（紧耦合） ---" << endl;
  OldDog d1; OldCat c1;
  letSpeakByKind(&d1, Kind::OldDog);
  letSpeakByKind(&c1, Kind::OldCat);

  cout << "--- 方案二：使用多态（松耦合） ---" << endl;
  Dog d2; Cat c2; Duck u2;
  std::vector<Animal *> zoo{&d2, &c2, &u2};   // 不同动物能放进同一个容器
  for (Animal *a : zoo) letSpeakByBase(a);    // 同一句调用，各自表现不同
  return 0;
}
