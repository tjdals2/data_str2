#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 정점의 개수
#define V 10
// 간선의 개수
#define E 20
// 무한대(연결되지 않음)를 나타내는 큰 값
#define INF 99999

/**
 * @brief 모든 노드 쌍 간의 최단 경로를 출력하는 함수
 * @param dist 최단 경로 비용이 저장된 2차원 배열
 */
void printAllPairsShortestPath(int dist[V][V]) {
    printf("\n--- 45개 노드 쌍 간의 최단 경로 ---\n");
    int pairCount = 0;
    
    // i < j 인 쌍만 출력하여 45개의 고유한 쌍을 출력합니다.
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            pairCount++;
            printf("[%2d] %d <-> %d : ", pairCount, i, j);
            if (dist[i][j] == INF) {
                printf("연결 경로 없음\n");
            } else {
                printf("%d\n", dist[i][j]);
            }
        }
    }
}

/**
 * @brief 플로이드-워셜 알고리즘을 수행하는 함수
 * @param graph 초기 가중치가 저장된 2차원 배열 (알고리즘 수행 후 최단 경로로 갱신됨)
 */
void floydWarshall(int graph[V][V]) {
    // dist 배열을 graph로 초기화
    int dist[V][V];
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            dist[i][j] = graph[i][j];
        }
    }

    // k = 거쳐가는 노드
    for (int k = 0; k < V; k++) {
        // i = 출발 노드
        for (int i = 0; i < V; i++) {
            // j = 도착 노드
            for (int j = 0; j < V; j++) {
                // i에서 j로 가는 기존 경로보다
                // i에서 k를 거쳐 j로 가는 경로가 더 짧은지 확인
                if (dist[i][k] != INF && dist[k][j] != INF && 
                    dist[i][k] + dist[k][j] < dist[i][j]) 
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    // 최종 최단 경로 출력
    printAllPairsShortestPath(dist);
}

int main() {
    int graph[V][V];

    // 난수 시드 초기화
    srand(time(NULL));

    // 1. 그래프 초기화 (모든 경로를 INF, 자신은 0으로)
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) {
                graph[i][j] = 0;
            } else {
                graph[i][j] = INF;
            }
        }
    }

    // 2. 무작위 간선 20개 생성 (가중치는 1~10 사이)
    printf("--- 생성된 무작위 간선 (총 %d개) ---\n", E);
    int edgesAdded = 0;
    while (edgesAdded < E) {
        int u = rand() % V; // 0 ~ 9 사이의 정점
        int v = rand() % V; // 0 ~ 9 사이의 정점
        int weight = (rand() % 10) + 1; // 가중치 1 ~ 10

        // 자기 자신으로 가는 간선, 이미 존재하는 간선은 제외
        if (u != v && graph[u][v] == INF) {
            // 무방향 그래프로 가정 (u->v, v->u 둘 다 설정)
            graph[u][v] = weight;
            graph[v][u] = weight;
            edgesAdded++;
            printf("  간선 %2d: %d <-> %d (가중치: %d)\n", edgesAdded, u, v, weight);
        }
    }

    // 3. 플로이드-워셜 알고리즘 실행
    floydWarshall(graph);

    return 0;
}
