#include <iostream>
#include <vector>

void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp(right - left + 1);
    
    int i = left;      
    int j = mid;   
    int k = 0;         
    
    while (i < mid && j < right) {
        if (arr[i] <= arr[j]) {
            temp[k] = arr[i];
            i++;
        } else {
            temp[k] = arr[j];
            j++;
        }
        k++;
    }
    
    while (i <= mid) {
        temp[k] = arr[i];
        i++;
        k++;
    }
    
    while (j <= right) {
        temp[k] = arr[j];
        j++;
        k++;
    }
    
    for (int idx = 0; idx < temp.size(); idx++) {
        arr[left + idx] = temp[idx];
    }
}

void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    
    mergeSort(arr, left, mid);
    mergeSort(arr, mid, right);
    
    merge(arr, left, mid, right);
}

void mergeSort(std::vector<int>& arr) {
    if (!arr.empty()) {
        mergeSort(arr, 0, arr.size());
    }
}

void printArray(const std::vector<int>& arr) {
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    
    std::cout << "Исходный массив: ";
    printArray(arr);
    
    mergeSort(arr);
    
    std::cout << "Отсортированный массив: ";
    printArray(arr);
    
    return 0;
}