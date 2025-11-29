#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

// 반복 횟수 설정
#define REPS_SLOW 10    // 버블, 선택, 삽입
#define REPS_FAST 1000  // 셸, 퀵, 힙, 병합, 기수, 트리

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

// --- [User Code: Load Students] ---
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

    if (!arr) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    // 첫 줄 헤더 스킵
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = realloc(arr, sizeof(Student) * capacity);
            if (!temp) {
                perror("Reallocation failed");
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

    Student* tight = realloc(arr, sizeof(Student) * count);
    if (!tight) {
        *out_count = count;
        return arr;
    }
    arr = tight;
    *out_count = count;
    return arr;
}

// --- [Comparator Functions] ---

// ID
int cmp_id_asc(const Student* a, const Student* b) { return a->id - b->id; }
int cmp_id_desc(const Student* a, const Student* b) { return b->id - a->id; }

// Name
int cmp_name_asc(const Student* a, const Student* b) { return strcmp(a->name, b->name); }
int cmp_name_desc(const Student* a, const Student* b) { return strcmp(b->name, a->name); }

// Gender
int cmp_gender_asc(const Student* a, const Student* b) { return a->gender - b->gender; }
int cmp_gender_desc(const Student* a, const Student* b) { return b->gender - a->gender; }

// Grade (Sum -> Kor -> Eng -> Math)
int cmp_grade_total_asc(const Student* a, const Student* b) {
    int sumA = a->korean + a->english + a->math;
    int sumB = b->korean + b->english + b->math;
    if (sumA != sumB) return sumA - sumB;
    if (a->korean != b->korean) return a->korean - b->korean;
    if (a->english != b->english) return a->english - b->english;
    return a->math - b->math;
}
int cmp_grade_total_desc(const Student* a, const Student* b) {
    return cmp_grade_total_asc(b, a); // Reverse
}

// --- [Sorting Algorithms] ---

// 1. Bubble Sort
void bubble_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            (*comparisons)++;
            if (cmp(&arr[j], &arr[j + 1]) > 0) {
                Student temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// 2. Selection Sort
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

// 3. Insertion Sort
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

// 4. Shell Sort
void shell_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
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

// 5. Quick Sort
void quick_sort_recursive(Student* arr, int low, int high, CompareFunc cmp, long long* comparisons) {
    if (low < high) {
        Student pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++) {
            (*comparisons)++;
            if (cmp(&arr[j], &pivot) < 0) {
                i++;
                Student temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
        Student temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;
        int pi = i + 1;

        quick_sort_recursive(arr, low, pi - 1, cmp, comparisons);
        quick_sort_recursive(arr, pi + 1, high, cmp, comparisons);
    }
}
void quick_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    quick_sort_recursive(arr, 0, n - 1, cmp, comparisons);
}

// 6. Heap Sort
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

// 7. Merge Sort
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

// 8. Tree Sort (BST)
typedef struct TreeNode {
    Student data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* new_node(Student s) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->data = s;
    node->left = node->right = NULL;
    return node;
}
void insert_tree(TreeNode** node, Student s, CompareFunc cmp, long long* comparisons) {
    if (*node == NULL) {
        *node = new_node(s);
        return;
    }
    (*comparisons)++;
    if (cmp(&s, &(*node)->data) < 0)
        insert_tree(&(*node)->left, s, cmp, comparisons);
    else
        insert_tree(&(*node)->right, s, cmp, comparisons);
}
void inorder_store(TreeNode* root, Student* arr, int* i) {
    if (root != NULL) {
        inorder_store(root->left, arr, i);
        arr[(*i)++] = root->data;
        inorder_store(root->right, arr, i);
    }
}
void free_tree(TreeNode* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}
void tree_sort(Student* arr, int n, CompareFunc cmp, long long* comparisons) {
    TreeNode* root = NULL;
    for (int i = 0; i < n; i++) {
        insert_tree(&root, arr[i], cmp, comparisons);
    }
    int idx = 0;
    inorder_store(root, arr, &idx);
    free_tree(root);
}

// 9. Radix Sort (Implemented for ID and Grades only, LSD)
// Helper for Radix: Get value based on criteria type
// Mode: 0 = ID, 1 = Grade Sum
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
    // mode: 0=ID, 1=Grade. Radix doesn't compare, so comparisons = 0.
    int max = get_radix_val(arr[0], mode);
    for (int i = 1; i < n; i++) {
        int val = get_radix_val(arr[i], mode);
        if (val > max) max = val;
    }

    for (int exp = 1; max / exp > 0; exp *= 10)
        count_sort_radix(arr, n, exp, mode);

    // If descending, reverse the array
    if (reverse) {
        for (int i = 0; i < n / 2; i++) {
            Student t = arr[i];
            arr[i] = arr[n - 1 - i];
            arr[n - 1 - i] = t;
        }
    }
}

