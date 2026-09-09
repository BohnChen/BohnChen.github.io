---
title: "set和 map，多态以及虚函数的特点"
date: 2026-09-08T10:31:49+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---


## set 和 map

`set` 与 `map` 都是**有序关联式容器（ordered associative container）**，这一讲里反复提到的"有序、去重、log 查找"全都来自它们的底层结构。先把结论放最前面：

> **`set` 与 `map` 的底层都是"红黑树（Red-Black Tree）"**，一棵自平衡的二叉搜索树。`set` 的节点只存一个元素本身；`map` 的节点存一对 `pair<const Key, Value>`（`Key` 带 `const`，即"键不可修改"）。两者都**按 key 从小到大**排列，key 唯一。
>
> 注意：真正用哈希表的，是它们的"无序"亲戚 `unordered_set` / `unordered_map`。

下面分成两件事讲清楚：**底层到底是不是树、从哪看出来 / 测出来**，以及 **set / map 各自的声明与增删改查**。

### 底层：为什么是红黑树

红黑树 = 一种**自平衡的二叉搜索树**。普通二叉搜索树在数据有序插入时会退化成链表（查找退化成 O(n)）；红黑树额外给每个节点加了一个"颜色"（红/黑）标记，并通过**变色 + 旋转**两条操作，在任何插入、删除之后都强制整棵树满足几条"红黑性质"，从而保证**树高始终 ≈ O(log n)**，增删查自然就是 **O(log n)**。

红黑树的四条核心性质（理解即可，不要求背）：
1. 每个节点非红即黑；
2. 根节点是黑的；
3. 红节点的两个子节点都是黑的（**红不连红**）；
4. 从任一节点到其每个叶子的所有路径，包含**相同数目**的黑节点（"黑高相同"）。

有了"黑高相同 + 红不连红"，最长路径最多是最短路径的两倍，所以最坏情况树高也才 `2·log₂(n+1)` 级别——这正是"O(log n)"的数学来源。

#### 从哪里"看出来"：直接读标准库头文件

本机（macOS，Apple clang 的 libc++）里，`map` 和 `set` 的成员几乎就是一棵树。看头文件 `/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1/map` 与 `set`：

```c++
// ---- libc++ 的 <map> 内部 ----
typedef __tree<__value_type, __vc, allocator_type> __base;  // __value_type = pair<const int,int>
__base __tree_;                                              // map 唯一的成员就是一棵 __tree

// ---- libc++ 的 <set> 内部 ----
typedef __tree<value_type, value_compare, allocator_type> __base;
__base __tree_;                                              // set 同样是一棵 __tree
```

再看 `__tree` 头文件里的**节点**定义和**平衡**函数，红黑树特征一览无余：

```c++
class __tree_node_base : public __tree_end_node<...> {
  pointer __right_;
  __end_node_pointer __parent_;
  bool __is_black_;        // ★ 每个节点带一个"颜色"标记 —— 红黑树铁证
};

class __tree_node : public __tree_node_base<...> {
  ...
  __node_value_type __value_;   // 节点里存一个元素/键值对
};

// 插入后要做的"染色 + 旋转"重平衡：
_LIBCPP_HIDE_FROM_ABI void __tree_balance_after_insert(_NodePtr __root, _NodePtr __x);
```

观察点：
- 节点是**链式**的（`__left_/__right_/__parent_` 三个指针 + 数据），不是一段连续数组 → 一定是链表类结构；
- 节点里有一个 `bool __is_black_` 颜色字段 → 一定是红黑树，而不是别的平衡树（AVL 没有颜色字段，`multiset` 也复用同一棵 `__tree`）。

> Linux / g++（GNU libstdc++）下如出一辙：`bits/stl_map.h` 里是
> `typedef _Rb_tree<key_type, value_type, ...> _Rep_type;`，容器内部那棵树叫 `_Rb_tree`（Red-Black tree 的缩写），节点里同样有颜色字段与旋转逻辑。**libc++ 叫 `__tree`，libstdc++ 叫 `_Rb_tree`，本质同一棵红黑树。**

