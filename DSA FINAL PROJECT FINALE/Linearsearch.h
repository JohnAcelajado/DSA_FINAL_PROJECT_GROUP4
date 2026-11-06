#ifndef linear_search_h
#define linear_search_h

template <typename T, typename K, typename GetKey>
int linearSearchAll(T data[], int arrSize, K findKey, GetKey getKey, int matches[]) {
    int found  = 0;
    int i = 0;

    while (i < arrSize) {
        if (getKey(data[i]) == findKey) {
            matches[found]  =   i;
            found++;
        }
        i++;
    }
    return found;
}

#endif