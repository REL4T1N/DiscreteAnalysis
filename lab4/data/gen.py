import random
import string

def generate_word(length):
    return ''.join(random.choices(string.ascii_lowercase, k=length))

def generate_random_test(filename, text_size, pattern_size):
    print(f"Генерация {filename}...")
    with open(filename, 'w') as f:
        # Генерируем паттерн
        pattern = [generate_word(random.randint(3, 10)) for _ in range(pattern_size)]
        f.write(" ".join(pattern) + "\n")
        
        # Генерируем текст
        text = [generate_word(random.randint(3, 10)) for _ in range(text_size)]
        # Искусственно вставим паттерн куда-нибудь в середину, чтобы он точно нашелся
        insert_pos = text_size // 2
        text[insert_pos:insert_pos+pattern_size] = pattern
        
        f.write(" ".join(text) + "\n")

def generate_worst_test(filename, text_size, pattern_size):
    print(f"Генерация {filename}...")
    with open(filename, 'w') as f:
        # Паттерн: 499 раз "a" и в конце "b"
        pattern = ["a"] * (pattern_size - 1) + ["b"]
        f.write(" ".join(pattern) + "\n")
        
        # Текст: 1 000 000 раз "a"
        text = ["a"] * text_size
        f.write(" ".join(text) + "\n")

if __name__ == "__main__":
    TEXT_WORDS = 1_000_000
    PATTERN_WORDS = 500
    
    generate_random_test("random_test.in", TEXT_WORDS, PATTERN_WORDS)
    generate_worst_test("worst_test.in", TEXT_WORDS, PATTERN_WORDS)
    print("Готово!")