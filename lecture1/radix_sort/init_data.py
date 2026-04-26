import csv
import json
import random
import os

# Константы для генерации
NAMES = [
    "Ivan", "Alex", "Maria", "Dmitry", "Elena",
    "Petr", "Olga", "Svetlana", "Igor", "Natalia"
]
SURNAMES = [
    "Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov",
    "Popov", "Vasiliev", "Sokolov", "Novikov"
]

def ensure_dir(directory):
    """Создает директорию, если она не существует"""
    if not os.path.exists(directory):
        os.makedirs(directory)
        print(f"Создана директория: {directory}")

# ========== Генерация для десятичной сортировки ==========

def get_decimal_score():
    """
    Для десятичной сортировки: числа от 0 до 999
    Специально делаем разные диапазоны для наглядности
    """
    choice = random.random()
    if choice < 0.3:  # 30% - маленькие числа (1-2 разряда)
        return random.randint(0, 99)
    elif choice < 0.7:  # 40% - средние числа (3 разряда)
        return random.randint(100, 999)
    else:  # 30% - большие числа (до 9999 для демонстрации)
        return random.randint(1000, 9999)

def generate_decimal_user(user_id):
    return {
        "id": user_id,
        "score": get_decimal_score(),
        "name": random.choice(NAMES),
        "surname": random.choice(SURNAMES),
    }

def save_decimal_data(filename, count):
    """Сохранение данных для десятичной сортировки"""
    ensure_dir(os.path.dirname(filename))
    print(f"Запись {count} строк в {filename}...")
    
    with open(filename, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=["id", "score", "name", "surname"])
        writer.writeheader()
        
        for i in range(1, count + 1):
            writer.writerow(generate_decimal_user(i))

# ========== Генерация для побитовой сортировки ==========

def get_bitwise_score():
    """
    Для побитовой сортировки: числа от 0 до 255 (8 бит)
    Специальные случаи для демонстрации
    """
    special_cases = [0, 255, 128, 1, 2, 4, 8, 16, 32, 64, 127, 254]
    
    if random.random() < 0.2:  # 20% - специальные случаи
        return random.choice(special_cases)
    else:
        return random.randint(0, 255)

def generate_bitwise_user(user_id):
    return {
        "id": user_id,
        "score": get_bitwise_score(),
        "name": random.choice(NAMES),
        "surname": random.choice(SURNAMES),
    }

def save_bitwise_data(filename, count):
    """Сохранение данных для побитовой сортировки"""
    ensure_dir(os.path.dirname(filename))
    print(f"Запись {count} строк в {filename}...")
    
    with open(filename, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=["id", "score", "name", "surname"])
        writer.writeheader()
        
        for i in range(1, count + 1):
            writer.writerow(generate_bitwise_user(i))

# ========== Генерация отладочных наборов ==========

def save_debug_sets():
    """Создание маленьких наборов для отладки алгоритмов"""
    
    # Для десятичной сортировки - специально подобранные числа
    decimal_debug = [
        {"id": 1, "score": 123, "name": "Test", "surname": "One"},
        {"id": 2, "score": 45, "name": "Test", "surname": "Two"},
        {"id": 3, "score": 789, "name": "Test", "surname": "Three"},
        {"id": 4, "score": 1, "name": "Test", "surname": "Four"},
        {"id": 5, "score": 999, "name": "Test", "surname": "Five"},
        {"id": 6, "score": 350, "name": "Test", "surname": "Six"},
        {"id": 7, "score": 351, "name": "Test", "surname": "Seven"},
        {"id": 8, "score": 349, "name": "Test", "surname": "Eight"},
    ]
    
    # Для побитовой сортировки - специально подобранные битовые комбинации
    bitwise_debug = [
        {"id": 1, "score": 255, "name": "Test", "surname": "AllOnes"},    # 11111111
        {"id": 2, "score": 128, "name": "Test", "surname": "MSB"},        # 10000000
        {"id": 3, "score": 1, "name": "Test", "surname": "LSB"},          # 00000001
        {"id": 4, "score": 170, "name": "Test", "surname": "PatternA"},   # 10101010
        {"id": 5, "score": 85, "name": "Test", "surname": "PatternB"},    # 01010101
        {"id": 6, "score": 0, "name": "Test", "surname": "Zero"},
        {"id": 7, "score": 127, "name": "Test", "surname": "Mid"},        # 01111111
        {"id": 8, "score": 192, "name": "Test", "surname": "HighBits"},   # 11000000
    ]
    
    # Сохраняем отладочные наборы
    decimal_debug_file = "./data/input/decimal/debug_8.csv"
    ensure_dir(os.path.dirname(decimal_debug_file))
    with open(decimal_debug_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=["id", "score", "name", "surname"])
        writer.writeheader()
        writer.writerows(decimal_debug)
    print(f"Создан отладочный набор для десятичной сортировки: {decimal_debug_file}")
    
    bitwise_debug_file = "./data/input/bitwise/debug_8.csv"
    ensure_dir(os.path.dirname(bitwise_debug_file))
    with open(bitwise_debug_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=["id", "score", "name", "surname"])
        writer.writeheader()
        writer.writerows(bitwise_debug)
    print(f"Создан отладочный набор для побитовой сортировки: {bitwise_debug_file}")

def main():
    """Главная функция генерации данных"""
    
    # Создаем необходимые директории
    ensure_dir("./data/input/decimal")
    ensure_dir("./data/input/bitwise")
    ensure_dir("./data/output/decimal")
    ensure_dir("./data/output/bitwise")
    
    # Создаем отладочные наборы
    print("=== Создание отладочных наборов ===")
    save_debug_sets()
    
    # Генерируем данные для десятичной сортировки
    print("\n=== Генерация данных для десятичной сортировки ===")
    decimal_sizes = [10, 50, 100, 1000, 10000]
    for size in decimal_sizes:
        filename = f"./data/input/decimal/users_{size}.csv"
        save_decimal_data(filename, size)
    
    # Генерируем данные для побитовой сортировки
    print("\n=== Генерация данных для побитовой сортировки ===")
    bitwise_sizes = [10, 50, 100, 1000, 10000]
    for size in bitwise_sizes:
        filename = f"./data/input/bitwise/users_{size}.csv"
        save_bitwise_data(filename, size)
    
    print("\n✅ Все данные успешно сгенерированы!")

if __name__ == "__main__":
    main()