#### 从哪里"看出来"：写一段"问题代码"，让编译器自己招供

这是最直观的做法——故意让程序编译不过，编译器为了报错会**把 `map`/`set` 迭代器的真实底层类型原样打出来**。

手法：声明一个**只声明、不定义**的模板 `Debug<T>`，然后去实例化它，编译器就会报"Debug<……> 未定义"，尖括号里那个 `……` 就是你想看的类型。把 `set`、`map`、`unordered_map` 三种迭代器都塞进去：

```c++
// 问题代码：故意编译失败，让编译器告诉我们 set/map/unordered_map 底层各是什么
#include <map>
#include <set>
#include <unordered_map>

template <typename T>
struct Debug;                       // 只有声明，没有定义：一实例化就报错

int main() {
  std::set<int> s;
  std::map<int, int> m;
  std::unordered_map<int, int> um;

  Debug<decltype(s.begin())> d1;    // 看 set 迭代器的底层类型
  Debug<decltype(m.begin())> d2;    // 看 map 迭代器的底层类型
  Debug<decltype(um.begin())> d3;   // 看 unordered_map 迭代器的底层类型
  (void)d1; (void)d2; (void)d3;     // 防"未使用变量"警告
  return 0;
}
```

编译（`clang++ -std=c++17 question.cc`），报错如下（本机实测节选）：

```
question.cc:13:30: error: implicit instantiation of undefined template 'Debug<std::__tree_const_iterator<int, std::__tree_node<int, void *> *, long>>'
question.cc:14:30: error: implicit instantiation of undefined template 'Debug<std::__map_iterator<std::__tree_iterator<std::__value_type<int, int>, std::__tree_node<std::__value_type<int, int>, void *> *, long>>>'
question.cc:15:31: error: implicit instantiation of undefined template 'Debug<std::__hash_map_iterator<std::__hash_iterator<std::__hash_node<std::__hash_value_type<int, int>, void *> *>>>'
```

把长长的模板参数剥掉，直接看关键信息：

| 容器 | 编译器报出的迭代器类型 | 结论 |
|---|---|---|
| `set<int>` | `__tree_const_iterator<..., __tree_node<...>, long>` | 底下一棵 `__tree` |
| `map<int,int>` | `__map_iterator<__tree_iterator<__value_type<...>, __tree_node<...>, long>>` | 底下一棵 `__tree`，节点里装 `__value_type`（即 `pair<const int,int>`） |
| `unordered_map<int,int>` | `__hash_map_iterator<__hash_iterator<__hash_node<...>*>>` | 底下是**哈希表** `__hash_table` |

> 在 Linux / g++ 上编译同一份代码，报错会变成 `Debug<std::_Rb_tree_iterator<...>>` 与 `Debug<std::_Rb_tree_node<...>>` —— 只是名字换成 `_Rb_tree`，同样自报家门。

再写一段"问题代码"验证迭代器类别：红黑树节点在内存里不连续，迭代器是**双向迭代器**，不支持 `+ n` 跳转（哈希表只是前向迭代器，也不支持；只有 `vector`/`deque` 的随机访问迭代器才能 `+ n`）：

```c++
#include <set>

int main() {
  std::set<int> s;
  auto it = s.begin();
  auto x = it + 2;      // ❌ 编译错误：不能给 set 的迭代器加整数
  (void)x;
  return 0;
}
```

```
error: invalid operands to binary expression ('iterator' (aka '__tree_const_iterator<int, __tree_node<int, void *> *, long>') and 'int')
```

#### 从哪里"测出来"：让比较器"数数"，实测查找次数 ≈ 2·log₂N

