import numpy as np


def find_difference_with_numpy(array):
    if len(array) == 0:  # Проверяем, что массив не пустой
        return None

    max_elem = np.max(array)  # Находим максимум
    min_elem = np.min(array)  # Находим минимум

    return max_elem - min_elem


# Пример использования
array = np.array([3, 7, 1, 9, 4])
difference = find_difference_with_numpy(array)
print(f"Вывод: {difference}")
