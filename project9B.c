#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

// 반복 횟수 설정
#define REPS_SLOW 10    // 버블, 선택, 삽입 (속도가 느리므로 적게 반복)
#define REPS_FAST 1000  // 셸, 퀵, 힙, 병합, 기수, 트리 (속도가 빠르므로 많이 반복)

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
} Student;

// 비교 함수 포인터 타입 정의
typedef int (*CompareFunc)(const Student* a, const Student* b);

// --- [사용자 코드: 학생 데이터 로드] ---
Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("파일 열기 실패");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int capacity = 10;
    int count = 0;
    Student* arr = malloc(sizeof(Student) * capacity);

    if (!arr) {
        perror("메모리 할당 실패");
        fclose(fp);
        return NULL;
    }

    // 첫 줄(헤더) 스킵
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = realloc(arr, sizeof(Student) * capacity);
            if (!temp) {
                perror("메모리 재할당 실패");
                free(arr);
                fclose(fp);
                return NULL;
            }
            arr = temp;
        }

        Student s;
        char* token = strtok(line, ",");
        if (!token) continue;
        s.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.name, token, MAX_NAME_LEN);
        s.name[MAX_NAME_LEN - 1] = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        s.gender = token[0];

        token = strtok(NULL, ",");
        if (!token) continue;
        s.korean = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.english = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.math = atoi(token);

        arr[count++] = s;
    }

    fclose(fp);

    // 실제 사용된 크기에 맞춰 메모리 조정
    Student* tight = realloc(arr, sizeof(Student) * count);
    if (!tight) {
        *out_count = count;
        return arr;
    }
    arr = tight;
    *out_count = count;
    return arr;
}

// --- [비교 함수들] ---

// ID 기준
int cmp_id_asc(const Student* a, const Student* b) { return a->id - b->id; }
int cmp_id_desc(const Student* a, const Student* b) { return b->id - a->id; }

// 이름 기준
int cmp_name_asc(const Student* a, const Student* b) { return strcmp(a->name, b->name); }
int cmp_name_desc(const Student* a, const Student* b) { return strcmp(b->name, a->name); }

// 성별 기준
int cmp_gender_asc(const Student* a, const Student* b) { return a->gender - b->gender; }
int cmp_gender_desc(const Student* a, const Student* b) { return b->gender - a->gender; }

// 성적 합계 기준 (동점 시 국어->영어->수학 순)
int cmp_grade_total_asc(const Student* a, const Student* b) {
    int sumA = a->korean + a->english + a->math;
    int sumB = b->korean + b->english + b->math;
    if (sumA != sumB) return sumA - sumB;
    if (a->korean != b->korean) return a->korean - b->korean;
    if (a->english != b->english) return a->english - b->english;
    return a->math - b->math;
}
int cmp_grade_total_desc(const Student* a, const Student* b) {
    return cmp_grade_total_asc(b, a); // 역순 호출
}

// --- [정렬 알고리즘] ---

// 1. 버블 정렬 (Bubble Sort)
void bubble_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            (*comparisons)++; // 비교 횟수 증가
            if (cmp(&arr[j], &arr[j + 1]) > 0) {
                Student temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// 2. 선택 정렬 (Selection Sort)
void selection_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            (*comparisons)++;
            if (cmp(&arr[j], &arr[min_idx]) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Student temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}

// 3. 삽입 정렬 (Insertion Sort)
void insertion_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0) {
            (*comparisons)++;
            if (cmp(&arr[j], &key) > 0) {
                arr[j + 1] = arr[j];
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
    }
}

