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

	// для тех кому нужно сохранить заголовок
	// if err := writer.Write([]string{"id", "score", "name", "surname"}); err != nil {
	// 	   return fmt.Errorf("ошибка записи заголовка: %w", err)
	// }

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

func countingSortByScore(users []*User) []*User {
	if len(users) == 0 {
		return users
	}

	minScore := users[0].Score
	maxScore := users[0].Score

	for _, user := range users {
		if user.Score < minScore {
			minScore = user.Score
		}
		if user.Score > maxScore {
			maxScore = user.Score
		}
	}

	rangeSize := maxScore - minScore + 1
	count := make([]int, rangeSize)

	for _, user := range users {
		index := user.Score - minScore
		count[index]++
	}

	for i := 1; i < rangeSize; i++ {
		count[i] += count[i-1]
	}

	sortedUsers := make([]*User, len(users))
	for i := len(users) - 1; i >= 0; i-- {
		user := users[i]
		index := user.Score - minScore
		pos := count[index] - 1
		sortedUsers[pos] = user
		count[index]--
	}

	return sortedUsers
}

func main() {
	inputFilePrefix := "../data/input/users_"
	outputFilePrefix := "../data/output/go_sorted_users_"
	fileFormat := ".csv"
	usersInFile := []int{10, 50, 100, 1000, 100000, 1000000}

	for _, countUsers := range usersInFile {
		inputFile := inputFilePrefix + strconv.Itoa(countUsers) + fileFormat
		outputFile := outputFilePrefix + strconv.Itoa(countUsers) + fileFormat

		users, err := readUsers(inputFile)
		if err != nil {
			fmt.Printf("Ошибка при чтении файла %s: %v\n", inputFile, err)
			continue
		}

		fmt.Printf("Прочитано %d пользователей\n", len(users))
		sortedUsers := countingSortByScore(users)

		if err := writeUsers(outputFile, sortedUsers); err != nil {
			fmt.Printf("Ошибка при записи в файл %s: %v\n", outputFile, err)
			continue
		}

		fmt.Printf("Отсортированные данные записаны в %s\n", outputFile)
	}
}
