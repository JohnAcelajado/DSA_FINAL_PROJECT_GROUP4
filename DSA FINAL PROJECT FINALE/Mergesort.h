#ifndef merge_sort_h
#define  merge_sort_h

template <typename T, typename Compare>
void mergeParts(T arr[], T temp[], int left, int mid, int right, Compare goesBefore) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (goesBefore(arr[j], arr[i])) {
            temp[k++] = arr[j++];
        } else {
            temp[k++] = arr[i++];
        }
    }
    while (i <= mid)   temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++]; 

    for (int x  =  left; x <= right; x++) {
        arr[x] = temp[x];
    }
}

template <typename T, typename Compare>
void mergeSortRange(T arr[], T temp[], int left, int right, Compare goesBefore) {
    if (left >=  right) return;

    int mid = left + (right - left) / 2;   
    mergeSortRange(arr, temp, left, mid, goesBefore);
    mergeSortRange(arr, temp, mid + 1, right, goesBefore);
    mergeParts(arr, temp, left, mid, right, goesBefore);
}

template <typename T, typename Compare>
void mergeSort(T arr[], int arrSize, Compare goesBefore) {
    if (arrSize < 2) return;
    T* temp = new T[arrSize];
    mergeSortRange(arr, temp, 0, arrSize - 1, goesBefore);
    delete[] temp;
}

#endif