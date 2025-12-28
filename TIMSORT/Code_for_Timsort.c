#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
//MIN_RUN вообще задаётся динамически, но тут пример
#define MIN_RUN 8
#define MAX_STACK 85

//Структура для хранения серии
typedef struct {
    int start;   //Начальный индекс серии
    int length;  //Длина серии
} Run;

int insertion_sort_range(int arr[], int left, int right);
int merge_runs(int arr[], int left, int mid, int right);
void merge_at(int arr[], Run stack[], int i);
int should_merge(Run stack[], int stack_size);
int timsort_with_stack(int arr[], int n);
void print_array(int arr[], int n);

int main() {
    setlocale(LC_CTYPE, "RUS");
    int arr[] = {
        // Разные естественные серии:
        100, 101, 102, 103, 104, 105, 106,107,108,109,  //10
        90, 89, 88, 87, 86, // 5  
        200, 201, 202, 203, // 4
        150, 151, 152, 153, 154, 155,156,  // 7
        50, 51, 52, 53, 54, // 5
        300, 301, 302, 303, 304, 305, 306, 307, 308, // 9
    };
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Исходный массив:\n");
    puts("");
    print_array(arr, n);
    puts("\n");
    if (timsort_with_stack(arr, n)) {
        printf("Отсортированный массив:\n");
        puts("");
        print_array(arr, n);
    }
    else {
        printf("Ошибка при сортировке!\n");
    }
    puts("\n\n\n\n\n\n\n");
    return 0;
}

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

//Сортировка вставками для диапазона
int insertion_sort_range(int arr[], int left, int right) {
    if (left < 0 || right < left || right < 0) {
        return 0;
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
    return 1;
}

//ФУНКЦИЯ СЛИЯНИЯ
int merge_runs(int arr[], int left, int mid, int right) {
    if (left > mid || mid >= right || left < 0 || right < 0) {
        return 0;
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

    // Копируем данные во временные массивы
    for (int i = 0; i < len1; i++) {
        left_arr[i] = arr[left + i];
    }
    for (int i = 0; i < len2; i++) {
        right_arr[i] = arr[mid + 1 + i];
    }

    //Сливаем временные массивы обратно в arr[left....right]
    int i = 0;     //Индекс для left_arr
    int j = 0;     //Индекс для right_arr
    int k = left;  //Индекс для arr[]

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

    //Копируем оставшиеся элементы left_arr, если есть
    while (i < len1) {
        arr[k] = left_arr[i];
        i++;
        k++;
    }

    //Копируем оставшиеся элементы right_arr, если есть
    while (j < len2) {
        arr[k] = right_arr[j];
        j++;
        k++;
    }

    //Освобождаем память
    free(left_arr);
    free(right_arr);

    return 1;
}

//Слияние двух серий в стеке на позиции i (i и i+1)
void merge_at(int arr[], Run stack[], int i) {
    //i - индекс первой серии для слияния
    //Вторая серия находится по индексу i+1

    int left = stack[i].start;
    int mid = stack[i].start + stack[i].length - 1;
    int right = stack[i + 1].start + stack[i + 1].length - 1;

    //Выполняем слияние
    merge_runs(arr, left, mid, right);

    //Обновляем первую серию (теперь содержит объединенную)
    stack[i].length += stack[i + 1].length;

    //Сдвигаем остальные серии в стеке
    for (int j = i + 1; j < MAX_STACK - 1; j++) {
        stack[j] = stack[j + 1];
    }
}

//Проверка условий X, Y, Z
int should_merge(Run stack[], int stack_size) {
    // Нужно минимум 3 серии
    if (stack_size < 3) {
        return 0;
    }

    int x = stack_size - 3;  // X (третья с конца)
    int y = stack_size - 2;  // Y (вторая с конца)
    int z = stack_size - 1;  // Z (последняя)

    //Проверяем ОБА условия:
    //1. len(X) > len(Y) + len(Z)
    //2. len(Y) > len(Z)
    //Если НЕ выполняются оба - нужно сливать

    int condition1 = stack[x].length > stack[y].length + stack[z].length;
    int condition2 = stack[y].length > stack[z].length;

    return !(condition1 && condition2);
}

//Функция Timsort со стеком и правилами X, Y, Z
int timsort_with_stack(int arr[], int n) {
    if (MIN_RUN > n) {
        //Если MIN_RUN больше размера массива,
        //просто сортируем весь массив вставками
        insertion_sort_range(arr, 0, n - 1);
        return 1;
    }
    if (n <= 0) return 0;
    if (n == 1) return 1;

    Run stack[MAX_STACK];
    int stack_size = 0;

    int i = 0;

    while (i < n) {
        //Находим следующую серию
        int j = i;

        // Поиск конца текущей серии
        if (j < n - 1) {
            if (arr[j] <= arr[j + 1]) {
                //Возрастающая серия
                while (j < n - 1 && arr[j] <= arr[j + 1]) {
                    j++;
                }
            }
            else {
                //Убывающая серия
                while (j < n - 1 && arr[j] > arr[j + 1]) {
                    j++;
                }

                //Разворачиваем убывающую серию
                for (int l = i, r = j; l < r; l++, r--) {
                    int temp = arr[l];
                    arr[l] = arr[r];
                    arr[r] = temp;
                }
            }
        }

        //Расширяем маленькие серии до MIN_RUN
        int run_length = j - i + 1;
        if (run_length < MIN_RUN) {
            j = (i + MIN_RUN - 1 < n) ? i + MIN_RUN - 1 : n - 1;
            run_length = j - i + 1;
            insertion_sort_range(arr, i, j);
        }

        //Добавляем серию в стек
        stack[stack_size].start = i;
        stack[stack_size].length = run_length;
        stack_size++;

        //Проверяем и выполняем слияния по правилам X, Y, Z
        while (stack_size > 1) {
            if (stack_size >= 3) {
                //Есть X, Y, Z
                if (should_merge(stack, stack_size)) {
                    //Нужно сливать
                    int x = stack_size - 3;
                    int y = stack_size - 2;
                    int z = stack_size - 1;

                    //Сливаем две меньшие соседние серии
                    if (stack[x].length <= stack[z].length) {
                        //X ≤ Z, сливаем X и Y
                        merge_at(arr, stack, y - 1);  // Сливаем на позиции y-1 (X) и y (Y)
                        stack_size--;
                    }
                    else {
                        //X > Z, сливаем Y и Z
                        merge_at(arr, stack, y);  // Сливаем на позиции y (Y) и z (Z)
                        stack_size--;
                    }
                }
                else {
                    break;
                }
            }
            else if (stack_size == 2) {
                //Только Y и Z
                int y = 0;
                int z = 1;

                //Проверяем только условие Y > Z
                if (stack[y].length <= stack[z].length) {
                    // Y ≤ Z, сливаем
                    merge_at(arr, stack, y);
                    stack_size--;
                }
                else {
                    //Условие выполнено
                    break;
                }
            }
        }

        i = j + 1;
    }

    //В конце сливаем все оставшиеся серии в стеке
    while (stack_size > 1) {
        merge_at(arr, stack, stack_size - 2);
        stack_size--;
    }

    return 1;
}
