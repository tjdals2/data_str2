#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NODES 1000

typedef struct Node {
    char data;
    int left;
    int right;
} Node;

typedef struct Stack {
    int items[MAX_NODES];
    int top;
} Stack;

// 트리를 저장할 전역 배열과 노드 개수 카운터
Node tree[MAX_NODES];
int node_count = 1;


void init_stack(Stack *s) { s->top = -1; }
void push(Stack *s, int item) { s->items[++(s->top)] = item; }
int pop(Stack *s) { return s->items[(s->top)--]; }
int is_empty(Stack *s) { return s->top == -1; }

// 새 노드를 생성하고 전역 tree 배열에 추가
int new_node(char data) {
    tree[node_count].data = data;
    tree[node_count].left = -1;
    tree[node_count].right = -1;
    return node_count++;
}

// 공백제거하뭇
void trim(char *str) {
    if (str == NULL) return;
    char *writer = str, *reader = str;
    while (*reader) {
        if (!isspace((unsigned char)*reader)) {
            *writer++ = *reader;
        }
        reader++;
    }
    *writer = '\0';
}

// 표준 (Root Left Right) 구조를 해석하는 재귀 파서
int parse_tree(const char *str, int *pos) {
    // 닫는 괄호거나 문자열 끝이면 자식 없음(-1)을 반환
    if (str[*pos] == ')' || str[*pos] == '\0') {
        if (str[*pos] == ')') (*pos)++; // 닫는 괄호는 읽고 지나감
        return -1;
    }

    int root_idx = -1;

    // (A ... ) 형태의 서브트리
    if (str[*pos] == '(') {
        (*pos)++; // '(' 건너뛰기
        root_idx = new_node(str[*pos]); // 루트 노드 생성
        (*pos)++;

        // 재귀적으로 왼쪽과 오른쪽 자식 파싱
        tree[root_idx].left = parse_tree(str, pos);
        tree[root_idx].right = parse_tree(str, pos);

        // 서브트리의 ')'를 찾을 때까지 건너뜀
        if (str[*pos] == ')') (*pos)++;
    }
    // C, D 와 같은 단일 노드(리프 노드)
    else if (isalpha(str[*pos])) {
        root_idx = new_node(str[*pos]);
        (*pos)++;
    }
    return root_idx;
}

// 전위 순회 
void preorder_traversal(int root_idx) {
    if (root_idx == -1) return;
    Stack s;
    init_stack(&s);
    push(&s, root_idx);

    printf("pre-order: ");
    while (!is_empty(&s)) {
        int current_idx = pop(&s);
        printf("%c ", tree[current_idx].data);
        if (tree[current_idx].right != -1) push(&s, tree[current_idx].right);
        if (tree[current_idx].left != -1) push(&s, tree[current_idx].left);
    }
    printf("\n");
}

// 중위 순회 
void inorder_traversal(int root_idx) {
    if (root_idx == -1) return;
    Stack s;
    init_stack(&s);
    int current_idx = root_idx;

    printf("in-order: ");
    while (current_idx != -1 || !is_empty(&s)) {
        while (current_idx != -1) {
            push(&s, current_idx);
            current_idx = tree[current_idx].left;
        }
        current_idx = pop(&s);
        printf("%c ", tree[current_idx].data);
        current_idx = tree[current_idx].right;
    }
    printf("\n");
}

// 후위 순회 
void postorder_traversal(int root_idx) {
    if (root_idx == -1) return;
    Stack s1, s2;
    init_stack(&s1);
    init_stack(&s2);
    push(&s1, root_idx);

    while (!is_empty(&s1)) {
        int current_idx = pop(&s1);
        push(&s2, current_idx);
        if (tree[current_idx].left != -1) push(&s1, tree[current_idx].left);
        if (tree[current_idx].right != -1) push(&s1, tree[current_idx].right);
    }

    printf("post-order: ");
    while (!is_empty(&s2)) {
        printf("%c ", tree[pop(&s2)].data);
    }
    printf("\n");
}

int main(void) {
    char buf[MAX_NODES];

    printf("input: ");
    scanf("%[^\n]", buf);
    //(A (B C D) (E G (H I (J (K L) (M N))))) 입력

    trim(buf); 

    int pos = 0;
    int root_idx = parse_tree(buf, &pos); // 문자열을 해석해 트리 생성

    // 생성된 트리를 순회하며 결과 출력
    if (root_idx != -1) {
        preorder_traversal(root_idx);
        //A B C D E G H I J K L M N 출력
        inorder_traversal(root_idx);
        //C B D A G E I H L K J N M 출력
        postorder_traversal(root_idx);
        //C D B G I L K N M J H E A 출력
    }

    return 0;
}

