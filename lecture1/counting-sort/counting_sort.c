#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    int score;
    char name[50];
    char surname[50];
} User;

User* read_users(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ошибка открытий файла %s\n", filename);
        return NULL;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), file);


    *count = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) > 1) (*count)++;
    }

    rewind(file);
    fgets(buffer, sizeof(buffer), file);

    User* users = (User*)malloc(*count * sizeof(User));
    if (!users) {
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < *count; i++) {
        fgets(buffer, sizeof(buffer), file);
        sscanf(buffer, "%d,%d,%49[^,],%49s", 
            &users[i].id,
            &users[i].score,
            users[i].name,
            users[i].surname   
        );
    }

    fclose(file);
    return users;
}

void write_user(const char* filename, User* users, int count) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("ошибка созданя файла %s\n", filename);
        return;
    }

    fprintf(file, "id,score,name,surname\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%d,%s,%s\n",
            users[i].id,
            users[i].score,
            users[i].name,
            users[i].surname    
        );
    }

    fclose(file);
}


User* counting_sort_by_score(User* users, int count) {
    if (count == 0) return NULL;

    int min_score = users[0].score;
    int max_score = users[0].score;

    for (int i = 1; i < count; i++) {
        if (users[i].score < min_score) min_score = users[i].score;
        if (users[i].score > max_score) max_score = users[i].score;
    }

    int range_size = max_score - min_score + 1;
    int *count_array = (int*)calloc(range_size, sizeof(int));
    if (!count_array) return NULL;

    for (int i = 0; i < count; i++) {
        int index = users[i].score - min_score;
        count_array[index]++;
    }

    for (int i = 1; i < range_size; i++) {
        count_array[i] += count_array[i - 1];
    }

    User* sorted_users = (User*)malloc(count * sizeof(User));
    if (!sorted_users) {
        free(count_array);
        return NULL;
    }

    for (int i = count - 1; i >= 0; i--) {
        int index = users[i].score - min_score;
        int pos = count_array[index] - 1;
        sorted_users[pos] = users[i];
        count_array[index]--;
    }

    free(count_array);
    return sorted_users;
}

int main() {
    const char* input_prefix = "../data/input/users_";
    const char* output_prefix = "../data/output/c_sorted_users_";
    const char* format = ".csv";

    int users_counts[] = {10, 50, 100, 1000, 100000, 1000000};
    int num_counts = sizeof(users_counts) / sizeof(users_counts[0]);

    for (int i = 0; i < num_counts; i++) {
        char input_file[256];
        char output_file[256];

        sprintf(input_file, "%s%d%s", input_prefix, users_counts[i], format);
        sprintf(output_file, "%s%d%s", output_prefix, users_counts[i], format);

        int count;
        User* users = read_users(input_file, &count);
        if (!users) {
            printf("ошибка чтения файла %s\n", input_file);
            continue;
        }
        printf("прочитано %d пользователей\n", count);

        User* sorted_users = counting_sort_by_score(users, count);
        if (!sorted_users) {
            printf("ошибка сортировки\n");
            free(users);
            continue;
        }

        write_user(output_file, sorted_users, count);
        printf("отсортированные данные записаны в %s\n", output_file);

        free(users);
        free(sorted_users);
    }

    return 0;
}