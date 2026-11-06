#ifndef stack_h
#define  stack_h

template <typename T>
class sNode {
public:
    T data;
    sNode* next;  
};

template <typename T>
bool isEmpty(sNode<T>* topPtr) {
    return topPtr == nullptr;
}

// add on top - o(1)
template <typename T>
void push(sNode<T>** topPtr, T newData) {
    sNode<T>* newNode = new sNode<T>;
    newNode->data = newData;
    newNode->next  = *topPtr;
    *topPtr = newNode;
}

template <typename T>
bool peek(sNode<T>* topPtr, T& result) {
    if (topPtr == nullptr) return false;
    result = topPtr->data;
    return true;
}

template <typename T>
bool pop(sNode<T>** topPtr, T& result) {
    if (*topPtr == nullptr) return false;

    sNode<T>* deleteNode = *topPtr; 
    result = deleteNode->data;
    *topPtr = (*topPtr)->next;   
    delete deleteNode;
    return true;
}

template <typename T>
void clearStack(sNode<T>** topPtr) {
    T temp;
    while (pop(topPtr, temp)) {}
}

#endif