红黑树查找沿"根 → 叶"的一条路径走，每次只和当前节点比一次，所以总比较次数 ≈ 树高。树高被限制在 `2·log₂(n+1)` 内。写一个**会数数的比较器**，让 `set` 每次用 `<` 比大小都计数，再测一次 `find` 用了多少次比较：

```c++
#include <iostream>
#include <set>
using std::cout;
using std::endl;

struct Cmp {                      // 会"数数"的比较器：每比较一次自增一次
  static long long cnt;
  bool operator()(int a, int b) const {
    ++cnt;
    return a < b;
  }
};
long long Cmp::cnt = 0;

int main() {
  const int N = 1 << 20;          // 1048576，一百万
  std::set<int, Cmp> s;

  // 故意"按升序"插入 —— 对不做旋转的普通二叉搜索树来说，这是最坏情况（会退化成链表）
  for (int i = 0; i < N; ++i) s.insert(i);

  Cmp::cnt = 0;
  s.find(N - 1);                  // 找最后一个元素
  cout << "set 在 size=" << N << " 中 find(" << N - 1
       << ") 用掉比较次数 = " << Cmp::cnt << endl;
  cout << "2*log2(N) = " << 2 * 20 << "（红黑树查找的比较次数理论上限）" << endl;
  return 0;
}
```

运行结果（本机实测）：

```
set 在 size=1048576 中 find(1048575) 用掉比较次数 = 39
2*log2(N) = 40（红黑树查找的比较次数理论上限）
```

一百万条数据里找一次只要 **39 次比较**（≈ 2·log₂N），并且**即使故意按升序喂数据也不会退化**——因为红黑树会自动旋转保持平衡。若换成顺序查找 `vector`，要找 1048576 次；这就是"自平衡树 O(log n)"的实证。

### set 用法

`set`：元素**唯一、按从小到大排序**，节点的 value 就是元素本身。找不到 / 插不进都有明确返回值，先记一个通用口诀：

> `set` 的 `insert` 返回 `pair<迭代器, bool>`，`bool` 表示"这次真的插进去了吗"；所有 `find` 系查找找不到都返回 `end()`。

#### set 的声明

```c++
#include <set>
using std::set;

set<int> sa;                 // ① 默认构造：空 set，默认按 < 升序

// initializer list constructor
set<int> ib{1, 2, 3, 4};

// iterator constructor
set<int> ic(ib.find(2), ib.end());

// copy constructor
set<int> id(ic);

// move constructor
set<int> ie(std::move(id));

// 自定义比较器：降序 set
set<int, std::greater<int>> sf;             // 注意比较器是 set 的第二个模板参数
```

> 观察：`ic(ib.find(2), ib.end())` 说明可以用**迭代器区间** `[first, last)` 初始化一个 set，元素会被"去重 + 排序"后再放进去。

#### set 增

```c++
set<int> s;

// insert：返回 pair<迭代器, bool>；second=true 表示插入成功
std::pair<set<int>::iterator, bool> ret = s.insert(3);
cout << "第一次 insert(3) second = " << ret.second << endl;   // 1

ret = s.insert(3);                    // 3 已经存在 → 插入失败
cout << "第二次 insert(3) second = " << ret.second << endl;   // 0，且 *ret.first == 3

s.emplace(5);                         // emplace：原地构造，比 insert 少一次拷贝
s.insert({1, 4, 2});                  // 批量插入：initializer_list

for (int x : s) cout << x << " ";     // 遍历结果：1 2 3 4 5（自动升序、自动去重）
```

要点：
- **重复插入同一值 → 插入失败**（返回 `second == false`），这就是 `set` 的"去重"；
- `emplace` 直接在节点里构造元素，适合插入需要"临时拼出来"的对象；
- 因为是红黑树，插入本身是 **O(log n)**。

#### set 删