// --- [Test Harness] ---

const char* ALGO_NAMES[] = {
    "Bubble", "Selection", "Insertion", "Shell", "Quick", "Heap", "Merge", "Radix", "Tree"
};
// Is Stable? 1=Yes, 0=No
int IS_STABLE[] = { 1, 0, 1, 0, 0, 0, 1, 1, 0 }; 

typedef enum { SORT_BUBBLE, SORT_SELECT, SORT_INSERT, SORT_SHELL, SORT_QUICK, SORT_HEAP, SORT_MERGE, SORT_RADIX, SORT_TREE } AlgoType;

void run_test(const char* label, Student* original, int n, CompareFunc cmp, AlgoType algo, int is_desc, int radix_mode) {
    // 1. Filter: Gender requires Stable Sort
    if (strstr(label, "GENDER") && !IS_STABLE[algo]) {
        // Skip unstable sort for Gender
        return; 
    }
    // 2. Filter: Heap/Tree skip if duplicates (Gender, Grade)
    // Assuming ID and Name are mostly unique, Gender/Grade have duplicates.
    if ((strstr(label, "GENDER") || strstr(label, "GRADE")) && (algo == SORT_HEAP || algo == SORT_TREE)) {
        return;
    }
    // 3. Filter: Radix only for ID and Grade
    if (algo == SORT_RADIX && (strstr(label, "NAME") || strstr(label, "GENDER"))) {
        return; 
    }

    int reps = (algo == SORT_BUBBLE || algo == SORT_SELECT || algo == SORT_INSERT) ? REPS_SLOW : REPS_FAST;
    long long total_compares = 0;
    
    // Memory Usage Calculation (Approximate Auxiliary)
    long long mem_usage = 0;
    switch(algo) {
        case SORT_MERGE: mem_usage = sizeof(Student) * n; break;
        case SORT_RADIX: mem_usage = sizeof(Student) * n; break;
        case SORT_TREE: mem_usage = (sizeof(Student) + 2 * sizeof(void*)) * n; break; 
        case SORT_QUICK: mem_usage = sizeof(void*) * 64; // Stack depth approx
        default: mem_usage = 0; break; // In-place sorts (Bubble, Select, Insert, Shell, Heap) O(1)
    }

    for (int r = 0; r < reps; r++) {
        // Copy data
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
            case SORT_RADIX: radix_sort(data, n, radix_mode, is_desc); break; // No compare count
        }
        total_compares += compares;
        free(data);
    }

    printf("[%-9s] %-20s | Avg Compares: %10lld | Mem: %8lld bytes\n", 
           ALGO_NAMES[algo], label, total_compares / reps, mem_usage);
}

int main() {
    int count = 0;
    // 파일명 설정 (사용자가 업로드한 파일명)
    const char* filename = "dataset_id_ascending.csv";
    Student* students = load_students(filename, &count);

    if (!students) {
        printf("Failed to load students.\n");
        return 1;
    }

    printf("Loaded %d students.\n", count);
    printf("Running Sort Algorithms...\n");
    printf("--------------------------------------------------------------------------------\n");

    // Scenarios
    // 1. ID
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("ID Ascending", students, count, cmp_id_asc, a, 0, 0);
        run_test("ID Descending", students, count, cmp_id_desc, a, 1, 0);
    }
    printf("--------------------------------------------------------------------------------\n");

    // 2. NAME
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("NAME Ascending", students, count, cmp_name_asc, a, 0, -1);
        run_test("NAME Descending", students, count, cmp_name_desc, a, 1, -1);
    }
    printf("--------------------------------------------------------------------------------\n");

    // 3. GENDER (Only Stable)
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("GENDER Ascending", students, count, cmp_gender_asc, a, 0, -1);
        run_test("GENDER Descending", students, count, cmp_gender_desc, a, 1, -1);
    }
    printf("--------------------------------------------------------------------------------\n");

    // 4. GRADE (Sum)
    for (int a = 0; a <= SORT_TREE; a++) {
        run_test("GRADE Ascending", students, count, cmp_grade_total_asc, a, 0, 1);
        run_test("GRADE Descending", students, count, cmp_grade_total_desc, a, 1, 1);
    }

    free(students);
    return 0;
}
