# Считаваем файл, например ../data/input/decimal/users_10.csv
# Сохраням данные в структуру, например массив элементов класса User для Python и соответсвующие реализации для других ЯП (Go, C, C++)

# Как мы сортируем? Естественно по score
# пройти по всем пользователям
#     найти максимальное количество цифр в записи одного числа поля score

# Обозначим номер разряда k 
# Чтобы отделить от числа значение конкретного разряда необходимо (score // 10^k) % 10
# создать результирующий список = исходному?
# Дальше итерируемся по всем разрядам от 0 до k - 1?
#     Создаём массив для сортировки подсчётом от 0 до 9
#     итерируемся по пользователям
#         находим значени заданного разряда score
#         увеличиваем значени по этому индексу в массиве частот

#     создаём массив префиксных сумм (преобразование частотного массива)
#     создаём временный массив для хранения отсортированных по заданному разряду пользователей
#     итерируемся по пользователям в обратном порядке
#         расставляем для конкретного разряда пользователь в отсортированном порядке во временный массив

#     результирующий массив = временный массив

# Получаем ответ
# записываем его в файл, например ../data/output/decimal/py_sorted_users_10.csv и тд для соответсвующего количества записей в файле и выбранного ЯП для реализации

import csv
from typing import List, Dict, Any

class User:
    __slots__ = ("id", "score", "name", "surname")

    def __init__(self, id: int, score: int, name: str, surname: str):
        self.id = id
        self.score = score
        self.name = name
        self.surname = surname

def read_users(filename: str) -> List[User]:
    users = []
    with open(filename, "r", encoding="UTF-8") as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            if not row:
                continue

            user = User(
                id=int(row[0]),
                score=int(row[1]),
                name = row[2],
                surname= row[3]
            )
            users.append(user)
    return users
    
def write_users(filename: str, users: List[User]):
    with open(filename, "w", encoding="UTF-8", newline="") as f:
        writer = csv.writer(f)
        for user in users:
            writer.writerow([user.id, user.score, user.name, user.surname])

def get_max_digit(users: List[User]) -> int:
    max_score = 0
    for user in users:
        if user.score > max_score:
            max_score = user.score

    return len(str(max_score))

def radix_sort_decimal(users: List[User]) -> List[User]:
    """
    Поразрядная сортировка по score (десятичная реализация)
    Возвращает новый отсортированный список
    """

    if not users:
        return users
    
    result = users.copy()

    max_digit = get_max_digit(users)
    for k in range(max_digit):
        count = [0]*10
        for user in result:
            digit = (user.score // (10**k)) % 10
            count[digit] += 1

        for i in range(1, len(count)): # лучше вместо len(count) использовать сразу '10', НО так менее читаемо
            count[i] += count[i-1]

        temp = [None] * len(result)
        for user in reversed(result):
            digit = (user.score // (10**k)) % 10
            pos = count[digit] - 1
            temp[pos] = user
            count[digit] -= 1

        result = temp

    return result

def main():
    input_file_prefix = "../data/input/decimal/users_"
    users_in_file = [10, 50, 100, 1000, 10000]
    output_file_prefix = "../data/output/decimal/py_sorted_users_"
    file_format = ".csv"

    for users_count in users_in_file:
        input_file = input_file_prefix + str(users_count) + file_format
        output_file = output_file_prefix + str(users_count) + file_format

        users = read_users(input_file)
        print(f"Прочитано {len(users)} пользователей")
        sorted_users = radix_sort_decimal(users)
        write_users(output_file, sorted_users)
        print(f"Отсортированные данные записаны в {output_file}")


if __name__ == "__main__":
    main()


