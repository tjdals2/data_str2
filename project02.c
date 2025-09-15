#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> 

#define error 0
#define true 1
#define false 2

#define MAX_ARRAY_SIZE 1024 

typedef struct TreeNode {
    char data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

void trim(char* buf);
TreeNode* build_binary_tree(const char* buf, int* pos);
void populate_tree_array(TreeNode* node, char arr[], int index);
int get_tree_height(TreeNode* node);
int count_all_nodes(TreeNode* node);
int count_leaf_nodes(TreeNode* node);
void free_tree(TreeNode* node);

//공백제거함수
void trim(char* buf) {
    if (buf == NULL) return;
    char* src = buf, * dst = buf;
    while (*src != '\0') {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

int main(void) {
    char buf[1000];
    scanf("%[^\n]", buf);

    trim(buf);

    if (strlen(buf) == 0) {
        printf("0,0,0\n");
        return 0;
    }

    int pos = 0;
    TreeNode* root = build_binary_tree(buf, &pos);

    char tree_array[MAX_ARRAY_SIZE] = { 0 }; // 배열을 0으로 초기화
    populate_tree_array(root, tree_array, 1);

    //속성분석
    int height = get_tree_height(root);
    int total_nodes = count_all_nodes(root);
    int leaf_nodes = count_leaf_nodes(root);

    printf("%d,%d,%d\n", height, total_nodes, leaf_nodes);
    //A (B (C D) E (G H (I J (K (L) M (N)))))을 입력했을 때 5,13,6출력

    free_tree(root);

    return 0;
}

/**
 * @brief 새 노드를 생성하고 초기화하는 헬퍼 함수
 */
TreeNode* create_node(char data) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        // 메모리 할당 실패 시 프로그램 종료
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

/**
 * @brief '루트(왼쪽 오른쪽)' 규칙에 따라 문자열을 재귀적으로 파싱하여 이진 트리를 생성하는 함수
 */
TreeNode* build_binary_tree(const char* buf, int* pos) {
    if (buf[*pos] == '\0' || buf[*pos] == ')') {
        return NULL;
    }

    // 1. 루트 노드 생성
    char root_data = buf[*pos];
    (*pos)++;
    TreeNode* node = create_node(root_data);

    // 2. 자식이 있는지 확인 (바로 뒤에 '('가 오는지)
    if (buf[*pos] == '(') {
        (*pos)++; // '(' 건너뛰기

        // 3. 왼쪽 자식 파싱
        node->left = build_binary_tree(buf, pos);

        // 4. 오른쪽 자식이 있는지 확인
        if (buf[*pos] != ')') {
            node->right = build_binary_tree(buf, pos);
        }

        // 5. 자식 파싱이 끝나면 ')' 건너뛰기
        if (buf[*pos] == ')') {
            (*pos)++;
        }
    }
    return node;
}

/**
 * @brief 연결 트리를 순회하며 배열 표현을 채우는 함수 (루트가 인덱스 1)
 */
void populate_tree_array(TreeNode* node, char arr[], int index) {
    if (node == NULL || index >= MAX_ARRAY_SIZE) return;
    
    arr[index] = node->data; // 현재 노드 저장
    populate_tree_array(node->left, arr, 2 * index); // 왼쪽 자식
    populate_tree_array(node->right, arr, 2 * index + 1); // 오른쪽 자식
}

/**
 * @brief 트리의 높이를 계산하는 함수 (단일 노드 높이 = 0)
 */
int get_tree_height(TreeNode* node) {
    if (node == NULL) return -1;
    int left_h = get_tree_height(node->left);
    int right_h = get_tree_height(node->right);
    return (left_h > right_h ? left_h : right_h) + 1;
}

/**
 * @brief 트리의 전체 노드 수를 계산하는 함수
 */
int count_all_nodes(TreeNode* node) {
    if (node == NULL) return 0;
    return 1 + count_all_nodes(node->left) + count_all_nodes(node->right);
}

/**
 * @brief 트리의 단말(리프) 노드 수를 계산하는 함수
 */
int count_leaf_nodes(TreeNode* node) {
    if (node == NULL) return 0;
    if (node->left == NULL && node->right == NULL) return 1;
    return count_leaf_nodes(node->left) + count_leaf_nodes(node->right);
}

/**
 * @brief 사용된 모든 노드의 메모리를 해제하는 함수
 */
void free_tree(TreeNode* node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}
