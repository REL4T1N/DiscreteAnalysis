import csv
import json
import random

# Константы для генерации
NAMES = [
    "Ivan", "Alex", "Maria", "Dmitry", "Elena",
    "Petr", "Olga", "Svetlana", "Igor", "Natalia"
]
SURNAMES = [
    "Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov",
    "Popov", "Vasiliev", "Sokolov", "Novikov"
]


def get_random_score():
    """
    Генерация score согласно условию:
    97-98% в диапазоне 250-300, остальные в 190-250.
    """
    probability = random.random()
    if probability <= 0.975:  # 97.5% случаев
        return random.randint(250, 300)
    else:
        return random.randint(190, 250)


def generate_user(user_id):
    """Создает один словарь с данными пользователя"""
    return {
        "id": user_id,
        "score": get_random_score(),
        "name": random.choice(NAMES),
        "surname": random.choice(SURNAMES),
    }


def save_to_csv(filename, count):
    """Потоковое сохранение в CSV"""
    print(f"Запись {count} строк в {filename}...")
    fieldnames = ["id", "score", "name", "surname"]

    with open(filename, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()

        for i in range(1, count + 1):
            writer.writerow(generate_user(i))

            if i % 1000000 == 0:
                print(f"Прогресс: {i} строк записано...")


def save_to_json(filename, count):
    """Потоковое сохранение в JSON (массив объектов)"""
    print(f"Запись {count} объектов в {filename}...")
    with open(filename, 'w', encoding='utf-8') as f:
        f.write("[\n")
        for i in range(1, count + 1):
            user = generate_user(i)
            json_str = json.dumps(user)
            f.write(f"  {json_str}")

            if i < count:
                f.write(",\n")
            else:
                f.write("\n")

            if i % 1000000 == 0:
                print(f"Прогресс: {i} объектов записано...")
        f.write("]")


def main():
    counts = [10, 50, 100, 1000, 100000, 1000000]

    # Формат файла: 'csv' или 'json'
    file_format = 'csv'

    for count in counts:
        filename = f"./data/input/users_{count}.{file_format}"
        try:
            if file_format == 'csv':
                save_to_csv(filename, count)
            else:
                save_to_json(filename, count)
            print(f"Успешно: {filename} создан.\n")
        except Exception as e:
            print(f"Ошибка при создании {filename}: {e}")


if __name__ == "__main__":
    main()