// 4. 셸 정렬 (Shell Sort) - Ciura 수열로 최적화
void shell_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    // Ciura 간격 수열 (실험적으로 가장 효율이 좋다고 알려짐)
    int gaps[] = {701, 301, 132, 57, 23, 10, 4, 1}; 
    int n_gaps = sizeof(gaps) / sizeof(gaps[0]);

    for (int g = 0; g < n_gaps; g++) {
        int gap = gaps[g];
        // 간격이 배열 크기보다 크면 건너뜀 (단, 1은 필수)
        if (gap >= n && gap != 1) continue; 

        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j;
            for (j = i; j >= gap; j -= gap) {
                (*comparisons)++;
                if (cmp(&arr[j - gap], &temp) > 0) {
                    arr[j] = arr[j - gap];
                } else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
}

// 5. 퀵 정렬 (Quick Sort) - 3값의 중앙값(Median-of-Three) 피벗 최적화
void swap_student(Student* a, Student* b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

// 보조 함수: 3값의 중앙값을 이용한 분할
int partition(Student* arr, int low, int high, CompareFunc cmp, long long* comparisons) {
    int mid = low + (high - low) / 2;
    
    // low, mid, high 위치의 값을 비교하여 중앙값을 찾고 정렬
    // 목표: arr[mid]가 중앙값이 되도록 만든 뒤, 피벗 위치(high)로 보냄
    
    if (cmp(&arr[mid], &arr[low]) < 0) {
        (*comparisons)++;
        swap_student(&arr[low], &arr[mid]);
    } else (*comparisons)++;

    if (cmp(&arr[high], &arr[low]) < 0) {
        (*comparisons)++;
        swap_student(&arr[low], &arr[high]);
    } else (*comparisons)++;

    if (cmp(&arr[high], &arr[mid]) < 0) {
        (*comparisons)++;
        swap_student(&arr[mid], &arr[high]);
    } else (*comparisons)++;

    // 이제 arr[high]는 3개 중 최댓값, arr[mid]는 중앙값임
    // arr[mid]를 피벗으로 사용하기 위해 high 위치로 이동
    swap_student(&arr[mid], &arr[high]); 
    
    Student pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        (*comparisons)++;
        if (cmp(&arr[j], &pivot) < 0) {
            i++;
            swap_student(&arr[i], &arr[j]);
        }
    }
    swap_student(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quick_sort_recursive(Student* arr, int low, int high, CompareFunc cmp, long long* comparisons) {
    while (low < high) {
        int pi = partition(arr, low, high, cmp, comparisons);

        // 최적화: 재귀 스택 깊이를 최소화하기 위해 더 작은 구간을 먼저 재귀 호출
        if (pi - low < high - pi) {
            quick_sort_recursive(arr, low, pi - 1, cmp, comparisons);
            low = pi + 1;
        } else {
            quick_sort_recursive(arr, pi + 1, high, cmp, comparisons);
            high = pi - 1;
        }
    }
}

void quick_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    quick_sort_recursive(arr, 0, n - 1, cmp, comparisons);
}

// 6. 힙 정렬 (Heap Sort)
void heapify(Student* arr, int n, int i, CompareFunc cmp, long long* comparisons) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n) {
        (*comparisons)++;
        if (cmp(&arr[left], &arr[largest]) > 0)
            largest = left;
    }

    if (right < n) {
        (*comparisons)++;
        if (cmp(&arr[right], &arr[largest]) > 0)
            largest = right;
    }

    if (largest != i) {
        Student temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        heapify(arr, n, largest, cmp, comparisons);
    }
}
void heap_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i, cmp, comparisons);
    for (int i = n - 1; i > 0; i--) {
        Student temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        heapify(arr, i, 0, cmp, comparisons);
    }
}

// 7. 병합 정렬 (Merge Sort)
void merge(Student* arr, int l, int m, int r, CompareFunc cmp, long long* comparisons) {
    int n1 = m - l + 1;
    int n2 = r - m;
    Student* L = malloc(sizeof(Student) * n1);
    Student* R = malloc(sizeof(Student) * n2);

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        (*comparisons)++;
        if (cmp(&L[i], &R[j]) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}
void merge_sort_recursive(Student* arr, int l, int r, CompareFunc cmp, long long* comparisons) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_recursive(arr, l, m, cmp, comparisons);
        merge_sort_recursive(arr, m + 1, r, cmp, comparisons);
        merge(arr, l, m, r, cmp, comparisons);
    }
}
void merge_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    merge_sort_recursive(arr, 0, n - 1, cmp, comparisons);
}

// 8. 트리 정렬 (Tree Sort) - AVL 트리로 최적화
typedef struct AVLNode {
    Student data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

int max_val(int a, int b) { return (a > b) ? a : b; }

int height(AVLNode* N) {
    if (N == NULL) return 0;
    return N->height;
}

AVLNode* new_avl_node(Student s) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = s;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// 오른쪽 회전 (Right Rotate)
AVLNode* right_rotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max_val(height(y->left), height(y->right)) + 1;
    x->height = max_val(height(x->left), height(x->right)) + 1;

    return x;
}

// 왼쪽 회전 (Left Rotate)
AVLNode* left_rotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max_val(height(x->left), height(x->right)) + 1;
    y->height = max_val(height(y->left), height(y->right)) + 1;

    return y;
}

