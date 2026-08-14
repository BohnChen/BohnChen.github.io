#include <iostream>
#include <string>

using std::cout;
using std::endl;

struct Node {
  int data;
  Node *pre;
  Node *next;
};

class List {
public:
  List();
  ~List();

  void push_front(int data); // 在头部进行插入
  void push_back(int data);  // 在尾部进行插入

  void pop_front(); // 在链表头部进行删除
  void pop_back();  // 在链表的尾部进行删除

  bool find(int data);            // 在链表中进行查找
  void insert(int pos, int data); // 在指定位置后面插入pos
  void display() const;           // 打印链表
  void erase(int data);           // 删除一个指定的节点

private:
  Node *_head;
  Node *_tail;
  int _size;
};
List::List() : _head(new Node()), _tail(new Node()), _size(0) {
  _head = nullptr;
  _tail = nullptr;
}
List::~List() {
  delete _head;
  delete _tail;
}
void List::push_front(int data) {
  Node *pNode = new Node();
  pNode->data = data;
  pNode->next = nullptr;
  pNode->pre = nullptr;
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

// 在尾部进行插入
void List::push_back(int data) {
  Node *pNode = new Node();
  pNode->data = data;
  pNode->next = nullptr;
  pNode->pre = nullptr;
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
    std::cout << pTemp->data << " ";
    pTemp = pTemp->next;
  }
  if (_size == 0)
    std::cout << "This is a null List." << std::endl;
}
void List::pop_front() {
  if (_size == 0) {
    std::cout << "This list is Null, can not pop_front." << std::endl;
    return;
  } else if (_size == 1) {
    std::cout << "Pop the only member : " << this->_head->data << std::endl;
    delete this->_head;
    this->_head = nullptr;
    this->_tail = nullptr;
    --_size;
  } else {
    std::cout << "Pop the front member : " << this->_head->data << std::endl;
    Node *pTemp = this->_head;
    this->_head = this->_head->next;
    this->_head->pre = nullptr;
    delete pTemp;
    pTemp = nullptr;
    --_size;
  }
}
void List::pop_back() {
  if (_size == 0) {
    std::cout << "This list is Null, can not pop_front." << std::endl;
    return;
  } else if (_size == 1) {
    std::cout << "Pop the only member : " << this->_tail->data << std::endl;
    delete this->_tail;
    this->_head = nullptr;
    this->_tail = nullptr;
    --_size;
  } else {
    std::cout << "Pop the back member : " << this->_tail->data << std::endl;
    Node *pTemp = this->_tail;
    // 如需访问先不要回收
    this->_tail = this->_tail->pre;
    this->_tail->next = nullptr;
    delete pTemp;
    pTemp = nullptr;
    --_size;
  }
}
// 在链表中进行查找
bool List::find(int data) {
  Node *pTemp = this->_head;
  while (pTemp) {
    if (pTemp->data == data)
      return true;
    pTemp = pTemp->next;
  }
  return false;
}
// 在指定位置后面插入pos
void List::insert(int pos, int data) {
  if (pos > _size || pos < 1) {
    std::cout << "插入位置不合理，功能不支持" << std::endl;
    return;
  }
  Node *pTemp = this->_head;
  for (int i = 1; i < pos - 1; i++) {
    pTemp = pTemp->next;
  }
  if (pos == 1) {
    push_front(data);
    return;
  }

  Node *pNew = new Node();
  pNew->data = data;
  pNew->pre = pTemp;
  pNew->next = pTemp->next;
  pTemp->next->pre = pNew;
  pTemp->next = pNew;
  ++_size;
}

// 删除一个指定的节点
void List::erase(int data) {
  if (find(data)) {

    Node *pTemp = this->_head;
    while (pTemp) {
      if (pTemp->data == data) {
        Node *pNext = pTemp->next;
        if (!pTemp->pre) {
          pop_front();
        } else if (!pTemp->next) {
          pop_back();
        } else {
          pTemp->pre->next = pTemp->next;
          pTemp->next->pre = pTemp->pre;
          delete pTemp;
          pTemp = nullptr;
          --_size;
        }
        pTemp = pNext;
      } else {
        pTemp = pTemp->next;
      }
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
  std::cout << endl << "after erase(100) " << std::endl;
  pList->display();
  pList->erase(5);
  pList->display();
  pList->erase(8);
  pList->display();

  return 0;
}
