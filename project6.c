#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V 100         // 정점의 개수
#define E_SPARSE 100  // 희소 그래프의 간선 개수
#define E_DENSE 4000  // 밀집 그래프의 간선 개수

// 인접 리스트 자료구조
typedef struct Node {
    int dest;
    struct Node* next;
} Node;

typedef struct {
    Node* head[V];
} AdjList;

// 성능 측정을 위한 전역 변수
// 각 연산을 수행할 때의 비교 횟수를 저장
long long comparisons = 0;

// 1. 자료구조 초기화 및 메모리 해제

// 인접 행렬 초기화
void init_matrix(int matrix[V][V]) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            matrix[i][j] = 0;
        }
    }
}

// 인접 리스트 초기화
void init_list(AdjList* list) {
    for (int i = 0; i < V; i++) {
        list->head[i] = NULL;
    }
}

// 인접 리스트 메모리 해제
void free_list(AdjList* list) {
    for (int i = 0; i < V; i++) {
        Node* curr = list->head[i];
        while (curr != NULL) {
            Node* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
}

// 그래프 랜덤 생성
// (undirected graph, self-loop 없음)
void generate_random_edges(int E, int matrix[V][V], AdjList* list) {
    int count = 0;
    while (count < E) {
        int u = rand() % V;
        int v = rand() % V;

        // self-loop 방지 및 이미 존재하는 간선인지 확인 (행렬 기준)
        if (u != v && matrix[u][v] == 0) {
            // 1. 인접 행렬에 추가
            matrix[u][v] = 1;
            matrix[v][u] = 1;

            // 2. 인접 리스트에 추가 (u -> v)
            Node* newNode1 = (Node*)malloc(sizeof(Node));
            newNode1->dest = v;
            newNode1->next = list->head[u];
            list->head[u] = newNode1;

            // 3. 인접 리스트에 추가 (v -> u)
            Node* newNode2 = (Node*)malloc(sizeof(Node));
            newNode2->dest = u;
            newNode2->next = list->head[v];
            list->head[v] = newNode2;

            count++;
        }
    }
}

// 2. 성능 측정 함수들

// [지표 1] 메모리 용량 (이론적 계산치)
long long get_matrix_memory() {
    // V * V 크기의 2차원 int 배열
    return (long long)sizeof(int) * V * V;
}

long long get_list_memory(int E) {
    // V개의 Node 포인터 배열 + (E * 2)개의 Node 구조체
    // (무방향 그래프이므로 간선 1개당 노드 2개 생성)
    long long array_size = (long long)sizeof(Node*) * V;
    long long node_size = (long long)sizeof(Node) * E * 2;
    return array_size + node_size;
}

// [지표 2] 간선 삽입/삭제

// 간선 삽입 (Matrix) - O(1)
void insert_edge_matrix(int matrix[V][V], int u, int v) {
    comparisons = 0;
    comparisons++; // 존재 여부 확인 1회
    if (matrix[u][v] == 0) {
        matrix[u][v] = 1;
        matrix[v][u] = 1;
    }
}

// 간선 삽입 (List) - O(deg(u) + deg(v))
void insert_edge_list(AdjList* list, int u, int v) {
    comparisons = 0;
    
    // 1. u -> v 가 이미 있는지 확인
    Node* temp = list->head[u];
    while (temp != NULL) {
        comparisons++; // 노드의 dest와 v를 비교
        if (temp->dest == v) {
            return; // 이미 존재하므로 종료
        }
        temp = temp->next;
    }

    // 2. v -> u 가 이미 있는지 확인 (무방향 그래프이므로)
    temp = list->head[v];
     while (temp != NULL) {
        comparisons++; // 노드의 dest와 u를 비교
        if (temp->dest == u) {
            return; // 이미 존재
        }
        temp = temp->next;
    }

    // 3. 간선 추가 (u -> v)
    Node* newNode1 = (Node*)malloc(sizeof(Node));
    newNode1->dest = v;
    newNode1->next = list->head[u];
    list->head[u] = newNode1;

    // 4. 간선 추가 (v -> u)
    Node* newNode2 = (Node*)malloc(sizeof(Node));
    newNode2->dest = u;
    newNode2->next = list->head[v];
    list->head[v] = newNode2;
}

// 간선 삭제 (Matrix) - O(1)
void delete_edge_matrix(int matrix[V][V], int u, int v) {
    comparisons = 0;
    comparisons++; // 존재 여부 확인 1회
    if (matrix[u][v] == 1) {
        matrix[u][v] = 0;
        matrix[v][u] = 0;
    }
}

// 간선 삭제 (List) - O(deg(u) + deg(v))
void delete_edge_list(AdjList* list, int u, int v) {
    comparisons = 0;

    // 1. u의 리스트에서 v 삭제
    Node* curr = list->head[u];
    Node* prev = NULL;
    while (curr != NULL) {
        comparisons++; // 노드의 dest와 v를 비교
        if (curr->dest == v) {
            if (prev == NULL) { // 헤드 노드인 경우
                list->head[u] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    // 2. v의 리스트에서 u 삭제
    curr = list->head[v];
    prev = NULL;
    while (curr != NULL) {
        comparisons++; // 노드의 dest와 u를 비교
        if (curr->dest == u) {
            if (prev == NULL) {
                list->head[v] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}


// [지표 3] 두 정점의 연결 여부 확인

// 연결 확인 (Matrix) - O(1)
void check_edge_matrix(int matrix[V][V], int u, int v) {
    comparisons = 1; // 행렬의 [u][v] 인덱스 접근 1회
    // if (matrix[u][v] == 1) { ... }
}

// 연결 확인 (List) - O(deg(u))
void check_edge_list(AdjList* list, int u, int v) {
    comparisons = 0;
    Node* temp = list->head[u];
    while (temp != NULL) {
        comparisons++; // 노드의 dest와 v를 비교
        if (temp->dest == v) {
            break; // 찾음
        }
        temp = temp->next;
    }
}

// [지표 4] 특정 노드의 인접 노드 출력

// 인접 노드 출력 (Matrix) - O(V)
void print_neighbors_matrix(int matrix[V][V], int u) {
    comparisons = 0;
    for (int v = 0; v < V; v++) {
        comparisons++; // 행렬의 [u][v] 값을 확인
        if (matrix[u][v] == 1) {
            // printf("%d ", v); 
        }
    }
    // printf("\n");
}

// 인접 노드 출력 (List) - O(deg(u))
void print_neighbors_list(AdjList* list, int u) {
    comparisons = 0;
    Node* temp = list->head[u];
    while (temp != NULL) {
        comparisons++; // 현재 노드가 NULL인지 확인 (비교 1회)
        // printf("%d ", temp->dest); 
        temp = temp->next;
    }
    // printf("\n");
}


// 실행함수
void run_analysis(const char* graph_type, int E, int matrix[V][V], AdjList* list) {
    printf("===============================================\n");
    printf("%s 분석 (V=%d, E=%d)\n", graph_type, V, E);

    // 지표 1: 메모리 용량
    printf("\n[지표 1] 메모리 사용량 (이론적 계산치)\n");
    printf("  - 인접 행렬: %lld bytes\n", get_matrix_memory());
    printf("  - 인접 리스트: %lld bytes\n", get_list_memory(E));

    // 지표 2: 간선 삽입/삭제
    // 테스트를 위해 존재하지 않는 간선 (u_ins, v_ins) 하나를 찾음
    int u_ins, v_ins;
    do {
        u_ins = rand() % V;
        v_ins = rand() % V;
    } while (u_ins == v_ins || matrix[u_ins][v_ins] == 1);

    printf("\n[지표 2] 간선 삽입 (존재하지 않는 간선 (%d, %d) 삽입)\n", u_ins, v_ins);
    insert_edge_matrix(matrix, u_ins, v_ins);
    printf("  - 인접 행렬 비교 횟수: %lld\n", comparisons);
    insert_edge_list(list, u_ins, v_ins);
    printf("  - 인접 리스트 비교 횟수: %lld\n", comparisons);

    printf("\n[지표 2] 간선 삭제 (방금 삽입한 간선 (%d, %d) 삭제)\n", u_ins, v_ins);
    delete_edge_matrix(matrix, u_ins, v_ins);
    printf("  - 인접 행렬 비교 횟수: %lld\n", comparisons);
    delete_edge_list(list, u_ins, v_ins);
    printf("  - 인접 리스트 비교 횟수: %lld\n", comparisons);


    // 지표 3: 연결 여부 확인
    // 3-1. 존재하는 간선 테스트
    int u_exist, v_exist;
    do {
        u_exist = rand() % V;
        v_exist = rand() % V;
    } while (u_exist == v_exist || matrix[u_exist][v_exist] == 0); // 0인 것을 찾으면 안됨

    printf("\n[지표 3] 연결 여부 확인 (존재하는 간선 (%d, %d))\n", u_exist, v_exist);
    check_edge_matrix(matrix, u_exist, v_exist);
    printf("  - 인접 행렬 비교 횟수: %lld\n", comparisons);
    check_edge_list(list, u_exist, v_exist);
    printf("  - 인접 리스트 비교 횟수: %lld\n", comparisons);

    // 3-2. 존재하지 않는 간선 테스트 (아까 삭제했던 간선 사용)
    printf("\n[지표 3] 연결 여부 확인 (존재하지 않는 간선 (%d, %d))\n", u_ins, v_ins);
    check_edge_matrix(matrix, u_ins, v_ins);
    printf("  - 인접 행렬 비교 횟수: %lld\n", comparisons);
    check_edge_list(list, u_ins, v_ins);
    printf("  - 인접 리스트 비교 횟수: %lld\n", comparisons);


    // --- 지표 4: 인접 노드 출력 ---
    int test_node = rand() % V;
    printf("\n[지표 4] 인접 노드 탐색 (임의의 정점 %d)\n", test_node);
    print_neighbors_matrix(matrix, test_node);
    printf("  - 인접 행렬 비교 횟수: %lld\n", comparisons);
    print_neighbors_list(list, test_node);
    printf("  - 인접 리스트 비교 횟수: %lld\n", comparisons);
}

int main() {
    srand((unsigned int)time(NULL));

    // 희소그래프
    int sparse_matrix[V][V];
    AdjList sparse_list;
    init_matrix(sparse_matrix);
    init_list(&sparse_list);
    generate_random_edges(E_SPARSE, sparse_matrix, &sparse_list);
    run_analysis("희소 그래프", E_SPARSE, sparse_matrix, &sparse_list);

    // 밀집그래프
    int dense_matrix[V][V];
    AdjList dense_list;
    init_matrix(dense_matrix);
    init_list(&dense_list);
    generate_random_edges(E_DENSE, dense_matrix, &dense_list);
    run_analysis("밀집 그래프", E_DENSE, dense_matrix, &dense_list);

    free_list(&sparse_list);
    free_list(&dense_list);

    return 0;
}
