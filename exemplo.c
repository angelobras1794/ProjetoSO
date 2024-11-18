#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define N 9  // Size of the grid (9x9)

// Structure to hold the Sudoku game data
typedef struct {
    int sudoku[N][N];
    int solution[N][N];
    char time_record[20];
    int point_record;
} SudokuGame;

// Function to load the Sudoku game from a JSON file
int load_sudoku_game(const char *filename, SudokuGame *game) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return 0;
    }

    // Read the entire file content into a string
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL) {
        printf("Memory allocation error\n");
        fclose(file);
        return 0;
    }
    fread(file_content, 1, file_size, file);
    fclose(file);
    file_content[file_size] = '\0';

    // Parse JSON
    cJSON *root = cJSON_Parse(file_content);
    free(file_content);
    if (!root) {
        printf("Error parsing JSON\n");
        return 0;
    }

    // Get the "games" array from the JSON
    cJSON *games = cJSON_GetObjectItem(root, "games");
    if (!cJSON_IsArray(games)) {
        printf("Error: 'games' is not an array\n");
        cJSON_Delete(root);
        return 0;
    }

    // Get the first game (assuming we want the first game in the array)

    cJSON *game_json = cJSON_GetArrayItem(games, 0);
    if (!game_json) {
        printf("Error: No game data found\n");
        cJSON_Delete(root);
        return 0;
    }

    // Check if 'tabuleiroInic' is an array
    cJSON *sudoku = cJSON_GetObjectItem(game_json, "tabuleiroInic");
    if (!cJSON_IsArray(sudoku)) {
        printf("Error: 'tabuleiroInic' is not an array\n");
        cJSON_Delete(root);
        return 0;
    }

    // Debug: Print the sudoku data type and structure
    printf("'tabuleiroInic' is an array, checking rows...\n");

    for (int i = 0; i < N; i++) {
        cJSON *row = cJSON_GetArrayItem(sudoku, i);
        if (!cJSON_IsArray(row)) {
            printf("Error: Row %d is not an array\n", i);
            cJSON_Delete(root);
            return 0;
        }

        // Print the row for debugging
        printf("Row %d: ", i);
        for (int j = 0; j < N; j++) {
            int value = cJSON_GetArrayItem(row, j)->valueint;
            printf("%d ", value);
            game->sudoku[i][j] = value;  // Store the value
        }
        printf("\n");
    }

    // Load the solution
    cJSON *solution = cJSON_GetObjectItem(game_json, "solucao");
    if (!cJSON_IsArray(solution)) {
        printf("Error: 'solucao' is not an array\n");
        cJSON_Delete(root);
        return 0;
    }
    for (int i = 0; i < N; i++) {
        cJSON *row = cJSON_GetArrayItem(solution, i);
        if (!cJSON_IsArray(row)) {
            printf("Error: Solution row %d is not an array\n", i);
            cJSON_Delete(root);
            return 0;
        }
        for (int j = 0; j < N; j++) {
            game->solution[i][j] = cJSON_GetArrayItem(row, j)->valueint;
        }
    }

    // Load the time record
    // cJSON *time_record = cJSON_GetObjectItem(game_json, "tempo_recorde");
    // if (cJSON_IsString(time_record)) {
    //     strncpy(game->time_record, time_record->valuestring, sizeof(game->time_record) - 1);
    //     game->time_record[sizeof(game->time_record) - 1] = '\0';
    // }

    // // Load the point record
    // cJSON *point_record = cJSON_GetObjectItem(game_json, "pontuacao_recorde");
    // if (cJSON_IsNumber(point_record)) {
    //     game->point_record = point_record->valueint;
    // }

    cJSON_Delete(root);
    return 1;  // Successfully loaded the game
}

// Function to print the Sudoku board
void print_sudoku(int board[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

int main() {
    SudokuGame game;

    // Load the Sudoku game from a JSON file
    if (load_sudoku_game("Jogos.json", &game)) {
        printf("Sudoku Puzzle:\n");
        print_sudoku(game.sudoku);
        printf("\nSolution:\n");
        print_sudoku(game.solution);
    } else {
        printf("Failed to load Sudoku game from JSON.\n");
    }

    return 0;
}
