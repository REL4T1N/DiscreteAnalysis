package main

import (
	"encoding/csv"
	"fmt"
	"io"
	"os"
	"strconv"
)

type User struct {
	ID      int
	Score   int
	Name    string
	Surname string
}

func readUsers(filename string) ([]*User, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("ошибка открытия файла: %w", err)
	}
	defer file.Close()

	reader := csv.NewReader(file)
	if _, err := reader.Read(); err != nil {
		return nil, fmt.Errorf("ошибка чтения файла: %w", err)
	}

	var users []*User

	for {
		row, err := reader.Read()
		if err == io.EOF {
			break
		}

		if err != nil {
			return nil, fmt.Errorf("ошибка чтения строки: %w", err)
		}

		if len(row) < 4 {
			continue
		}

		id, err := strconv.Atoi(row[0])
		if err != nil {
			return nil, fmt.Errorf("ошибка преобразование id в int: %w", err)
		}

		score, err := strconv.Atoi(row[1])
		if err != nil {
			return nil, fmt.Errorf("ошибка преобразования score в int: %w", err)
		}

		user := &User{
			ID:      id,
			Score:   score,
			Name:    row[2],
			Surname: row[3],
		}
		users = append(users, user)
	}

	return users, nil
}

func writeUsers(filename string, users []*User) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("ошибка создания файла: %w", err)
	}
	defer file.Close()

	writer := csv.NewWriter(file)
	defer writer.Flush()

	for _, user := range users {
		row := []string{
			strconv.Itoa(user.ID),
			strconv.Itoa(user.Score),
			user.Name,
			user.Surname,
		}
		if err := writer.Write(row); err != nil {
			return fmt.Errorf("ошибка записи строки: %w", err)
		}
	}

	return nil
}

func getMaxDigit(users []*User) int {
	maxScore := 0
	for _, user := range users {
		if user.Score > maxScore {
			maxScore = user.Score
		}
	}

	return len(strconv.Itoa(maxScore))
}

func radixSortDecimal(users []*User) []*User {
	if len(users) == 0 {
		return users
	}

	result := users
	maxDigit := getMaxDigit(users)
	pow := 1
	for k := 0; k < maxDigit; k++ {
		count := make([]int, 10)
		for _, user := range result {
			digit := (user.Score / pow) % 10
			count[digit]++
		}

		for i := 1; i < len(count); i++ {
			count[i] += count[i-1]
		}

		temp := make([]*User, len(result))
		for i := len(result) - 1; i >= 0; i-- {
			user := result[i]
			digit := (user.Score / pow) % 10
			pos := count[digit] - 1
			temp[pos] = user
			count[digit]--
		}
		result = temp
		pow *= 10
	}
	return result
}

func main() {
	inputFilePrefix := "../data/input/decimal/users_"
	outputFilePrefix := "../data/output/decimal/go_sorted_users_"
	fileFormat := ".csv"
	usersInFile := []int{10, 50, 100, 1000, 10000}

	for _, countUsers := range usersInFile {
		inputFile := inputFilePrefix + strconv.Itoa(countUsers) + fileFormat
		outputFile := outputFilePrefix + strconv.Itoa(countUsers) + fileFormat

		users, err := readUsers(inputFile)
		if err != nil {
			fmt.Printf("Ошибка при чтении файла %s: %v\n", inputFile, err)
			continue
		}

		fmt.Printf("Прочитано %d пользователей\n", len(users))
		sortedUsers := radixSortDecimal(users)

		if err := writeUsers(outputFile, sortedUsers); err != nil {
			fmt.Printf("Ошибка при записи в файл %s: %v\n", outputFile, err)
			continue
		}

		fmt.Printf("Отсортированные данные записаны в %s\n", outputFile)
	}
}