```c++
set<int> s{1, 2, 3, 4, 5, 6};

size_t n = s.erase(3);            // ① 按"值"删除，返回删掉几个（0 或 1）
cout << "删掉了 " << n << " 个" << endl;

auto it = s.find(4);
s.erase(it);                      // ② 按"迭代器"删除

s.erase(s.begin(), s.end());      // ③ 按区间删除（整个删光）
s.clear();                        // ④ 清空所有元素
```

`erase` 返回删除个数这一点值得留意：对 `set` 它只会是 0 或 1（键唯一）；对比后面补充的 `multiset`，那里会返回 >1。

#### set 改

**set 没有真正的"改"。** 因为元素本身就是 key，改了会破坏排序，所以标准库把 set 的元素设成只读的——通过迭代器拿到的元素是 `const` 的：

```c++
set<int> s{1, 2, 3, 4, 5};
// *s.find(4) = 40;   // ❌ 编译错误：set 的元素是 const，不能原位修改
```

想"改"某个元素，标准做法是**先删旧的，再插新的**（两步都是 O(log n)）：

```c++
s.erase(4);      // 删掉旧值
s.insert(40);    // 插入新值，充当"改"
```

#### set 查

```c++
set<int> s{1, 2, 3, 4, 5, 6};

// ① find：找不返回 end()
auto it = s.find(3);
if (it != s.end())
  cout << "找到了 " << *it << endl;

// ② count：在 set 里只会是 0 或 1，等价于"在不在"
cout << s.count(9) << endl;        // 0 = 不在

// ③ 区间查找（有序容器专享，见下）
auto lb = s.lower_bound(3);        // 第一个 >= 3 的元素
auto ub = s.upper_bound(3);        // 第一个 >  3 的元素
cout << "lower_bound(3) = " << *lb << ", upper_bound(3) = " << *ub << endl;
// 输出：lower_bound(3) = 3, upper_bound(3) = 4

// ④ equal_range：一次拿到 [low, up) 区间两端
auto range = s.equal_range(3);     // range.first == lb, range.second == ub

// ⑤ 因为有序，还能反着遍历
for (auto rit = s.rbegin(); rit != s.rend(); ++rit)
  cout << *rit << " ";             // 6 5 4 3 2 1
```

### map 用法

`map`：存储**键值对** `pair<const Key, Value>`，按 key 排序、key 唯一。可以粗暴理解成："key 当作下标，value 当作元素"的树。map 与 set 最大的两个差异：
1. 节点里多存了一个 value，因此能通过 key **找到/修改一个关联值**；
2. 多了 `operator[]` / `at` 这种"用 key 直接取 value"的接口。

> 注意 map 的元素类型是 `pair<const Key, Value>`：**`first`（key）带 `const`，不能改**；只有 `second`（value）能改。

#### map 声明

```c++
#include <map>
#include <string>
using std::map;
using std::string;

// ① 默认构造：空 map
map<string, int> m1;

// ② initializer_list：花括号里每项是一个 pair（键:值）
map<string, int> m2{{"tom", 90}, {"jerry", 85}, {"mike", 95}};

// ③ 迭代器区间构造
map<string, int> m3(m2.begin(), m2.end());

// ④ 拷贝构造 / move 构造
map<string, int> m4(m2);
map<string, int> m5(std::move(m4));
```

#### map 增

```c++
map<string, int> m;

// ① insert(pair)：key 不存在才插入；返回 pair<迭代器,bool>
auto ret = m.insert({"tom", 90});                 // 花括号会隐式转成 pair<const string,int>
cout << "第一次插入 tom second = " << ret.second << endl;    // 1

ret = m.insert({"tom", 100});                     // key "tom" 已存在 → 插入失败
cout << "重复插入 tom second = " << ret.second << endl;      // 0
cout << "tom 的值仍是 " << ret.first->second << endl;        // 90（没被覆盖）

// ② emplace：原地构造
m.emplace("jerry", 85);

// ③ operator[]："键不存在就插入，存在就返回引用"，既能增也能改
m["mike"] = 95;                                   // 不存在 → 新增一个 mike:95
```

