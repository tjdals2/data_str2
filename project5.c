#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ELEMENTS 1000
#define MAX_VAL 10000

typedef struct Node {
    int key;
    struct Node *left;
    struct Node *right;
    int height; 
} Node;

//유틸리티함수
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int height(Node *n) {
    if (n == NULL) return 0;
    return n->height;
}

Node* newNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // 새 노드는 항상 높이 1
    return node;
}

void free_tree(Node* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

// (1) 0~10000 사이의 무작위 정수 1000개 (중복 X)
void generate_data_1(int arr[]) {
    int used[MAX_VAL + 1] = {0};
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        int r;
        do {
            r = rand() % (MAX_VAL + 1);
        } while (used[r]);
        arr[i] = r;
        used[r] = 1;
    }
}

// (2) 0~999까지 정렬된 정수 1000개
void generate_data_2(int arr[]) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        arr[i] = i;
    }
}

// (3) 999~0까지 정렬된 정수 1000개
void generate_data_3(int arr[]) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        arr[i] = NUM_ELEMENTS - 1 - i;
    }
}

// (4) i * (i % 2 + 2)로 구성된 정수 1000개
void generate_data_4(int arr[]) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        arr[i] = i * (i % 2 + 2);
    }
}

// 탐색에 사용할 난수 1000개 생성
void generate_search_keys(int arr[]) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        arr[i] = rand() % (MAX_VAL + 1);
    }
}


//배열 선형탐색
void linear_search(int arr[], int key, int* comparisons) {
    *comparisons = 0;
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        (*comparisons)++;
        if (arr[i] == key) {
            return;
        }
    }
}

//이진탐색트리 BST
Node* insert_bst(Node* node, int key) {
    if (node == NULL) return newNode(key);
    if (key < node->key) {
        node->left = insert_bst(node->left, key);
    } else if (key > node->key) {
        node->right = insert_bst(node->right, key);
    }
    return node;
}

void search_bst(Node* root, int key, int* comparisons) {
    *comparisons = 0;
    Node* current = root;
    while (current != NULL) {
        (*comparisons)++;
        if (key == current->key) {
            return;
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
}


//AVL트리
Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(Node* n) {
    if (n == NULL) return 0;
    return height(n->left) - height(n->right);
}

Node* insert_avl(Node* node, int key) {
    if (node == NULL) return newNode(key);

    if (key < node->key) {
        node->left = insert_avl(node->left, key);
    } else if (key > node->key) {
        node->right = insert_avl(node->right, key);
    } else {
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // LL Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);
    // RR Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);
    // LR Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // RL Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

#define search_avl search_bst

int main() {
    srand((unsigned int)time(NULL));

    for (int i = 1; i <= 4; i++) {
        printf("--- 데이터 (%d) 결과 ---\n", i);

        int data[NUM_ELEMENTS];
        int search_keys[NUM_ELEMENTS];
        
        // 1. 데이터 생성
        switch (i) {
            case 1: generate_data_1(data); break;
            case 2: generate_data_2(data); break;
            case 3: generate_data_3(data); break;
            case 4: generate_data_4(data); break;
        }
        
        // 2. 자료구조에 데이터 삽입
        Node* bst_root = NULL;
        Node* avl_root = NULL;
        for (int j = 0; j < NUM_ELEMENTS; j++) {
            bst_root = insert_bst(bst_root, data[j]);
            avl_root = insert_avl(avl_root, data[j]);
        }
        
        // 3. 탐색할 키 생성 및 탐색 수행
        generate_search_keys(search_keys);
        
        long long array_total_comp = 0;
        long long bst_total_comp = 0;
        long long avl_total_comp = 0;
        
        for (int j = 0; j < NUM_ELEMENTS; j++) {
            int comparisons;
            
            // 배열 탐색
            linear_search(data, search_keys[j], &comparisons);
            array_total_comp += comparisons;
            
            // BST 탐색
            search_bst(bst_root, search_keys[j], &comparisons);
            bst_total_comp += comparisons;
            
            // AVL 탐색
            search_avl(avl_root, search_keys[j], &comparisons);
            avl_total_comp += comparisons;
        }
        
        // 4. 결과 출력
        printf("Array: 데이터 (%d)에서 평균 %.2f회 탐색\n", i, (double)array_total_comp / NUM_ELEMENTS);
        printf("BST:   데이터 (%d)에서 평균 %.2f회 탐색\n", i, (double)bst_total_comp / NUM_ELEMENTS);
        printf("AVL:   데이터 (%d)에서 평균 %.2f회 탐색\n", i, (double)avl_total_comp / NUM_ELEMENTS);
        printf("\n");

        free_tree(bst_root);
        free_tree(avl_root);
    }
    
    return 0;
}
