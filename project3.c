#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NODES 50

typedef struct {
    char data;
    int left;
    int right;
} TreeNode;


TreeNode tree[MAX_NODES];
int node_count = 1; // 인덱스1을 루트로 저장

int stack[MAX_NODES];
int top = -1;

void push(int item) {
    stack[++top] = item;
}

int pop() {
    return stack[top--];
}

int is_empty() {
    return top == -1;
}

//공백제거함수
void trim(char* str) {
    char* write_ptr = str;
    char* read_ptr = str;
    while (*read_ptr) {
        if (!isspace((unsigned char)*read_ptr)) {
            *write_ptr++ = *read_ptr;
        }
        read_ptr++;
    }
    *write_ptr = '\0'; 
}


// 괄호 문자열을 파싱하여 배열 트리로 구축하는 함수
void build_tree_from_string(const char* str) {
    int parent_stack[MAX_NODES];
    int parent_top = -1;
    int last_node_index = 0;

    for (int i = 0; i < strlen(str); ++i) {
        

        if (isalpha(str[i])) {
            // 현재 노드 정보 채우기
            tree[node_count].data = str[i];
            tree[node_count].left = 0; // 0은 NULL을 의미
            tree[node_count].right = 0;

            // 부모 스택이 비어있지 않다면, 현재 노드를 부모의 자식으로 연결
            if (parent_top != -1) {
                int parent_index = parent_stack[parent_top];
                if (tree[parent_index].left == 0) {
                    tree[parent_index].left = node_count;
                } else {
                    tree[parent_index].right = node_count;
                }
            }
            last_node_index = node_count;
            node_count++;
        } else if (str[i] == '(') {
            // '(' 앞의 노드가 새로운 부모가 됨
            parent_stack[++parent_top] = last_node_index;
        } else if (str[i] == ')') {
            // 현재 서브트리 구성 완료, 부모 스택에서 pop
            parent_top--;
        }
    }
}

// 1. 반복적 전위 순회 (Iterative Pre-order)
void iterative_preorder(int root_index) {
    if (root_index == 0) return;
    top = -1; // 스택 초기화
    push(root_index);
    printf("pre-order: ");
    while (!is_empty()) {
        int current_index = pop();
        printf("%c ", tree[current_index].data);

        if (tree[current_index].right != 0) {
            push(tree[current_index].right);
        }
        if (tree[current_index].left != 0) {
            push(tree[current_index].left);
        }
    }
    printf("\n");
}

// 2. 반복적 중위 순회 (Iterative In-order)
void iterative_inorder(int root_index) {
    if (root_index == 0) return;
    top = -1; // 스택 초기화
    int current_index = root_index;
    printf("in-order:  ");
    while (current_index != 0 || !is_empty()) {
        while (current_index != 0) {
            push(current_index);
            current_index = tree[current_index].left;
        }
        current_index = pop();
        printf("%c ", tree[current_index].data);
        current_index = tree[current_index].right;
    }
    printf("\n");
}

// 3. 반복적 후위 순회 (Iterative Post-order)
void iterative_postorder(int root_index) {
    if (root_index == 0) return;
    int stack2[MAX_NODES];
    int top2 = -1;
    
    top = -1; 
    push(root_index);
    
    while (!is_empty()) {
        int current_index = pop();
        stack2[++top2] = current_index;

        if (tree[current_index].left != 0) {
            push(tree[current_index].left);
        }
        if (tree[current_index].right != 0) {
            push(tree[current_index].right);
        }
    }

    printf("post-order: ");
    while (top2 != -1) {
        printf("%c ", tree[stack2[top2--]].data);
    }
    printf("\n");
}

int main() {
    char input[256]; // 사용자 입력을 저장할 버퍼

    printf("input: ");
    scanf(" %[^\n]", input);

    trim(input);

    build_tree_from_string(input);

    iterative_preorder(1);  
    iterative_inorder(1);
    iterative_postorder(1);

    return 0;
}