#### map 删

```c++
map<string, int> m{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}};

size_t n = m.erase("b");       // ① 按键删除，返回删了几个（0 或 1）
cout << "删掉了 " << n << " 个" << endl;

auto it = m.find("c");
m.erase(it);                   // ② 按迭代器删除

m.erase(m.begin(), m.end());   // ③ 区间删除
m.clear();                     // ④ 清空
```

#### map 改

value 可改，key 不能改：

```c++
map<string, int> m{{"tom", 90}, {"jerry", 85}};

// ① operator[] 赋值：key 已存在就是"改 value"
m["tom"] = 95;

// ② 通过迭代器改 value
auto it = m.find("jerry");
if (it != m.end()) it->second = 100;

// ③ at() 返回 value 的引用，改了也生效
m.at("tom") = 96;

// ❌ 想改 key？不行——key 是 const
// m["tom"] 可以给 value 赋值，但改键必须先删后插：
m.erase("tom");
m["Tom"] = 96;     // 相当于"把键 tom 改成 Tom"
```

#### map 查

```c++
map<string, int> m{{"tom", 90}, {"jerry", 85}, {"mike", 95}};

// ① find：返回迭代器，找不到返回 end()
auto it = m.find("tom");
if (it != m.end())
  cout << it->first << " : " << it->second << endl;   // tom : 90

// ② count：0 或 1（键唯一），用来判断"在不在"
if (m.count("mike")) cout << "mike 在" << endl;

// ③ at()：返回 value 的引用；key 不存在会抛 out_of_range 异常
try {
  cout << m.at("tom") << endl;
} catch (const std::out_of_range &) {
  cout << "没有这个键" << endl;
}

// ④ operator[]：⚠️ 查询千万别用这个！key 不存在会"顺手插入默认值"
cout << m["bob"] << endl;     // 打印 0，但 map 里凭空多了一个 bob:0！
cout << m.size() << endl;     // 变成 4 了，注意这个副作用
```

> **map 查询口诀**：`find` 最安全（不插入）；`at` 第二安全（查不到就抛异常，还能写）；`count` 只用来判断在不在；**`operator[]` 只在"确知键存在或想新增"时用**，否则会产生"查询副作用"。

### 补充

1. **家族对比**：`set`/`map` 红黑树 vs `unordered_set`/`unordered_map` 哈希表。选谁的关键看需求：

| 需求 | 用 |
|---|---|
| 需要按键有序遍历 / lower_bound 区间查找 | set / map |
| 只在乎"查得快"，不在乎顺序 | unordered_set / unordered_map |
| 键允许重复 | multiset / multimap（红黑树，`insert` 永不失败、`count` 可 >1，无 `operator[]`） |

2. **复杂度**：set/map 的增删查、`find/count/lower_bound` 全是 **O(log n)**；遍历是 O(n) 且天然按键升序（红黑树中序）。因为有序，`lower_bound/upper_bound/equal_range` 这三件套在 set/map 上才有意义，哈希版 unordered 没有这些成员。

3. **key 不可变**：map 的 value_type 是 `pair<const Key, Value>`，set 元素只读。要"改键"一律先删后插。自定义类型当 key 时，需要能比较（重载 `operator<` 或传比较器），并且比较行为要与内容一致，否则会破坏红黑树的不变量。

4. **迭代器与内存**：set/map 的迭代器是**双向迭代器**（只能 `++/--`），不是随机访问（`vector` 那种能 `+n`）。原因从底层一眼可知——红黑树节点靠指针相连、内存不连续。节点元素/键值对是**动态分配**的，每个节点额外带左右/父指针与颜色位，因此比存 `vector` 更费内存，换来的是"插入/删除不搬动已有元素"。

