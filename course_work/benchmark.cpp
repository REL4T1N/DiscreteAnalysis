#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <zlib.h>

using Bytes = std::vector<unsigned char>;
using Clock = std::chrono::steady_clock;
using Milliseconds = std::chrono::duration<double, std::milli>;

constexpr std::uint32_t EOF_CODE = 26;
constexpr unsigned int RANDOM_SEED = 20261111;
constexpr int COMPRESSION_LEVEL = 6;

struct Codec {
    const char* name;
    Bytes (*encode)(const std::string&);
    std::string (*decode)(const Bytes&, std::size_t);
};

// Store each integer in four bytes, starting with the least significant byte.
void writeCode(Bytes& output, std::uint32_t code) {
    for (int byteIndex = 0; byteIndex < 4; ++byteIndex) {
        output.push_back(static_cast<unsigned char>(code >> (8 * byteIndex)));
    }
}

std::uint32_t readCode(const Bytes& input, std::size_t& position) {
    if (position + 4 > input.size()) {
        throw std::runtime_error("Truncated code");
    }

    std::uint32_t code = 0;
    for (int byteIndex = 0; byteIndex < 4; ++byteIndex) {
        code |= std::uint32_t(input[position++]) << (8 * byteIndex);
    }
    return code;
}

std::pair<std::unordered_map<std::string, int>, int> encodeDictionaryKeys(const std::string& alphabet) {
    std::unordered_map<std::string, int> dictionary;
    int code = 0;
    for (char symbol : alphabet) {
        dictionary[std::string(1, symbol)] = code;
        ++code;
    }
    return {dictionary, ++code}; // т.к. 26 занято eof
}

std::pair<std::unordered_map<int, std::string>, int> decodeDictionaryKeys(const std::string& alphabet) {
    std::unordered_map<int, std::string> dictionary;
    int code = 0;
    for (; code < static_cast<int>(alphabet.size()); ++code) {
        dictionary[code] = std::string(1, alphabet[code]);
    } 
    return {dictionary, ++code}; // т.к. 26 занято eof
} 

std::vector<int> lzwCompress(const std::string& encodeString, const std::string& alphabet) {
    auto [dictionary, nextCode] = encodeDictionaryKeys(alphabet);
    std::vector<int> result;
    std::string currentString;
    for (char symbol : encodeString) {
        std::string candidateString = currentString + symbol;
        if (dictionary.contains(candidateString)) {
            currentString = candidateString;
        } else {
            result.emplace_back(dictionary[currentString]);
            dictionary[candidateString] = nextCode;
            ++nextCode;
            currentString = std::string(1, symbol);
        }
    }
    if (!currentString.empty()) {
        result.emplace_back(dictionary[currentString]);
    }
    result.emplace_back(EOF_CODE);
    
    return result;
}

std::string lzwDecompress(std::vector<int> decodeArray, const std::string& alphabet) {
    if (decodeArray.empty() || decodeArray[0] == EOF_CODE) {
        return "";
    }
    auto [dictionary, nextCode] = decodeDictionaryKeys(alphabet);
    std::string currentString = dictionary[decodeArray[0]];
    std::string result = currentString;

    for (std::size_t i = 1; i < decodeArray.size(); ++i) {
        int code = decodeArray[i];

        if (code == EOF_CODE) {
            break;
        }
        std::string newString;
        if (dictionary.contains(code)) {
            newString = dictionary[code];
        } else {
            newString = currentString + std::string(1, currentString[0]);
        }

        std::string candidateString = currentString + std::string(1, newString[0]);
        dictionary[nextCode] = candidateString;
        ++nextCode;

        currentString = newString;
        result += currentString;
    }

    return result;
}

// Двоичное представление для сравнения размеров: четыре байта на код.
Bytes lzwEncode(const std::string& input) {
    const auto codes = lzwCompress(input, "abcdefghijklmnopqrstuvwxyz");
    Bytes output;
    for (int code : codes) {
        writeCode(output, static_cast<std::uint32_t>(code));
    }
    return output;
}

std::string lzwDecode(const Bytes& input, std::size_t) {
    std::vector<int> codes;
    std::size_t position = 0;
    while (position < input.size()) {
        codes.push_back(static_cast<int>(readCode(input, position)));
    }
    return lzwDecompress(codes, "abcdefghijklmnopqrstuvwxyz");
}

Bytes rleEncode(const std::string& input) {
    Bytes output;
    std::size_t runStart = 0;

    while (runStart < input.size()) {
        std::size_t runEnd = runStart + 1;
        while (runEnd < input.size()
               && input[runEnd] == input[runStart]
               && runEnd - runStart < UINT32_MAX) {
            ++runEnd;
        }

        output.push_back(static_cast<unsigned char>(input[runStart]));
        writeCode(output, static_cast<std::uint32_t>(runEnd - runStart));
        runStart = runEnd;
    }
    return output;
}

