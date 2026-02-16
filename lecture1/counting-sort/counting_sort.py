# прочитать данные из файла user_10.csv
# сохранить прочитанные данные пользователей согласно структуре
# структура пользоватлея: 
#   id: int
#   score: int # Баллы за экзамен
#   name: string
#   surname: string 
# Получившаяся струтура должна иметь ссылки на память, где хранится каждый пользователь, 
# т.е. при дальнейшей сортировке мне нужно будет упорядочить всех пользователей по параметру score сортировкой подсчётом,
# следовательно, я должен иметь возможность переставлять пользователей как угодно. 
# Реализация планируется на 4 ЯП: Python, C++, Go, C. Если в ЯП имеется подходящая структура, эффективная по памяти и времени - можно пользоваться
# Для каждого ЯП должна получиться самая эффективная реализация по памяти (по времени у нас и так O(n) для сортировки подсчётом)

# К моменту имеем пользователей в структуре для хранения их
# Необходимо подсчитать диапазон значений для хранения score (примерно 97% находится в диапазоне 250-300, остальные точно больше 190 но меньше 250)
# Решить вопрос, как хранить диапазон значений для score: просто массив где элемент [user_score - min_user_score] будет хранить количество таких пользователей или хеш мапу где ключ - user_score - min_user_score, а значение - количество студентов с таким score 
# Начинаем циклом for проходить по всем пользователям
#     смотрим score пользователя
#     1) если массив: по индексу user_score - min_user_score увеличиваем значение на 1
#     2) если хеш мапа: по ключу user_score-min_user_score увеличиваем значение на 1

# создаём структуру для пользователей, где они будут отсортированы
# Идем по структуре с пользователями в обратном порядке:
#     1) смотрим score пользователя
#     2) смотрим какой счётчик у такого score
#     3) записываем в итоговую структуру на позицию [счётчик - 1]
#     4) уменьшаем счётчик у соответсвующего score на 1

# получаем отсортированных пользователей
# записываем результат в файл sorted_users_10.csv

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
    # чтение пользователей из файла
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

def counting_sort_by_score(users: List[User]) -> List[User]:
    if not users:
        return []
    
    # найти диапазон значений
    min_score = users[0].score
    max_score = users[0].score
    for user in users:
        if user.score < min_score:
            min_score = user.score
        if user.score > max_score:
            max_score = user.score

    # Массив с счётчиками
    range_size = max_score - min_score + 1
    count = [0] * range_size

    # Чтение пользователей для заполнения счётчикв
    for user in users:
        index = user.score - min_score
        count[index]+=1

    # Преобразование счётчиков в префиксные суммы
    for i in range(1, range_size):
        count[i] += count[i - 1]

    # результирующий массив
    sorted_users = [None] * len(users)

    # Обход массива пользователей в обратном порядке согласно алгоритму
    for user in reversed(users):
        index = user.score - min_score
        pos = count[index] - 1
        sorted_users[pos] = user
        count[index] -= 1

    return sorted_users

def main():
    input_file_path = "../data/input/"
    input_file_prefix = "users_"
    users_in_file = [10, 50, 100, 1000, 100000, 1000000]
    output_file_path = "../data/output/"
    output_file_prefix = "py_sorted_users_"
    file_format = ".csv"

    for users_count in users_in_file:
        input_file = input_file_path + input_file_prefix + str(users_count) + file_format
        output_file = output_file_path + output_file_prefix + str(users_count) + file_format

        users = read_users(input_file)
        print(f"Прочитано {len(users)} пользователей")
        sorted_users = counting_sort_by_score(users)
        write_users(output_file, sorted_users)
        print(f"Отсортированные данные записаны в {output_file}")


if __name__ == "__main__":
    main()









