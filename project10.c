#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
    long long product; // 세 과목의 곱
} Student;

// 정렬 비교 횟수 카운터
long long sort_comparisons = 0;

// 데이터 로드 함수
Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int capacity = 10;
    int count = 0;
    Student* arr = malloc(sizeof(Student) * capacity);

    if (!arr) return NULL;

    fgets(line, sizeof(line), fp); // 헤더 스킵

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = realloc(arr, sizeof(Student) * capacity);
            if (!temp) { free(arr); fclose(fp); return NULL; }
            arr = temp;
        }

        Student s;
        char* token = strtok(line, ",");
        if(token) s.id = atoi(token);
        
        token = strtok(NULL, ",");
        if(token) strncpy(s.name, token, MAX_NAME_LEN);
        
        token = strtok(NULL, ",");
        if(token) s.gender = token[0];
        
        token = strtok(NULL, ",");
        if(token) s.korean = atoi(token);
        
        token = strtok(NULL, ",");
        if(token) s.english = atoi(token);
        
        token = strtok(NULL, ",");
        if(token) s.math = atoi(token);

        // 곱 계산
        s.product = (long long)s.korean * s.english * s.math;

        arr[count++] = s;
    }
    fclose(fp);
    Student* tight = realloc(arr, sizeof(Student) * count);
    if (tight) arr = tight;
    *out_count = count;
    return arr;
}

// 순차 탐색
int sequential_search(Student* arr, int n, long long target, long long* comparisons) {
    *comparisons = 0;
    for (int i = 0; i < n; i++) {
        (*comparisons)++;
        if (arr[i].product == target) {
            return i;
        }
    }
    return -1;
}

// 정렬 비교 함수 (qsort용)
int compare_students(const void* a, const void* b) {
    sort_comparisons++;
    long long val_a = ((Student*)a)->product;
    long long val_b = ((Student*)b)->product;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

// 이진 탐색
int binary_search(Student* arr, int n, long long target, long long* comparisons) {
    *comparisons = 0;
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        (*comparisons)++;
        if (arr[mid].product == target) return mid;

        (*comparisons)++;
        if (arr[mid].product < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

int main(void) {
    srand((unsigned int)time(NULL));

    int count = 0;
    Student* students = load_students("/Users/hwangsungmin/Desktop/수업자료/자료구조2/dataset_id_ascending.csv", &count);

    if (!students) return 1;
    printf("데이터 로드 완료: %d명\n", count);

    // -----------------------------------------------------------
    // 탐색이 성공할 때까지 임의의 값 생성 (데이터 내 존재하는 값 선택)
    // -----------------------------------------------------------
    int random_index = rand() % count;
    long long target = students[random_index].product;

    printf("탐색 목표 값: %lld\n\n", target);

    // 1. 순차 탐색
    long long seq_comparisons = 0;
    sequential_search(students, count, target, &seq_comparisons);
    
    printf("[순차 탐색]\n");
    printf("  - 순차탐색 비교 횟수: %lld\n\n", seq_comparisons);

    // 2. 정렬 후 이진 탐색
    sort_comparisons = 0;
    // 퀵정렬 사용
    qsort(students, count, sizeof(Student), compare_students);
    
    long long bin_comparisons = 0;
    binary_search(students, count, target, &bin_comparisons);
    
    printf("[퀵정렬 후 이진 탐색]\n");
    printf("  - 퀵정렬 비교 횟수: %lld\n", sort_comparisons);
    printf("  - 이진탐색 비교 횟수: %lld\n", bin_comparisons);
    printf("  - 총 비교 횟수: %lld\n", sort_comparisons + bin_comparisons);

    free(students);
    return 0;
}