5. **代码复现**：上面所有"问题代码"与实测程序都在本仓库 `/TestCode/cppTest/cppDay12/` 下，可直接编译验证：
   - [`question.cc`](/TestCode/cppTest/cppDay12/question.cc)：故意编译失败，让编译器吐出 set/map/unordered_map 的底层类型；
   - [`testNoRandomAccess.cc`](/TestCode/cppTest/cppDay12/testNoRandomAccess.cc)：故意编译失败，证明 set 迭代器不能 `+n`；
   - [`testCountCmp.cc`](/TestCode/cppTest/cppDay12/testCountCmp.cc)：数比较器的次数，实测 ≈ 2·log₂N。


## 多态的概念
抽象是为了我们解决问题时候的建模思想；封装可以隐藏实现细节，让代码模块化；继承可以扩展已经存在的代码模块和类。封装和继承都是为了代码重用。而多态除了提高代码的复用性之外，还可以解决项目中紧耦合的问题，提高程序的扩展性。
那么，为什么需要降低程序的耦合度呢？因为软件工程是必须考虑代码的维护问题的，当一个项目的耦合度很高，那么后期的修改过程中，一个小的修改，就有更大的可能牵连很多别的地方，这会增加开发成本。

那么多态是怎样降低程序的耦合度的呢？我们看一个代码例子：

```c++
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
```

运行结果：

```
--- 方案一：不使用多态（紧耦合） ---
汪汪
喵喵
--- 方案二：使用多态（松耦合） ---
汪汪
喵喵
嘎嘎
```

逐句体会两种方案在"扩展"时的差别：

- **方案一（无多态）里，调用方 `letSpeakByKind` 依赖的是"具体类型"**。它必须知道世界上有 `OldDog`、`OldCat`……于是要靠 `Kind` 标记 + `switch` 手动分发。将来要加一只 `Duck`，你必须**修改这个已经写好的函数**：在枚举里加成员、在 `switch` 里加一个 `case`。改旧代码 = 牵一发而动全身，这就是**高耦合**——调用方被"绑死"在具体实现上。
- **方案二（多态）里，调用方 `letSpeakByBase` 依赖的是"抽象基类 `Animal`"**。它只认 `Animal*`，根本不需要知道实际传进来的是 Dog、Cat 还是 Duck。将来要加新动物，只需要**新增一个继承 `Animal` 的类并重写 `makeSound()`**，这段调用代码**一行都不用改**。这就是**低耦合**——新增功能时不需要改动已有代码，只做"增量添加"，即面向对象设计里的**开闭原则**：对扩展开放、对修改关闭。

多态之所以能解耦，本质是因为它把"变化的部分"（每种动物怎么叫）**上移到了派生类去各自实现**，而把"不变的部分"（我有一群动物，要一只只叫它们）**留在基类接口处统一处理**。这样一来，代码的维护点从"调用方"转移到了"新增的类"上，改动范围被限制在新增代码里，而不是遍布全网。


多态分为编译时多态和运行时多态。
- 编译时多态：
    * 也称为静态多态，我们之前学习过的函数重载、运算符重载就是采用的静态多态，C++编译器根据传递给函数的参数和函数名决定具体要使用哪一个函数，又称为先期联编（early binding）。
- 运行时多态
    * 在一些场合下，编译器无法在编译过程中完成联编，必须在程序运行时完成选择，因此编译器必须提供这么一套称为“动态联编”（dynamic binding）的机制，也叫晚期联编（late binding）。C++通过虚函数来实现动态联编。

#### 补充：运行时多态形成的"三条件"

上面例子里的 `letSpeakByBase(a)` 之所以能让 Dog/Cat/Duck 表现出不同行为，需要同时满足三个条件（缺一不可，可以拿它来检验一段代码到底"多不多态"）：

