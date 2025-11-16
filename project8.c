#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 

#define ARRAY_SIZE 10000   // 데이터 개수
#define MAX_VAL 1000000    // 최대값 (0 ~ 1,000,000)
#define NUM_RUNS 100       // 실행 횟수

/**
 * @brief 배열에 무작위 데이터를 생성합니다.
 * @param arr 데이터를 채울 배열
 * @param n 배열의 크기
 */
void generate_data(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (MAX_VAL + 1);
    }
}

// --- 1. 단순 삽입 정렬 ---
/**
 * @brief 단순 삽입 정렬을 수행하고 비교 횟수를 계산합니다.
 * @param arr 정렬할 배열
 * @param n 배열의 크기
 * @param comps 비교 횟수를 저장할 포인터 (long long)
 */
void insertion_sort(int arr[], int n, long long *comps) {
    *comps = 0;
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // key를 arr[j]와 비교
        while (j >= 0) {
            (*comps)++; // 비교 1회 수행 (arr[j] vs key)
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            } else {
                break; // key가 arr[j]보다 크거나 같으므로 위치 찾음
            }
        }
        arr[j + 1] = key;
    }
}

// --- 2. 기본 쉘 정렬 (n/2, n/4, ... 1 간격) ---
/**
 * @brief 기본 쉘 정렬(간격: n/2, n/4, ...)을 수행하고 비교 횟수를 계산합니다.
 * @param arr 정렬할 배열
 * @param n 배열의 크기
 * @param comps 비교 횟수를 저장할 포인터 (long long)
 */
void shell_sort_basic(int arr[], int n, long long *comps) {
    *comps = 0;
    // 간격(gap)을 n/2부터 1이 될 때까지 2로 나눠가며 줄임
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 각 간격에 대해 gapped-insertion sort 수행
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i - gap;

            while (j >= 0) {
                (*comps)++; // 비교 1회 수행 (arr[j] vs temp)
                if (arr[j] > temp) {
                    arr[j + gap] = arr[j];
                    j -= gap;
                } else {
                    break;
                }
            }
            arr[j + gap] = temp;
        }
    }
}

// --- 3. 최적 쉘 정렬 (Knuth 간격) ---
/**
 * @brief 최적화된 쉘 정렬(Knuth 간격: 1, 4, 13, 40, ...)을 수행합니다.
 * Knuth의 (3^k - 1) / 2 또는 3*k + 1 방식이 널리 쓰입니다.
 * 여기서는 3*k + 1 (1, 4, 13, 40, 121, ...)을 사용합니다.
 * @param arr 정렬할 배열
 * @param n 배열의 크기
 * @param comps 비교 횟수를 저장할 포인터 (long long)
 */
void shell_sort_optimal(int arr[], int n, long long *comps) {
    *comps = 0;

    // 1. Knuth 간격(gap) 생성
    // (1, 4, 13, 40, 121, 364, 1093, 3280, 9841, ...)
    int gap = 1;
    // n/3을 기준으로 하여 gap이 n을 넘지 않도록 함
    while (gap <= n / 3) {
        gap = gap * 3 + 1;
    }

    // 2. 간격을 줄여가며 정렬 수행
    for (/* gap은 위에서 계산된 최대값 */; gap > 0; gap = (gap - 1) / 3) {
        // 각 간격에 대해 gapped-insertion sort 수행
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i - gap;

            while (j >= 0) {
                (*comps)++; // 비교 1회 수행 (arr[j] vs temp)
                if (arr[j] > temp) {
                    arr[j + gap] = arr[j];
                    j -= gap;
                } else {
                    break;
                }
            }
            arr[j + gap] = temp;
        }
    }
}


// --- 메인 함수 ---
int main() {
    // srand는 프로그램 실행 시 한 번만 호출
    srand((unsigned int)time(NULL));

    // 원본 데이터와 정렬에 사용할 사본 배열 선언
    // (static을 사용하거나 힙에 할당하여 스택 오버플로우 방지)
    static int original_data[ARRAY_SIZE];
    static int working_copy[ARRAY_SIZE];

    // 100회 실행 동안의 총 비교 횟수 (오버플로우 방지)
    long long total_insertion_comps = 0;
    long long total_shell_basic_comps = 0;
    long long total_shell_optimal_comps = 0;
    
    long long current_comps = 0; // 각 실행의 비교 횟수

    printf("데이터 %d개, 총 %d회 실행 시작...\n", ARRAY_SIZE, NUM_RUNS);

    for (int run = 0; run < NUM_RUNS; run++) {
        // 1. 새로운 무작위 데이터 생성
        generate_data(original_data, ARRAY_SIZE);

        // 2. 단순 삽입 정렬 실행
        memcpy(working_copy, original_data, sizeof(int) * ARRAY_SIZE);
        insertion_sort(working_copy, ARRAY_SIZE, &current_comps);
        total_insertion_comps += current_comps;

        // 3. 기본 쉘 정렬 (n/2) 실행
        memcpy(working_copy, original_data, sizeof(int) * ARRAY_SIZE);
        shell_sort_basic(working_copy, ARRAY_SIZE, &current_comps);
        total_shell_basic_comps += current_comps;

        // 4. 최적 쉘 정렬 (Knuth) 실행
        memcpy(working_copy, original_data, sizeof(int) * ARRAY_SIZE);
        shell_sort_optimal(working_copy, ARRAY_SIZE, &current_comps);
        total_shell_optimal_comps += current_comps;

        if ((run + 1) % 10 == 0) {
            printf("... %d / %d 회 실행 완료\n", run + 1, NUM_RUNS);
        }
    }

    printf("\n--- 최종 결과: 100회 실행 평균 비교 횟수 ---\n");
    
    // 평균 계산 (double로 캐스팅하여 소수점 버림)
    printf("1. 단순 삽입 정렬:    %.0f 회\n", (double)total_insertion_comps / NUM_RUNS);
    printf("2. 기본 쉘 정렬 (n/2): %.0f 회\n", (double)total_shell_basic_comps / NUM_RUNS);
    printf("3. 최적 쉘 정렬 (Knuth): %.0f 회\n", (double)total_shell_optimal_comps / NUM_RUNS);

    return 0;
}
