#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

template <typename T, typename K, typename GetKey>
int binarySearch(T data[], int arrSize, K findKey, GetKey getKey) {
    int low = 0;
    int high = arrSize - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        K midKey = getKey(data[mid]);

        if (midKey == findKey) {
            return mid;
        }
        if (midKey < findKey) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

#endif