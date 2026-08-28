#define INF 1000000000

int findCheapestPrice(int n, int** flights, int flightsSize,
                      int* flightsColSize, int src, int dst, int k)
{
    int price[n];
    int temp[n];

    int i, j;

    for(i = 0; i < n; i++)
        price[i] = INF;

    price[src] = 0;

    for(i = 0; i <= k; i++)
    {
        for(j = 0; j < n; j++)
            temp[j] = price[j];

        for(j = 0; j < flightsSize; j++)
        {
            int from = flights[j][0];
            int to = flights[j][1];
            int cost = flights[j][2];

            if(price[from] != INF &&
               price[from] + cost < temp[to])
            {
                temp[to] = price[from] + cost;
            }
        }

        for(j = 0; j < n; j++)
            price[j] = temp[j];
    }

    if(price[dst] == INF)
        return -1;

    return price[dst];
}