std::string rleDecode(const Bytes& input, std::size_t) {
    std::string result;
    std::size_t position = 0;

    while (position < input.size()) {
        char symbol = char(input[position++]);
        std::uint32_t runLength = readCode(input, position);
        result.append(runLength, symbol);
    }
    return result;
}

Bytes deflateEncode(const std::string& input) {
    uLongf outputSize = compressBound(input.size());
    Bytes output(outputSize);

    int status = compress2(
        output.data(), &outputSize,
        reinterpret_cast<const Bytef*>(input.data()), input.size(),
        COMPRESSION_LEVEL
    );
    if (status != Z_OK) {
        throw std::runtime_error("compress2 failed");
    }

    output.resize(outputSize);
    return output;
}

std::string deflateDecode(const Bytes& input, std::size_t originalSize) {
    std::string result(std::max<std::size_t>(originalSize, 1), '\0');
    uLongf outputSize = result.size();

    int status = uncompress(
        reinterpret_cast<Bytef*>(result.data()), &outputSize,
        input.data(), input.size()
    );
    if (status != Z_OK) {
        throw std::runtime_error("uncompress failed");
    }

    result.resize(outputSize);
    return result;
}

double median(std::vector<double> measurements) {
    std::sort(measurements.begin(), measurements.end());
    return measurements[measurements.size() / 2];
}

void checkCodecs(const std::vector<Codec>& codecs) {
    for (const auto& codec : codecs) {
        for (const std::string input : {
                 "", "a", "aaa", "abababa", "abracadabra",
                 "bananabandana", "abadabaababa"
             }) {
            if (codec.decode(codec.encode(input), input.size()) != input) {
                throw std::runtime_error("Self test failed");
            }
        }
    }

    Bytes expected;
    for (auto code : {0, 1, 17, 0, 2, 0, 3, 27, 29, 26}) {
        writeCode(expected, code);
    }
    if (lzwEncode("abracadabra") != expected) {
        throw std::runtime_error("Reference codes differ");
    }
}

std::string generateInput(std::size_t size, const std::string& kind) {
    std::string input(size, 'a');
    std::mt19937 generator(RANDOM_SEED);
    const std::string pattern = "abracadabra";

    for (std::size_t index = 0; index < size; ++index) {
        if (kind == "periodic") {
            input[index] = pattern[index % pattern.size()];
        }
        if (kind == "random") {
            // Reject the final 22 values to obtain equal probabilities for 26 letters.
            std::uint32_t randomValue;
            do {
                randomValue = generator();
            } while (randomValue >= 4294967274u);

            input[index] = char('a' + randomValue % 26);
        }
    }
    return input;
}

void benchmarkCodec(const Codec& codec, const std::string& input,
                    const std::string& kind, int repetitions) {
    // Warm up once; this run is not included in the measurements.
    auto warmup = codec.encode(input);
    if (codec.decode(warmup, input.size()) != input) {
        throw std::runtime_error("Warmup mismatch");
    }

    std::vector<double> encodeTimes;
    std::vector<double> decodeTimes;
    std::size_t encodedSize = 0;

    for (int repetition = 0; repetition < repetitions; ++repetition) {
        auto encodeStart = Clock::now();
        auto encoded = codec.encode(input);
        auto decodeStart = Clock::now();
        auto restored = codec.decode(encoded, input.size());
        auto decodeEnd = Clock::now();

        // Verification is outside the timed interval.
        if (restored != input) {
            throw std::runtime_error("Round trip mismatch");
        }

        encodedSize = encoded.size();
        encodeTimes.push_back(Milliseconds(decodeStart - encodeStart).count());
        decodeTimes.push_back(Milliseconds(decodeEnd - decodeStart).count());
    }

    std::cout << kind << ',' << input.size() << ',' << codec.name << ','
              << encodedSize << ',' << double(encodedSize) / input.size() << ','
              << median(encodeTimes) << ',' << median(decodeTimes) << '\n';
}

int main(int argc, char** argv) {
    try {
        int repetitions = argc > 1 ? std::stoi(argv[1]) : 5;
        if (repetitions < 1 || repetitions % 2 == 0) {
            throw std::runtime_error("Use positive odd repetitions");
        }

        std::vector<Codec> codecs = {
            {"DEFLATE", deflateEncode, deflateDecode},
            {"LZW", lzwEncode, lzwDecode},
            {"RLE", rleEncode, rleDecode}
        };
        checkCodecs(codecs);

        std::cerr << "zlib=" << zlibVersion()
                  << ", level=" << COMPRESSION_LEVEL
                  << ", repetitions=" << repetitions
                  << ", seed=" << RANDOM_SEED << "; self-tests OK\n";
        std::cout << "dataset,input_bytes,codec,encoded_bytes,ratio,encode_ms,decode_ms\n"
                  << std::fixed << std::setprecision(6);

        for (std::size_t size : {1000, 10000, 100000, 1000000}) {
            for (const std::string kind : {"constant", "periodic", "random"}) {
                std::string input = generateInput(size, kind);
                for (const auto& codec : codecs) {
                    benchmarkCodec(codec, input, kind, repetitions);
                }
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
