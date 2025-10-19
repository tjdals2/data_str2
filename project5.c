#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 1000
#define SEARCH_SIZE 1000
#define MAX_VALUE 10001

typedef struct Node {
    int key;
    struct Node *left;
    struct Node *right;
    int height; // AVL 트리를 위한 높이 정보
} Node;

// --- 유틸리티 함수 ---
int max(int a, int b) {
    return (a > b) ? a : b;
}

// --- 노드 생성 및 높이 계산 ---
Node* newNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // 새 노드는 항상 높이 1로 시작
    return node;
}

int height(Node* n) {
    if (n == NULL)
        return 0;
    return n->height;
}

// 일반적인 배열 선형 탐색
long long linear_search(int arr[], int size, int key) {
    long long count = 0;
    for (int i = 0; i < size; i++) {
        count++;
        if (arr[i] == key) {
            return count;
        }
    }
    return count; // 찾지 못해도 전체 비교 횟수 반환
}

//이진 탐색 트리
Node* insert_bst(Node* node, int key) {
    if (node == NULL) return newNode(key);

    if (key < node->key)
        node->left = insert_bst(node->left, key);
    else if (key > node->key)
        node->right = insert_bst(node->right, key);
    
    return node;
}

long long search_tree(Node* root, int key) {
    long long count = 0;
    Node* current = root;
    while (current != NULL) {
        count++;
        if (key == current->key) {
            return count;
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return count; // 찾지 못해도 비교 횟수 반환
}


// 균형이진탐색트리
int getBalance(Node* n) {
    if (n == NULL)
        return 0;
    return height(n->left) - height(n->right);
} 
//밸런스펙터값을 return (왼쪽서브트리의높이)-(오른쪽서브트리의높이)
// 1: 왼쪽 서브트리가 한 칸 더 높음 (정상)
// 0: 양쪽 서브트리의 높이가 같음 (완벽한 균형)
// -1: 오른쪽 서브트리가 한 칸 더 높음 (정상)
// 2 (또는 그 이상): 왼쪽으로 너무 기울었음 (불균형 상태)
// -2 (또는 그 이하): 오른쪽으로 너무 기울었음 (불균형 상태)

Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}//왼쪽으로 기울어진 트리를 오른쪽으로 돌려 균형을 맞춰줌

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}//오른쪽으로 기울어진 트리를 왼쪽으로 돌려 균형을 맞춰줌

Node* insert_avl(Node* node, int key) {
    if (node == NULL)
        return newNode(key);

    if (key < node->key)
        node->left = insert_avl(node->left, key);
    else if (key > node->key)
        node->right = insert_avl(node->right, key);
    else // 중복 키는 허용하지 않음
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    // LL 회전
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // RR 회전
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // LR 회전
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // RL 회전
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// --- 데이터 생성 함수 ---
void generate_random(int value[]) {
    for (int i = 0; i < DATA_SIZE; i++) value[i] = rand() % MAX_VALUE;
}

void generate_sorted(int value[]) {
    for (int i = 0; i < DATA_SIZE; i++) value[i] = i;
}

void generate_reverse_sorted(int value[]) {
    for (int i = 0; i < DATA_SIZE; i++) value[i] = DATA_SIZE - 1 - i;
}

void generate_special(int value[]) {
    for (int i = 0; i < DATA_SIZE; i++) value[i] = i * (i % 2 + 2);
}

// --- 실험 진행 함수 ---
void run_test(const char* test_name, void (*generator)(int[])) {
    printf("--- [%s] ---\n", test_name);

    int initial_data[DATA_SIZE];
    int search_keys[SEARCH_SIZE];
    
    generator(initial_data);
    for (int i = 0; i < SEARCH_SIZE; i++) {
        search_keys[i] = initial_data[i];
    }

    // 자료구조 준비
    int arr[DATA_SIZE];
    Node* bst_root = NULL;
    Node* avl_root = NULL;

    for (int i = 0; i < DATA_SIZE; i++) {
        arr[i] = initial_data[i];
        bst_root = insert_bst(bst_root, initial_data[i]);
        avl_root = insert_avl(avl_root, initial_data[i]);
    }

    // 탐색 및 비교 횟수 계산
    long long total_arr_count = 0;
    long long total_bst_count = 0;
    long long total_avl_count = 0;

    for (int i = 0; i < SEARCH_SIZE; i++) {
        total_arr_count += linear_search(arr, DATA_SIZE, search_keys[i]);
        total_bst_count += search_tree(bst_root, search_keys[i]);
        total_avl_count += search_tree(avl_root, search_keys[i]);
    }

    // 평균 출력
    printf("배열 (선형 탐색) 평균 비교 횟수 : %.2f\n", (double)total_arr_count / SEARCH_SIZE);
    printf("이진 탐색 트리 (BST) 평균 비교 횟수 : %.2f\n", (double)total_bst_count / SEARCH_SIZE);
    printf("AVL 트리 평균 비교 횟수          : %.2f\n\n", (double)total_avl_count / SEARCH_SIZE);

    // TODO: 메모리 해제 (여기서는 생략)
}


int main() {
    srand(time(NULL));

    run_test("0~10000 사이의 무작위 정수 1000개", generate_random);
    run_test("0~999까지 정렬된 정수 1000개", generate_sorted);
    run_test("999~0까지 정렬된 정수 1000개", generate_reverse_sorted);
    run_test("for (int i = 0; i < 1000; i++) value[i] = i * (i % 2 + 2)로 구성된 정수 1000개", generate_special);

    return 0;
}
