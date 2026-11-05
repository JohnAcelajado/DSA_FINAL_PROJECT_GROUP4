#ifndef QUEUE_H
#define QUEUE_H
#include <iostream>

template <typename T>
class qNode {
public:
    T data;
    qNode* next;
};

template <typename T>
qNode<T>* new_node(T newData) {
    qNode<T>* newNode = new qNode<T>;
    newNode->data = newData;
    newNode->next = nullptr;
    return newNode;
}

template <typename T>
bool isEmpty(qNode<T>* frontPtr) {
    return frontPtr == nullptr;
}

template <typename T>
void enqueue(qNode<T>** frontPtr, qNode<T>** backPtr, T newData) {
    qNode<T>* newNode = new_node(newData);

    if (*frontPtr == nullptr && *backPtr == nullptr) {
        *frontPtr = newNode;
        *backPtr = newNode;
    } else {
        (*backPtr)->next = newNode;
        *backPtr = newNode;
    }
}

template <typename T>
bool peek(qNode<T>* frontPtr, T& result) {
    if (frontPtr == nullptr) return false;
    result = frontPtr->data;
    return true;
}

template <typename T>
bool dequeue(qNode<T>** frontPtr, qNode<T>** backPtr, T& result) {
    if (*frontPtr == nullptr) return false;

    qNode<T>* deleteNode = *frontPtr;
    result = deleteNode->data;
    *frontPtr = (*frontPtr)->next;
    if (*frontPtr == nullptr) {
        *backPtr = nullptr;
    }
    delete deleteNode;
    return true;
}

template <typename T>
void clearQueue(qNode<T>** frontPtr, qNode<T>** backPtr) {
    T temp;
    while (dequeue(frontPtr, backPtr, temp)) {}
}

#endif