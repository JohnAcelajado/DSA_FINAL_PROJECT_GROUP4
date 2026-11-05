#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

template <typename T>
class DynamicArray {
private:
    T* items;
    int count;
    int capacity;

    void grow() {
        capacity = capacity * 2;
        T* bigger = new T[capacity];
        for (int i = 0; i < count; i++) {
            bigger[i] = items[i];
        }
        delete[] items;
        items = bigger;
    }

public:
    DynamicArray() {
        capacity = 4;
        count = 0;
        items = new T[capacity];
    }
    ~DynamicArray() { delete[] items; }
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    void push_back(const T& value) {
        if (count == capacity) grow();
        items[count] = value;
        count++;
    }
    T& operator[](int index) { return items[index]; }
    int size() const { return count; }
    T* data() { return items; }
};

#endif