1. **要有继承关系**：一个基类（`Animal`）和一个或多个派生类（`Dog`/`Cat`/`Duck`）；
2. **基类把该成员函数声明为 `virtual`，派生类重写（override）它**：即"同一个函数名"在基类和派生类里有不同的实现；
3. **必须通过基类的指针或引用去调用**：代码里是 `Animal *a`。这三点齐了，运行时才会根据"指针实际指向的对象类型"去挑选该调用哪个版本——这就是动态联编。

反过来理解"为什么能解耦"：正因为第 3 步里代码只写死了 `Animal*`，调用方才不依赖具体子类，新增子类才不用改调用方。


## 虚函数定义
什么是虚函数呢？虚函数就是在基类中被声明为virtual，并在一个或多个派生类中被重新定义的成员函
数。其形式如下:
```c++
// 类内部
class 类名
{
    virtual 返回类型 函数名(参数表)
    {
        //...
    }
};

//类之外
virtual 返回类型 类名::函数名(参数表)
{
    //...
}
```
如果一个基类的成员函数定义为虚函数，那么它在所有派生类中也保持为虚函数，即使在派生类中省略了virtual关键字，也仍然是虚函数。派生类可以对虚函数根据需要进行重定义，重定义的格式有一定的要求：
- 与基类的虚函数有相同的参数个数；
- 与基类的虚函数有相同的参数类型；
- 与基类的虚函数有相同的返回类型。

#### 补充：关于虚函数，还要记几点

1. **函数重写（override） vs 名字隐藏（name hiding）**：派生类里写一个与基类虚函数"同名同参同返回"的函数，叫**重写**，多态就靠它；但若只**同名、参数或返回类型不同**，那它不叫重写，而是把基类的版本**隐藏**掉了——此时通过基类指针调用，走的仍是基类版本，多态不生效。上面例子里的 `Dog::makeSound()` 就是标准重写（基类是 `Animal::makeSound()`）。

2. **`virtual` 只出现在基类声明处**：重写时派生类不写 `virtual` 也仍是虚函数；但强烈建议写上 **`override`**（C++11 起），让编译器帮你检查"我到底有没有写错"（比如少写个 `const` 或参数类型不匹配，编译器会直接报错，而不是静默隐藏，埋下运行时走错版本的坑）。上面 `Dog`/`Cat`/`Duck` 都写了 `override`，就是这个用意。

3. **构造函数不能是虚函数，析构函数通常要是虚函数**：构造函数运行时对象还没"完全成型"，没有意义做动态联编，C++禁止把构造函数写成 virtual；反过来，`Animal` 的析构函数写成了 `virtual ~Animal()`，是因为将来要通过 `Animal*` 去 `delete` 一个 `Dog` 对象时，必须能调用到 `Dog` 的析构函数（回忆继承章节：析构顺序是派生类先析构）。若析构不是虚函数，通过基类指针删除派生类对象就是未定义行为——上面例子刻意把析构函数写成了虚的，就是为了演示这个"必修点"。

4. **纯虚函数与抽象类**：`virtual 返回类型 函数名(...) = 0;` 是**纯虚函数**，只声明不实现。包含纯虚函数的类叫**抽象类**（如例子中的 `Animal`），**不能创建对象**，只能当基类被继承——派生类必须实现（重写）所有纯虚函数后才能实例化。抽象类存在的意义就是定义"接口"，强迫所有子类提供统一的行为入口，这也正是它能把调用方和具体类型解耦的原因。

5. **虚函数需要在运行期"查表"调用**：含虚函数的类，其对象里会多一个隐藏指针 vptr，指向一张虚函数表 vtbl。通过基类指针调用虚函数时，程序要沿着 vptr → vtbl 找到真正要调的那个函数地址再跳转，比普通函数多一次间接寻址，这是运行时多态的性能代价（换来了"晚一点再决定调谁"的灵活性）。

> 本段"多态降低耦合"的例子代码在仓库 [`/TestCode/cppTest/cppDay12/testPolymorphism.cc`](/TestCode/cppTest/cppDay12/testPolymorphism.cc)，可直接编译复现。


