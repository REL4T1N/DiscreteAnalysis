import random
import string
import os

# Настройки
NUM_RANDOM = 1_000_000
NUM_SORTED = 1_000_000
NUM_SEARCH_INSERT = 100_000
NUM_SEARCH_QUERIES = 1_000_000

def random_word(length=10):
    return ''.join(random.choices(string.ascii_lowercase, k=length))

def generate_random():
    print("Generating test_random.in ...")
    with open("test_random.in", "w") as f:
        for _ in range(NUM_RANDOM):
            f.write(f"+ {random_word(random.randint(5, 15))} {random.randint(1, 1000)}\n")

def generate_sorted():
    print("Generating test_sorted.in ...")
    with open("test_sorted.in", "w") as f:
        # Генерируем лексикографически отсортированные слова 
        # (например: aaaaaaa, aaaaaab, aaaaaac ...)
        for i in range(NUM_SORTED):
            # Конвертируем число в строку из букв (симуляция base26)
            chars = []
            n = i
            for _ in range(7):
                chars.append(chr(ord('a') + (n % 26)))
                n //= 26
            word = "".join(reversed(chars))
            f.write(f"+ {word} {i}\n")

def generate_search():
    print("Generating test_search.in ...")
    pool = [random_word(10) for _ in range(NUM_SEARCH_INSERT)]
    with open("test_search.in", "w") as f:
        # Сначала вставляем пул слов
        for i, word in enumerate(pool):
            f.write(f"+ {word} {i}\n")
        
        # Затем генерируем запросы на поиск (80% существующие, 20% случайные)
        for _ in range(NUM_SEARCH_QUERIES):
            if random.random() < 0.8:
                f.write(f"{random.choice(pool)}\n")
            else:
                f.write(f"{random_word(10)}\n")

if __name__ == "__main__":
    # Переходим в директорию скрипта, чтобы файлы создавались рядом с ним
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    generate_random()
    generate_sorted()
    generate_search()
    print("All datasets generated successfully!")