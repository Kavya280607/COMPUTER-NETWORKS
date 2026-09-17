#include <stdio.h>
#include <stdlib.h>
#define INF 9999
typedef struct Edge {
    int src;
    int dest;
    int weight;
    struct Edge *next;
} Edge;
typedef struct Graph {
    int vertices;
    Edge *head;
} Graph;
Graph *createGraph(int v)
{
    Graph *g = (Graph *)malloc(sizeof(Graph));
    g->vertices = v;
    g->head = NULL;
    return g;
}
void addEdge(Graph *g, int src, int dest, int weight)
{
    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->src = src;
    newEdge->dest = dest;
    newEdge->weight = weight;
    newEdge->next = g->head;
    g->head = newEdge;
}
Graph *readGraph()
{
    int V, E, i;
    printf("Enter Total Number of Routers: ");
    if(scanf("%d", &V) != 1) return NULL;
    Graph *g = createGraph(V);
    printf("Enter Total Number of Links: ");
    if(scanf("%d", &E) != 1) return NULL;
    printf("\nEnter: [Source] [Destination] [Cost]\n");
    for(i = 0; i < E; i++)
    {
        int s, d, w;
        if(scanf("%d%d%d", &s, &d, &w) == 3) {
            addEdge(g, s, d, w); 
        }
    }
    return g;
}
void printDistanceTable(int *dist, int V)
{
    printf("\n  Router\t| Distance\n  -------------------");
    for(int i = 0; i < V; i++) {
        if(dist[i] == INF)
            printf("\n  %d\t\t| INF", i);
        else
            printf("\n  %d\t\t| %d", i, dist[i]);
    }
    printf("\n");
}
void printPath(int dest, int *parent, int src)
{
    if (dest == src) {
        printf("%d", src);
        return;
    }
    if (parent[dest] == -1) {
        printf("%d", dest);
        return;
    }
    printPath(parent[dest], parent, src);
    printf(" -> %d", dest);
}
int getNextHop(int dest, int *parent, int src)
{
    if (parent[dest] == -1) return -1;
    int curr = dest;
    while (parent[curr] != src && parent[curr] != -1) {
        curr = parent[curr];
    }
    return curr;
}
void executeBellmanFord(Graph *g, int src)
{
    int V = g->vertices;
    int *dist = (int *)malloc(V * sizeof(int));
    int *parent = (int *)malloc(V * sizeof(int));
    printf("\n*****************************************");
    printf("\n  STARTING ROUTER : %d", src);
    printf("\n*****************************************");
    for(int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[src] = 0;
    printf("\n\nInitial Distance Table:");
    printDistanceTable(dist, V);
    for(int count = 1; count <= V - 1; count++)
    {
        printf("\n--- ITERATION RUN #%d ---", count);
        printf("\n-----------------------------------------");
        int relaxationPerformed = 0;
        Edge *temp = g->head;
        while(temp != NULL)
        {
            int u = temp->src;
            int v = temp->dest;
            int w = temp->weight;
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                printf("\n\n Checking Link : %d --> %d", u, v);
                printf("\n   Old Distance : ");
                if(dist[v] == INF) printf("INF"); else printf("%d", dist[v]);
                dist[v] = dist[u] + w;
                parent[v] = u;
                printf("\n   New Distance : %d", dist[v]);
                printf("\n   --> Distance Updated!");
                relaxationPerformed = 1;
            }
            temp = temp->next;
        }
        if(!relaxationPerformed) {
            printf("\n\n No changes made in this step.");
        }
        printf("\n\nDistance Table After Iteration %d", count);
        printDistanceTable(dist, V);
        printf("-----------------------------------------\n");
    }
    printf("\nChecking for negative loops...");
    int hasNegativeCycle = 0;
    Edge *temp = g->head;
    while(temp != NULL) {
        int u = temp->src;
        int v = temp->dest;
        int w = temp->weight;
        if (dist[u] != INF && dist[u] + w < dist[v]) {
            hasNegativeCycle = 1;
            break;
        }
        temp = temp->next;
    }
    if(hasNegativeCycle) {
        printf("\n Found a negative loop in the graph!\n");
    } else {
        printf("\n No negative loops found.\n");
    }
    printf("\n============================================================");
    printf("\n            FINAL ROUTING TABLE FOR ROUTER %d", src);
    printf("\n============================================================");
    printf("\nTo Router\tTotal Cost\tNext Hop\tFull Path");
    printf("\n------------------------------------------------------------\n");
    for(int i = 0; i < V; i++) {
        if(i == src) continue;
        printf("%d\t\t", i);
        if(dist[i] == INF) {
            printf("INF\t\t-\t\tNo Path\n");
        } else {
            int nextHopVal = getNextHop(i, parent, src);
            printf("%d\t\t", dist[i]);
            if (nextHopVal == -1) {
                printf("-\t\t");
            } else {
                printf("%d\t\t", nextHopVal);
            }
            printPath(i, parent, src);
            printf("\n");
        }
    }
    printf("============================================================\n");
    free(dist);
    free(parent);
}
int main()
{
    Graph *graph = readGraph();
    if(!graph) return 1;
    for(int i = 0; i < graph->vertices; i++) {
        executeBellmanFord(graph, i);
    }
    return 0;
}

