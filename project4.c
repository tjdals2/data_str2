#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 100
#define MAX_NUM 1000

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

// 새로운 노드를 생성하는 함수
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("메모리 할당 오류\n");
        exit(1);
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 이진 탐색 트리에 데이터를 삽입하는 함수 (재귀 방식)
Node* insert(Node* root, int data) {
    if (root == NULL) {
        return createNode(data);
    }
    if (data < root->data) {
        root->left = insert(root->left, data);
    } else {
        root->right = insert(root->right, data);
    }
    return root;
}

// 배열 선형 탐색 함수
int linearSearch(int arr[], int size, int key, int* comparisons) {
    *comparisons = 0;
    for (int i = 0; i < size; i++) {
        (*comparisons)++;
        if (arr[i] == key) {
            return i;
        }
    }
    return -1;
}

// 이진 탐색 트리 검색 함수
Node* bstSearch(Node* root, int key, int* comparisons) {
    *comparisons = 0;
    Node* current = root;
    while (current != NULL) {
        (*comparisons)++;
        if (key == current->data) {
            return current;
        }
        if (key < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return NULL;
}

// 트리 메모리 해제 함수 (재귀 방식)
void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}


int main() {
    srand(time(NULL));

    int arr[ARRAY_SIZE];
    Node* root = NULL;

    printf("## 0~1000 사이의 무작위 숫자 100개를 생성\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int num = rand() % (MAX_NUM + 1);
        arr[i] = num;
        root = insert(root, num);
        printf("%d ", num);
        if ((i + 1) % 20 == 0) {
            printf("\n");
        }
    }

    int key_to_find;
    printf("탐색할 숫자를 입력하세요: ");
    
    if (scanf(" %d", &key_to_find) != 1) {
        printf("\n[오류]\n");
        freeTree(root); // 오류 발생 시에도 메모리 해제
        return 1;
    }

    // 변수 초기화
    int ls_comparisons, bst_comparisons;

    // 3. 선형 탐색 실행 및 결과 출력
    printf("\n--- 1. 배열 선형 탐색 결과 ---\n");
    int found_index = linearSearch(arr, ARRAY_SIZE, key_to_find, &ls_comparisons);
    if (found_index != -1) {
        printf("결과: 숫자 %d를(을) 인덱스 %d에서 찾았습니다.\n", key_to_find, found_index);
    } else {
        printf("결과: 숫자 %d를(을) 찾지 못했습니다.\n", key_to_find);
    }
    printf("총 비교 횟수: %d회\n", ls_comparisons);

    // 4. 이진 탐색 트리 검색 실행 및 결과 출력
    printf("\n--- 2. 이진 탐색 트리 검색 결과 ---\n");
    Node* found_node = bstSearch(root, key_to_find, &bst_comparisons);
    if (found_node != NULL) {
        printf("결과: 숫자 %d를(을) 찾았습니다.\n", key_to_find);
    } else {
        printf("결과: 숫자 %d를(을) 찾지 못했습니다.\n", key_to_find);
    }
    printf("총 비교 횟수: %d회\n", bst_comparisons);

    // 5. 결과 비교
    printf("비교 결과: ");
    if (ls_comparisons < bst_comparisons) {
        printf("선형 탐색이 %d회 더 적은 비교로 빨랐습니다.\n", bst_comparisons - ls_comparisons);
    } else if (bst_comparisons < ls_comparisons) {
        printf("이진 탐색 트리가 %d회 더 적은 비교로 빨랐습니다.\n\n", ls_comparisons - bst_comparisons);
    } else {
        printf("두 탐색의 비교 횟수가 동일했습니다.\n");
    }

    // 할당된 트리 메모리 해제
    freeTree(root);
    return 0;
}
