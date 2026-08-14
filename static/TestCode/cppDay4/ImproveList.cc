// ============================================================================
// ImproveList.cc —— 对 testList.cc 的双向链表进行优化后的版本
//
// 相比原版，做了以下优化（按严重程度排序）：
//
// 【优化 1｜正确性】构造函数不再"new 完立刻丢弃"
//   原版：List::List() : _head(new Node()), _tail(new Node()), _size(0) {
//             _head = nullptr;   // 刚 new 出来的两个节点立即被覆盖
//             _tail = nullptr;   // 造成 2 个节点的内存泄漏
//         }
//   改为：直接用 nullptr 初始化，不产生任何无意义的 new。
//
// 【优化 2｜正确性】析构函数真正释放整条链表
//   原版：~List() { delete _head; delete _tail; }
//   问题：(a) 只释放了头尾两个节点，中间所有节点全部泄漏；
//         (b) 当 _size == 1 时 _head == _tail，同一节点被 delete 两次，
//             触发"二次释放"未定义行为（double free）。
//   改为：从头到尾逐个 delete，并保存 next 再前进。
//
// 【优化 3｜正确性】pop_front / pop_back 合并重复分支
//   原版：分 "_size == 0 / ==1 / >1" 三个分支，代码重复且依赖 _size 判断。
//   改为：只判断"空"与"非空"，删除后统一根据新头/新尾是否为 nullptr
//         决定是否同步置空 _head/_tail，天然覆盖 size==1 的情况。
//
// 【优化 4｜正确性】修正 pop_back 的报错文案
//   原版：pop_back 里打印 "can not pop_front."（复制粘贴笔误），改为 pop_back。
//
// 【优化 5｜健壮性】erase 去掉冗余的 find(data) 预判断
//   原版：if (find(data)) { ... while 遍历 ... }
//   问题：find 本身就要遍历整条链表，随后 while 又遍历一遍，纯属重复劳动。
//   改为：直接 while 遍历，找不到时自然什么都不删，行为一致但少一趟遍历。
//
// 【优化 6｜封装】给 Node 增加构造函数
//   原版：每次 push 都要写 4 行手动赋值（data/pre/next）。
//   改为：Node(int d, Node *p, Node *n) 一步初始化，调用点更简洁、不易漏。
//
// 【优化 7｜const 正确性】find 改为 const 成员函数
//   find 不修改对象状态，标记为 const 后可在 const 对象/const 引用上调用。
//
// 【优化 8｜风格】统一 using std::cout / std::endl
//   原版：display / main 里混用 cout 与 std::cout、endl 与 std::endl。
//
// 说明：以上优化不改变任何对外行为，main 的运行结果与原版完全一致。
// ============================================================================

#include <iostream>

using std::cout;
using std::endl;

struct Node {
  int data;
  Node *pre;
  Node *next;
  // 优化6：构造函数一步初始化，调用点不再手写 4 行赋值
  Node(int d = 0, Node *p = nullptr, Node *n = nullptr)
      : data(d), pre(p), next(n) {}
};

class List {
public:
  List();
  ~List();

  void push_front(int data); // 在头部进行插入
  void push_back(int data);  // 在尾部进行插入

  void pop_front(); // 在链表头部进行删除
  void pop_back();  // 在链表的尾部进行删除

  bool find(int data) const;      // 在链表中进行查找（不修改状态，故为 const）
  void insert(int pos, int data); // 在位置 pos 插入（新节点成为第 pos 个节点）
  void display() const;           // 打印链表
  void erase(int data);           // 删除所有值等于 data 的节点

private:
  Node *_head;
  Node *_tail;
  int _size;
};

// 优化1：直接用 nullptr 初始化，杜绝"new 完立刻覆盖"导致的 2 个节点泄漏
List::List() : _head(nullptr), _tail(nullptr), _size(0) {}

// 优化2：遍历释放所有节点，避免中间节点泄漏 + size==1 时的二次释放
List::~List() {
  Node *pTemp = _head;
  while (pTemp) {
    Node *pNext = pTemp->next; // 先保存后继，再 delete
    delete pTemp;
    pTemp = pNext;
  }
  _head = nullptr;
  _tail = nullptr;
  _size = 0;
}

void List::push_front(int data) {
  Node *pNode = new Node(data); // 优化6：构造即初始化
  if (_size == 0) {
    this->_head = pNode;
    this->_tail = pNode;
    ++_size;
    return;
  }
  pNode->next = this->_head;
  this->_head->pre = pNode;
  this->_head = pNode;
  ++_size;
}

void List::push_back(int data) {
  Node *pNode = new Node(data); // 优化6：构造即初始化
  if (_size == 0) {
    this->_head = pNode;
    this->_tail = pNode;
    ++_size;
    return;
  }
  pNode->pre = this->_tail;
  this->_tail->next = pNode;
  this->_tail = pNode;
  ++_size;
}

