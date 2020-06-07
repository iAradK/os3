#include <iostream>
#include "ThreadSafeList.h"

int main3() {
    List<int> lst;
    cout << "aaa \n";
    lst.insert(5);
    lst.myPrint();
    lst.insert(1);
    lst.myPrint();
    lst.insert(2);
    lst.myPrint();
    lst.insert(7);
    lst.myPrint();
    lst.insert(0);
    lst.insert(-2);
    lst.insert(15);
    lst.insert(3);
    lst.insert(-20);
    lst.myPrint();
    lst.remove(-20);
    lst.myPrint();
    lst.remove(15);
    lst.myPrint();
    return 0;
}