int get_balance(AVLNode* N) {
    if (N == NULL) return 0;
    return height(N->left) - height(N->right);
}

AVLNode* insert_avl(AVLNode* node, Student s, CompareFunc cmp, long long* comparisons) {
    if (node == NULL) return new_avl_node(s);

    (*comparisons)++;
    if (cmp(&s, &node->data) < 0)
        node->left = insert_avl(node->left, s, cmp, comparisons);
    else if (cmp(&s, &node->data) > 0)
        node->right = insert_avl(node->right, s, cmp, comparisons);
    else {
        // 키가 같을 때: 일반적인 BST에서는 중복을 무시하거나 별도 처리함.
        // 여기서는 모든 레코드를 보존하기 위해 중복 데이터를 오른쪽 서브트리에 삽입하여 처리.
        node->right = insert_avl(node->right, s, cmp, comparisons);
    }

    node->height = 1 + max_val(height(node->left), height(node->right));

    int balance = get_balance(node);

    // Left Left 케이스
    if (balance > 1 && cmp(&s, &node->left->data) < 0) {
        (*comparisons)++; 
        return right_rotate(node);
    }
    // Right Right 케이스
    if (balance < -1 && cmp(&s, &node->right->data) > 0) {
        (*comparisons)++;
        return left_rotate(node);
    }
    // Left Right 케이스
    if (balance > 1 && cmp(&s, &node->left->data) > 0) {
        (*comparisons)++;
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    // Right Left 케이스
    if (balance < -1 && cmp(&s, &node->right->data) < 0) {
        (*comparisons)++;
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

void inorder_store_avl(AVLNode* root, Student* arr, int* i) {
    if (root != NULL) {
        inorder_store_avl(root->left, arr, i);
        arr[(*i)++] = root->data;
        inorder_store_avl(root->right, arr, i);
    }
}

void free_avl_tree(AVLNode* root) {
    if (root != NULL) {
        free_avl_tree(root->left);
        free_avl_tree(root->right);
        free(root);
    }
}

void tree_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    AVLNode* root = NULL;
    for (int i = 0; i < n; i++) {
        root = insert_avl(root, arr[i], cmp, comparisons);
    }
    int idx = 0;
    inorder_store_avl(root, arr, &idx);
    free_avl_tree(root);
}

// 9. 기수 정렬 (Radix Sort)
int get_radix_val(Student s, int mode) {
    if (mode == 0) return s.id;
    if (mode == 1) return s.korean + s.english + s.math;
    return 0;
}
void count_sort_radix(Student* arr, int n, int exp, int mode) {
    Student* output = malloc(sizeof(Student) * n);
    int count[10] = {0};

    for (int i = 0; i < n; i++)
        count[(get_radix_val(arr[i], mode) / exp) % 10]++;

    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    for (int i = n - 1; i >= 0; i--) {
        output[count[(get_radix_val(arr[i], mode) / exp) % 10] - 1] = arr[i];
        count[(get_radix_val(arr[i], mode) / exp) % 10]--;
    }

    for (int i = 0; i < n; i++)
        arr[i] = output[i];
    free(output);
}
void radix_sort(Student* arr, int n, int mode, int reverse) {
    int max = get_radix_val(arr[0], mode);
    for (int i = 1; i < n; i++) {
        int val = get_radix_val(arr[i], mode);
        if (val > max) max = val;
    }

    for (int exp = 1; max / exp > 0; exp *= 10)
        count_sort_radix(arr, n, exp, mode);

    if (reverse) {
        for (int i = 0; i < n / 2; i++) {
            Student t = arr[i];
            arr[i] = arr[n - 1 - i];
            arr[n - 1 - i] = t;
        }
    }
}

// --- [테스트 실행 환경] ---

const char* ALGO_NAMES[] = {
    "Bubble", "Selection", "Insertion", "Shell(Opt)", "Quick(Opt)", "Heap", "Merge", "Radix", "Tree(AVL)"
};
// 안정 정렬 여부? 1=예, 0=아니오
int IS_STABLE[] = { 1, 0, 1, 0, 0, 0, 1, 1, 0 }; 

typedef enum { SORT_BUBBLE, SORT_SELECT, SORT_INSERT, SORT_SHELL, SORT_QUICK, SORT_HEAP, SORT_MERGE, SORT_RADIX, SORT_TREE } AlgoType;

void run_test(const char* label, Student* original, int n, CompareFunc cmp, AlgoType algo, int is_desc, int radix_mode) {
    // 1. 필터: GENDER(성별) 기준은 안정 정렬만 수행
    if (strstr(label, "GENDER") && !IS_STABLE[algo]) return; 
    
    // 2. 필터: 힙과 트리는 중복 데이터(성별, 성적 등)가 많을 때 제외 (과제 요건)
    if ((strstr(label, "GENDER") || strstr(label, "GRADE")) && (algo == SORT_HEAP || algo == SORT_TREE)) {
        return;
    }
    
    // 3. 필터: 기수 정렬은 정수형(ID, 성적)만 가능
    if (algo == SORT_RADIX && (strstr(label, "NAME") || strstr(label, "GENDER"))) {
        return; 
    }

    int reps = (algo == SORT_BUBBLE || algo == SORT_SELECT || algo == SORT_INSERT) ? REPS_SLOW : REPS_FAST;
    long long total_compares = 0;
    
    // 메모리 사용량 추산
    long long mem_usage = 0;
    switch(algo) {
        case SORT_MERGE: mem_usage = sizeof(Student) * n; break;
        case SORT_RADIX: mem_usage = sizeof(Student) * n; break;
        case SORT_TREE: mem_usage = (sizeof(Student) + 2 * sizeof(void*) + sizeof(int)) * n; break; // +높이
        case SORT_QUICK: mem_usage = sizeof(void*) * 64; // 스택 깊이
        default: mem_usage = 0; break; 
    }

    for (int r = 0; r < reps; r++) {
        Student* data = malloc(sizeof(Student) * n);
        memcpy(data, original, sizeof(Student) * n);
        
        long long compares = 0;

        switch (algo) {
            case SORT_BUBBLE: bubble_sort(data, n, cmp, &compares); break;
            case SORT_SELECT: selection_sort(data, n, cmp, &compares); break;
            case SORT_INSERT: insertion_sort(data, n, cmp, &compares); break;
            case SORT_SHELL: shell_sort(data, n, cmp, &compares); break;
            case SORT_QUICK: quick_sort(data, n, cmp, &compares); break;
            case SORT_HEAP: heap_sort(data, n, cmp, &compares); break;
            case SORT_MERGE: merge_sort(data, n, cmp, &compares); break;
            case SORT_TREE: tree_sort(data, n, cmp, &compares); break;
            case SORT_RADIX: radix_sort(data, n, radix_mode, is_desc); break; 
        }
        total_compares += compares;
        free(data);
    }

    printf("[%-10s] %-20s | 평균 비교 횟수: %10lld | 메모리: %8lld bytes\n", 
           ALGO_NAMES[algo], label, total_compares / reps, mem_usage);
}

int main() {
    int count = 0;
    const char* filename = "dataset_id_ascending.csv";
    Student* students = load_students(filename, &count);

    if (!students) {
        printf("학생 데이터를 불러오는데 실패했습니다.\n");
        return 1;
    }

    printf("학생 %d명의 데이터를 불러왔습니다.\n", count);
    printf("최적화된 정렬 알고리즘 실행 중 (비교 횟수 최소화)...\n");
    printf("--------------------------------------------------------------------------------\n");

    // 1. ID 기준
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("ID 오름차순", students, count, cmp_id_asc, a, 0, 0);
        run_test("ID 내림차순", students, count, cmp_id_desc, a, 1, 0);
    }
    printf("--------------------------------------------------------------------------------\n");

    // 2. NAME 기준
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("NAME 오름차순", students, count, cmp_name_asc, a, 0, -1);
        run_test("NAME 내림차순", students, count, cmp_name_desc, a, 1, -1);
    }
    printf("--------------------------------------------------------------------------------\n");

    // 3. GENDER 기준 (Stable 정렬만)
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("GENDER 오름차순", students, count, cmp_gender_asc, a, 0, -1);
        run_test("GENDER 내림차순", students, count, cmp_gender_desc, a, 1, -1);
    }
    printf("--------------------------------------------------------------------------------\n");

    // 4. GRADE 합계 기준
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("GRADE 오름차순", students, count, cmp_grade_total_asc, a, 0, 1);
        run_test("GRADE 내림차순", students, count, cmp_grade_total_desc, a, 1, 1);
    }

    free(students);
    return 0;
}
