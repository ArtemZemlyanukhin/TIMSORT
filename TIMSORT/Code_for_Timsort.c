#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#define MIN_RUN 4

int insertion_sort_range(int arr[], int left, int right);
int merge_runs(int arr[], int left, int mid, int right);
int timsort(int arr[], int n);
void print_array(int arr[], int n);

int main() {
    setlocale(LC_CTYPE, "RUS");
    int arr[] = { 1, -4, -5, -33, 17, -2, 8, 23, 25, 0, -12, 34, 88, -99,123,1001 };
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Исходный массив:\n");
    print_array(arr, n);

    // Сортируем массив
    int result = timsort(arr, n);

    if (result == 1) {
        printf("Отсортированный массив:\n");
        print_array(arr, n);
    }
    else {
        printf("Ошибка при сортировке!\n");
    }

    return 0;
}

// Функция для вывода массива
void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Сортировка вставками для диапазона
// Возвращает 1 при успехе, 0 при ошибке
int insertion_sort_range(int arr[], int left, int right) {
    if (left < 0 || right < left || right < 0) {
        return 0; // Некорректные параметры
    }

    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }

    return 1; // Успех
}

// Слияние двух отсортированных серий
// Возвращает 1 при успехе, 0 при ошибке
int merge_runs(int arr[], int left, int mid, int right) {
    if (left > mid || mid >= right || left < 0 || right < 0) {
        return 0; // Некорректные параметры
    }

    int len1 = mid - left + 1;
    int len2 = right - mid;

    
    int* left_arr = (int*)malloc(len1 * sizeof(int));
    int* right_arr = (int*)malloc(len2 * sizeof(int));

    if (left_arr == NULL || right_arr == NULL) {
        if (left_arr) free(left_arr);
        if (right_arr) free(right_arr);
        return 0; 
    }

    // Копируем данные
    for (int i = 0; i < len1; i++) {
        left_arr[i] = arr[left + i];
    }
    for (int i = 0; i < len2; i++) {
        right_arr[i] = arr[mid + 1 + i];
    }

    // Сливаем
    int i = 0, j = 0, k = left;

    while (i < len1 && j < len2) {
        if (left_arr[i] <= right_arr[j]) {
            arr[k] = left_arr[i];
            i++;
        }
        else {
            arr[k] = right_arr[j];
            j++;
        }
        k++;
    }

    // Остатки из левого подмассива
    while (i < len1) {
        arr[k] = left_arr[i];
        k++;
        i++;
    }

    // Остатки из правого подмассива
    while (j < len2) {
        arr[k] = right_arr[j];
        k++;
        j++;
    }

    free(left_arr);
    free(right_arr);

    return 1;
}

// Основная функция Timsort
// Возвращает 1 при успехе, 0 при ошибке
int timsort(int arr[], int n) {
    if (n <= 0) {
        return 0; 
    }

    if (n == 1) {
        return 1; 
    }

    // Шаг 1: Поиск и обработка серий
    int i = 0;

    while (i < n) {
        int j = i;

        // Поиск конца текущей серии
        if (j < n - 1) {
            if (arr[j] <= arr[j + 1]) {
                while (j < n - 1 && arr[j] <= arr[j + 1]) {
                    j++;
                }
            }
            else {                
                while (j < n - 1 && arr[j] > arr[j + 1]) {
                    j++;
                }

                // Разворачиваем убывающую серию
                for (int l = i, r = j; l < r; l++, r--) {
                    int temp = arr[l];
                    arr[l] = arr[r];
                    arr[r] = temp;
                }
            }
        }

        // Расширяем маленькие серии
        if (j - i + 1 < MIN_RUN) {
            j = (i + MIN_RUN - 1 < n - 1) ? i + MIN_RUN - 1 : n - 1;
        }

        // Сортируем серию вставками
        if (!insertion_sort_range(arr, i, j)) {
            return 0; 
        }

        i = j + 1;
    }

    // Шаг 2: Слияние серий
    for (int size = MIN_RUN; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            if (mid >= n - 1) {
                break; 
            }

            int right = (left + 2 * size - 1 < n - 1) ?
                left + 2 * size - 1 : n - 1;

            if (!merge_runs(arr, left, mid, right)) {
                return 0; 
            }
        }
    }

    return 1; 
}
