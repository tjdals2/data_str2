#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 33000
#define NAME_LEN 64
#define NUM_TRIALS 40

// =====================================================
// 데이터 구조 정의
// =====================================================
typedef struct {
    int id;
    char name[NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
    int mul_of_score; // Key
} Student;

// AVL 트리 노드 정의
typedef struct AVLNode {
    Student data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

// =====================================================
// 전역 변수
// =====================================================
long long g_sort_comp_count = 0;
int last_id = 0;

Student students[MAX_STUDENTS];               // A. 정렬되지 않은 배열 (순차)
Student sorted_students[MAX_STUDENTS];        // B. 정렬된 배열 (이진)
Student sorted_students_interp[MAX_STUDENTS]; // D. 정렬된 배열 (보간)
AVLNode* root = NULL;                         // C. AVL 트리

// =====================================================
// 1. 공통 헬퍼 함수
// =====================================================
int max_int(int a, int b) {
    return (a > b) ? a : b;
}

Student create_random_student(int key) {
    last_id++;
    Student s;
    s.id = last_id;
    s.gender = (rand() % 2 == 0) ? 'F' : 'M';
    s.korean = rand() % 101;
    s.english = rand() % 101;
    s.math = rand() % 101;
    s.mul_of_score = key;
    strcpy(s.name, "RANDOM");
    return s;
}

// =====================================================
// 2. AVL 트리 구현부
// =====================================================
int get_height(AVLNode* n) {
    if (n == NULL) return 0;
    return n->height;
}

int get_balance(AVLNode* n) {
    if (n == NULL) return 0;
    return get_height(n->left) - get_height(n->right);
}

AVLNode* create_node(Student data) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

AVLNode* right_rotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max_int(get_height(y->left), get_height(y->right)) + 1;
    x->height = max_int(get_height(x->left), get_height(x->right)) + 1;
    return x;
}

AVLNode* left_rotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max_int(get_height(x->left), get_height(x->right)) + 1;
    y->height = max_int(get_height(y->left), get_height(y->right)) + 1;
    return y;
}

AVLNode* avl_search(AVLNode* root, int key, int* compCount) {
    if (root == NULL) return NULL;
    (*compCount)++;
    if (key == root->data.mul_of_score) return root;
    if (key < root->data.mul_of_score) return avl_search(root->left, key, compCount);
    else return avl_search(root->right, key, compCount);
}

AVLNode* avl_insert(AVLNode* node, Student data, int* compCount, int* success) {
    if (node == NULL) { *success = 1; return create_node(data); }
    (*compCount)++;
    if (data.mul_of_score < node->data.mul_of_score) node->left = avl_insert(node->left, data, compCount, success);
    else if (data.mul_of_score > node->data.mul_of_score) node->right = avl_insert(node->right, data, compCount, success);
    else { *success = 0; return node; }

    node->height = 1 + max_int(get_height(node->left), get_height(node->right));
    int balance = get_balance(node);

    if (balance > 1 && data.mul_of_score < node->left->data.mul_of_score) return right_rotate(node);
    if (balance < -1 && data.mul_of_score > node->right->data.mul_of_score) return left_rotate(node);
    if (balance > 1 && data.mul_of_score > node->left->data.mul_of_score) { node->left = left_rotate(node->left); return right_rotate(node); }
    if (balance < -1 && data.mul_of_score < node->right->data.mul_of_score) { node->right = right_rotate(node->right); return left_rotate(node); }
    return node;
}

AVLNode* min_value_node(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != NULL) current = current->left;
    return current;
}

AVLNode* avl_delete(AVLNode* root, int key, int* compCount, int* success) {
    if (root == NULL) { *success = 0; return root; }
    (*compCount)++;
    if (key < root->data.mul_of_score) root->left = avl_delete(root->left, key, compCount, success);
    else if (key > root->data.mul_of_score) root->right = avl_delete(root->right, key, compCount, success);
    else {
        *success = 1;
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (temp == NULL) { temp = root; root = NULL; }
            else *root = *temp;
            free(temp);
        }
        else {
            AVLNode* temp = min_value_node(root->right);
            root->data = temp->data;
            root->right = avl_delete(root->right, temp->data.mul_of_score, compCount, success);
        }
    }
    if (root == NULL) return root;
    root->height = 1 + max_int(get_height(root->left), get_height(root->right));
    int balance = get_balance(root);
    if (balance > 1 && get_balance(root->left) >= 0) return right_rotate(root);
    if (balance > 1 && get_balance(root->left) < 0) { root->left = left_rotate(root->left); return right_rotate(root); }
    if (balance < -1 && get_balance(root->right) <= 0) return left_rotate(root);
    if (balance < -1 && get_balance(root->right) > 0) { root->right = right_rotate(root->right); return left_rotate(root); }
    return root;
}

// =====================================================
// 3. 탐색 알고리즘 (순차, 이진, 보간)
// =====================================================
int sequential_search(Student arr[], int n, int key, int* compCount) {
    *compCount = 0;
    for (int i = 0; i < n; i++) {
        (*compCount)++;
        if (arr[i].mul_of_score == key) return i;
    }
    return -1;
}

