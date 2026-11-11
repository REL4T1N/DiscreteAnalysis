# a = dict()

# alphabet = 'abcdefghijklmnopqrstuvwxyz'
# k = 0
# for char in alphabet:
#     a[char]=k
#     k+=1

# a['EOF'] = k
# print(a)

# {
#     'a': 0, 
#     'b': 1, 
#     'c': 2, 
#     'd': 3, 
#     'e': 4, 
#     'f': 5, 
#     'g': 6, 
#     'h': 7, 
#     'i': 8, 
#     'j': 9, 
#     'k': 10, 
#     'l': 11, 
#     'm': 12, 
#     'n': 13, 
#     'o': 14, 
#     'p': 15, 
#     'q': 16, 
#     'r': 17, 
#     's': 18, 
#     't': 19, 
#     'u': 20, 
#     'v': 21, 
#     'w': 22, 
#     'x': 23, 
#     'y': 24, 
#     'z': 25, 
#     'EOF': 26
# }

def data_encode(alphabet: list) -> tuple[dict, int]:
    a = dict()
    k = 0
    for char in alphabet:
        a[char]=k
        k += 1
    return a, k


def data_decode(alphabet: list) -> tuple[dict, int]:
    a = dict()
    for iter in range(len(alphabet)):
        a[iter] = alphabet[iter]
    return a, len(alphabet)

def lzw_encode(alphabet: list, encode_string: str) -> list:
    keys_dict, num = data_encode(alphabet)
    print(keys_dict)
    result = []
    sub_string = encode_string[0]
    for i in range(1, len(encode_string)):
        char = encode_string[i]
        cand_sub_string = sub_string + char
        if cand_sub_string in keys_dict.keys():
            sub_string = cand_sub_string
        else:
            result.append(keys_dict[sub_string])
            keys_dict[cand_sub_string] = num
            num += 1
            sub_string = char

    result.append(keys_dict[sub_string])
    result.append(keys_dict["EOF"])

    return result

def lzw_decode(alphabet: list, decode_array: list) -> str:
    keys_dict, num = data_decode(alphabet)

    cur_string = keys_dict[decode_array[0]]
    result =  cur_string

    for i in range(1, len(decode_array)):
        code = decode_array[i]

        if code == 26:
            break

        if code in keys_dict.keys():
            new_str = keys_dict[code]
        else:
            new_str = cur_string + cur_string[0]

        candidate = cur_string + new_str[0]
        keys_dict[num] = candidate
        num += 1
        cur_string = new_str
        result += cur_string


        # if code in keys_dict.keys():
        #     keys_dict[num] = candidate
        #     num += 1
        #     cur_string = keys_dict[code]
        # result += keys_dict[code]
    print(keys_dict)
    return result



def main():
    alphabet = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', "EOF"]
    # alphabet = ['A', 'B']
    # keys_dict = data(alphabet=alphabet)
    # print(keys_dict)
    # print(len(keys_dict.keys()))
    encode = int(input("Выберите режим:\n\t1) Кодирование\n\t2) Декодирование\n"))
    if encode == 1:
        encode_string = input("Введите строку для кодирования:\n")
        result = lzw_encode(alphabet, encode_string)
        print(' '.join([str(char) for char in result]))
    elif encode == 2:
        decode_array = list(map(int, input("Введите последовательность для декодирования:\n").split()))
        result = lzw_decode(alphabet, decode_array)
        print(result)
        

if __name__ == "__main__":
    main()


