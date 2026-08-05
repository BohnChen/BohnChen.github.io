// Example showing pointers vs references
// 汇编命令
// g++ -S -O0 testRefAndPointer.cc -o RefAndp.s
// - -S — 编译到汇编但不链接
// - -O0 — 无优化（方便对比指针/引用的原始实现）
// - -O2 — 常用优化级别，可看两者最终是否一样
void byPointer(int* p) {
    *p = 42;
}

void byReference(int& r) {
    r = 42;
}
