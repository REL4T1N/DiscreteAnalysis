'''
Псевдокодим:

у нас допустим число int - обычно в ЯП это 4 байтовая штуку
Нужно перевести это в двоичную штуку
Далее создаём маску чтобы отсортировать элементы по левому байту
Создаём результирующий список
Т.е. мы маской отделяем правый байт, получаем значения от 0 до 255 и сортируем их сортировкой подсчётом в временный список
результирующий список = временный список
Отсортировав правый байт, сдвигаем маску на 1 байт
По новой маске выделяем следующий байт в числе
Сдвигаем эти байт в диапазон от 0 до 255 и опять выравниваем сортировкой подсчётом в новый временный список, к этому моменту числа уже выравнены по предыдущему байту, поэтому мы можем просто сдвинуть нас текущий байт на самый правый и отсортировать их по нему в диапазоне 0 255
результирующий список = временный список
ИТЕРИРУЕМСЯ, пока не пройдём по всем байтам в записи самого большого числа (следовательно в начале необходимо найти самое больше число и количество байт в его записи - отдельная функция)
в результирующем списке получаем ответ, который сохраняем в файл
'''

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

def get_byte_count(users: List[User]) -> int:
    max_score = users[0].score
    for user in users:
        if user.score > max_score:
            max_score = user.score

    if max_score == 0:
        return 1

    return (max_score.bit_length() + 7) // 8

def radix_sort_bitwise(users: List[User]) -> List[User]:
    if not users:
        return []
    
    byte_count = get_byte_count(users)
    MASK = 0xFF
    result = users.copy()
    for k in range(byte_count):
        count = [0] * 256
        for user in result:
            byte = (user.score >> (8 * k)) & MASK
            count[byte] += 1

        for i in range(1, len(count)):
            count[i] += count[i-1]

        temp = [None]*len(result)
        for user in reversed(result):
            byte = (user.score >> (8 * k)) & MASK
            pos = count[byte] - 1
            temp[pos] = user
            count[byte] -= 1

        result = temp
    
    return result

def main():
    input_file_prefix = "../data/input/bitwise/users_"
    users_in_file = [10, 50, 100, 1000, 10000]
    output_file_prefix = "../data/output/bitwise/py_sorted_users_"
    file_format = ".csv"

    for users_count in users_in_file:
        input_file = input_file_prefix + str(users_count) + file_format
        output_file = output_file_prefix + str(users_count) + file_format

        users = read_users(input_file)
        print(f"Прочитано {len(users)} пользователей")
        sorted_users = radix_sort_bitwise(users)
        write_users(output_file, sorted_users)
        print(f"Отсортированные данные записаны в {output_file}")


if __name__ == "__main__":
    main()