void List::display() const {
  Node *pTemp = this->_head;
  cout << endl;
  while (pTemp) {
    cout << pTemp->data << " "; // 优化8：统一使用 cout
    pTemp = pTemp->next;
  }
  if (_size == 0)
    cout << "This is a null List." << endl; // 优化8：统一使用 cout
}

// 优化3：合并分支，删除后根据新头是否为 nullptr 决定是否同步置空 _tail
void List::pop_front() {
  if (_size == 0) {
    cout << "This list is Null, can not pop_front." << endl;
    return;
  }
  cout << "Pop the front member : " << this->_head->data << endl;
  Node *pTemp = this->_head;
  this->_head = this->_head->next; // 先更新头（此时尚未 delete，读 next 安全）
  if (this->_head != nullptr) {
    this->_head->pre = nullptr; // 新头的前驱置空（双链表必须）
  } else {
    this->_tail = nullptr; // 链表被删空，尾也要置空
  }
  delete pTemp; // 最后才释放旧头
  --_size;
}

// 优化3 + 优化4：合并分支，并修正报错文案 pop_back
void List::pop_back() {
  if (_size == 0) {
    cout << "This list is Null, can not pop_back." << endl; // 优化4：修正文案
    return;
  }
  cout << "Pop the back member : " << this->_tail->data << endl;
  Node *pTemp = this->_tail;
  this->_tail = this->_tail->pre; // 先更新尾（此时尚未 delete，读 pre 安全）
  if (this->_tail != nullptr) {
    this->_tail->next = nullptr; // 新尾的后继置空（双链表必须）
  } else {
    this->_head = nullptr; // 链表被删空，头也要置空
  }
  delete pTemp; // 最后才释放旧尾
  --_size;
}

// 优化7：const 成员函数，不修改对象状态
bool List::find(int data) const {
  Node *pTemp = this->_head;
  while (pTemp) {
    if (pTemp->data == data)
      return true;
    pTemp = pTemp->next;
  }
  return false;
}

// 语义：在位置 pos 插入（新节点成为第 pos 个节点）
// 优化说明：保留原逻辑，仅将注释写准确；头插复用 push_front 避免重复代码
void List::insert(int pos, int data) {
  if (pos > _size || pos < 1) {
    cout << "插入位置不合理，功能不支持" << endl;
    return;
  }
  if (pos == 1) {
    push_front(data); // 头插交给 push_front，统一维护 _head/_tail/_size
    return;
  }
  // 定位到第 pos-1 个节点，在其后插入，新节点即为第 pos 个
  Node *pTemp = this->_head;
  for (int i = 1; i < pos - 1; i++) {
    pTemp = pTemp->next;
  }
  Node *pNew = new Node(data); // 优化6：构造即初始化
  pNew->pre = pTemp;
  pNew->next = pTemp->next;
  pTemp->next->pre = pNew; // 此时 pTemp 非尾，next 必非空，安全
  pTemp->next = pNew;
  ++_size;
}

// 优化5：去掉冗余的 find(data) 预判断，直接遍历，找不到自然不删
// 删除所有值等于 data 的节点（先存 next 再删除，避免悬垂指针）
void List::erase(int data) {
  Node *pTemp = this->_head;
  while (pTemp) {
    if (pTemp->data == data) {
      Node *pNext = pTemp->next; // 先保存后继，删除后靠它继续遍历
      if (!pTemp->pre) {
        pop_front(); // 头删，pop_front 内部已维护 _size/_head/_tail
      } else if (!pTemp->next) {
        pop_back(); // 尾删，pop_back 内部已维护 _size/_head/_tail
      } else {
        pTemp->pre->next = pTemp->next;
        pTemp->next->pre = pTemp->pre;
        delete pTemp; // 中间节点手动 delete，必须同步 --_size
        --_size;
      }
      pTemp = pNext; // 从保存的后继继续（不读已释放的节点）
    } else {
      pTemp = pTemp->next;
    }
  }
}

int main(int argc, char *argv[]) {
  List *pList = new List();
  pList->push_front(1);
  pList->push_front(2);
  pList->push_front(3);
  pList->push_back(6);
  pList->push_back(7);
  pList->push_back(8);
  pList->push_back(8);
  pList->push_back(8);
  pList->push_back(8);
  pList->display();
  cout << endl;
  pList->pop_front();
  cout << endl;
  pList->display();
  pList->insert(3, 5);
  cout << endl;
  pList->display();
  cout << endl;
  pList->pop_back();
  pList->display();
  pList->insert(2, 100);
  cout << endl << "after inser(2,100) " << endl;
  pList->display();
  pList->erase(100);
  cout << endl << "after erase(100) " << endl;
  pList->display();
  pList->erase(5);
  pList->display();
  pList->erase(8);
  pList->display();

  delete pList; // 优化2配套：真正触发析构函数，验证整条链表被正确释放

  return 0;
}