int binary_search(Student arr[], int n, int key, int* compCount) {
    int low = 0, high = n - 1;
    *compCount = 0;
    while (low <= high) {
        int mid = (low + high) / 2;
        (*compCount)++;
        if (arr[mid].mul_of_score == key) return mid;
        else if (key < arr[mid].mul_of_score) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}

int interpolation_search(Student arr[], int n, int key, int* compCount) {
    int low = 0;
    int high = n - 1;
    *compCount = 0;

    while (low <= high && key >= arr[low].mul_of_score && key <= arr[high].mul_of_score) {
        if (low == high) {
            (*compCount)++;
            if (arr[low].mul_of_score == key) return low;
            return -1;
        }
        double ratio = (double)(key - arr[low].mul_of_score) / (arr[high].mul_of_score - arr[low].mul_of_score);
        int pos = low + (int)(ratio * (high - low));

        (*compCount)++;
        if (arr[pos].mul_of_score == key) return pos;
        if (arr[pos].mul_of_score < key) low = pos + 1;
        else high = pos - 1;
    }
    return -1;
}

// =====================================================
// 4. 삽입/삭제 헬퍼
// =====================================================
int compare_by_mul(const void* a, const void* b) {
    const Student* s1 = (const Student*)a;
    const Student* s2 = (const Student*)b;
    g_sort_comp_count++;
    if (s1->mul_of_score < s2->mul_of_score) return -1;
    else if (s1->mul_of_score > s2->mul_of_score) return 1;
    return 0;
}

int unsorted_insert(Student arr[], int* n, int key, int* compCount) {
    if (sequential_search(arr, *n, key, compCount) != -1) return 0;
    if (*n >= MAX_STUDENTS) return 0;
    arr[*n] = create_random_student(key);
    (*n)++;
    return 1;
}

int unsorted_delete(Student arr[], int* n, int key, int* compCount) {
    int idx = sequential_search(arr, *n, key, compCount);
    if (idx == -1) return 0;
    arr[idx] = arr[*n - 1];
    (*n)--;
    return 1;
}

int lower_bound(Student arr[], int n, int key, int* compCount) {
    int low = 0, high = n;
    *compCount = 0;
    while (low < high) {
        int mid = (low + high) / 2;
        (*compCount)++;
        if (arr[mid].mul_of_score < key) low = mid + 1;
        else high = mid;
    }
    return low;
}

int sorted_insert(Student arr[], int* n, int key, int* compCount) {
    if (*n >= MAX_STUDENTS) return 0;
    int pos = lower_bound(arr, *n, key, compCount);
    if (pos < *n) {
        (*compCount)++;
        if (arr[pos].mul_of_score == key) return 0;
    }
    int moves = (*n - pos);
    for (int i = *n; i > pos; i--) arr[i] = arr[i - 1];
    *compCount += moves;
    Student s = create_random_student(key);
    s.id = 0; strcpy(s.name, "DUMMY");
    arr[pos] = s;
    (*n)++;
    return 1;
}

int sorted_delete(Student arr[], int* n, int key, int* compCount) {
    int idx = binary_search(arr, *n, key, compCount);
    if (idx == -1) return 0;
    for (int i = idx; i < *n - 1; i++) arr[i] = arr[i + 1];
    (*n)--;
    return 1;
}

int sorted_delete_interp(Student arr[], int* n, int key, int* compCount) {
    int idx = interpolation_search(arr, *n, key, compCount);
    if (idx == -1) return 0;
    for (int i = idx; i < *n - 1; i++) arr[i] = arr[i + 1];
    (*n)--;
    return 1;
}

// =====================================================
// 5. 메인 함수
// =====================================================
int parse_student_line(char* line, Student* stu) {
    char* token = strtok(line, ",\n"); if (!token) return 0;
    stu->id = atoi(token); last_id = max_int(last_id, stu->id);
    token = strtok(NULL, ",\n"); if (!token) return 0; strcpy(stu->name, token);
    token = strtok(NULL, ",\n"); if (!token) return 0; stu->gender = token[0];
    token = strtok(NULL, ",\n"); if (!token) return 0; stu->korean = atoi(token);
    token = strtok(NULL, ",\n"); if (!token) return 0; stu->english = atoi(token);
    token = strtok(NULL, ",\n"); if (!token) return 0; stu->math = atoi(token);
    stu->mul_of_score = stu->korean * stu->english * stu->math;
    return 1;
}

int main(void) {
    FILE* fp = fopen("students.csv", "r");
    if (!fp) { fprintf(stderr, "students.csv 파일이 없습니다.\n"); return 1; }

    char line[256];
    int n = 0;
    fgets(line, sizeof(line), fp);
    while (fgets(line, sizeof(line), fp)) {
        if (n >= MAX_STUDENTS) break;
        if (parse_student_line(line, &students[n])) n++;
    }
    fclose(fp);
    printf("데이터 로드 완료: %d명\n\n", n);

    // B, D 실험용 초기화
    int n_uns = n;
    int n_sort = n;
    int n_sort_interp = n;

    memcpy(sorted_students, students, sizeof(Student) * n);
    qsort(sorted_students, n, sizeof(Student), compare_by_mul);

    memcpy(sorted_students_interp, students, sizeof(Student) * n);
    qsort(sorted_students_interp, n, sizeof(Student), compare_by_mul);

    // C 실험용 AVL 구축
    printf("AVL 트리 구축 중...\n");
    int dummy_comp = 0, dummy_suc = 0;
    for (int i = 0; i < n; i++) {
        root = avl_insert(root, students[i], &dummy_comp, &dummy_suc);
    }
    printf("AVL 트리 구축 완료. 높이: %d\n\n", get_height(root));

    // 키 생성
    srand((unsigned)time(NULL));
    int keys[NUM_TRIALS];
    for (int i = 0; i < NUM_TRIALS; i++) keys[i] = rand() % 1000001;

    // ----------------------------------------------------------------
    // A. 정렬되지 않은 배열 (순차)
    // ----------------------------------------------------------------
    printf("A) 정렬되지 않은 배열 + 순차 탐색\n");
    printf("----------------------------------------------------\n");
    long long total_A = 0;
    for (int t = 0; t < NUM_TRIALS; t++) {
        int key = keys[t]; int op = key % 3; int comp = 0, ok = 0;
        if (op == 0) ok = unsorted_insert(students, &n_uns, key, &comp);
        else if (op == 1) ok = unsorted_delete(students, &n_uns, key, &comp);
        else { sequential_search(students, n_uns, key, &comp); ok = 1; }
        total_A += comp;
        // 주석 해제됨!
        printf("%4d | %6d | %s | %8d | %s\n", t+1, key, (op==0?"삽입":(op==1?"삭제":"검색")), comp, ok?"성공":"실패");
    }
    printf(">> 평균 비교 횟수: %.2f\n\n", (double)total_A / NUM_TRIALS);

    // ----------------------------------------------------------------
    // B. 정렬된 배열 (이진)
    // ----------------------------------------------------------------
    printf("B) 정렬된 배열 + 이진 탐색\n");
    printf("----------------------------------------------------\n");
    long long total_B = 0;
    for (int t = 0; t < NUM_TRIALS; t++) {
        int key = keys[t]; int op = key % 3; int comp = 0, ok = 0;
        if (op == 0) ok = sorted_insert(sorted_students, &n_sort, key, &comp);
        else if (op == 1) ok = sorted_delete(sorted_students, &n_sort, key, &comp);
        else { binary_search(sorted_students, n_sort, key, &comp); ok = 1; }
        total_B += comp;
        printf("%4d | %6d | %s | %8d | %s\n", t+1, key, (op==0?"삽입":(op==1?"삭제":"검색")), comp, ok?"성공":"실패");
    }
    printf(">> 평균 비교 횟수: %.2f\n\n", (double)total_B / NUM_TRIALS);

    // ----------------------------------------------------------------
    // D. 정렬된 배열 (보간)
    // ----------------------------------------------------------------
    printf("C) 정렬된 배열 + 보간 탐색\n");
    printf("----------------------------------------------------\n");
    long long total_D = 0;
    for (int t = 0; t < NUM_TRIALS; t++) {
        int key = keys[t]; int op = key % 3; int comp = 0, ok = 0;
        if (op == 0) ok = sorted_insert(sorted_students_interp, &n_sort_interp, key, &comp);
        else if (op == 1) ok = sorted_delete_interp(sorted_students_interp, &n_sort_interp, key, &comp);
        else { interpolation_search(sorted_students_interp, n_sort_interp, key, &comp); ok = 1; }
        total_D += comp;
        printf("%4d | %6d | %s | %8d | %s\n", t+1, key, (op==0?"삽입":(op==1?"삭제":"검색")), comp, ok?"성공":"실패");
    }
    printf(">> 평균 비교 횟수: %.2f\n\n", (double)total_D / NUM_TRIALS);

    // ----------------------------------------------------------------
    // C. AVL 트리
    // ----------------------------------------------------------------
    printf("D) AVL 트리\n");
    printf("----------------------------------------------------\n");
    long long total_C = 0;
    for (int t = 0; t < NUM_TRIALS; t++) {
        int key = keys[t]; int op = key % 3; int comp = 0, ok = 0;
        if (op == 0) { Student newS = create_random_student(key); root = avl_insert(root, newS, &comp, &ok); }
        else if (op == 1) root = avl_delete(root, key, &comp, &ok);
        else { ok = (avl_search(root, key, &comp) != NULL); }
        total_C += comp;
        // 주석 해제됨!
        printf("%4d | %6d | %s | %8d | %s\n", t+1, key, (op==0?"삽입":(op==1?"삭제":"검색")), comp, ok?"성공":"실패");
    }
    printf(">> 평균 비교 횟수: %.2f\n", (double)total_C / NUM_TRIALS);

    return 0;
}
