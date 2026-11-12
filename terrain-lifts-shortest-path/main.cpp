#include <cstdio>
#include <climits>

static const int INF = INT_MAX / 2;

inline void heapPush(int time, int node,
    int*& heapTime, int*& heapNode,
    int& heapSize, int& heapCap)
{
    int idx = ++heapSize;
    if (heapSize >= heapCap) {
        int newCap = heapCap * 2;
        int* newHT = new int[newCap];
        int* newHN = new int[newCap];
        for (int i = 1; i < heapSize; i++) {
            newHT[i] = heapTime[i];
            newHN[i] = heapNode[i];
        }
        delete[] heapTime;
        delete[] heapNode;
        heapTime = newHT;
        heapNode = newHN;
        heapCap = newCap;
    }
    while (idx > 1 && time < heapTime[idx / 2]) {
        heapTime[idx] = heapTime[idx / 2];
        heapNode[idx] = heapNode[idx / 2];
        idx /= 2;
    }
    heapTime[idx] = time;
    heapNode[idx] = node;
}

int main()
{
    int width, height;
    if (scanf_s("%d %d", &width, &height) != 2) return 0;
    int sx, sy, tx, ty, numLifts;
    scanf_s("%d %d %d %d %d", &sx, &sy, &tx, &ty, &numLifts);

    int N = width * height;

    // Read lifts, build forward‐star list
    int* head = new int[N];
    for (int i = 0; i < N; i++) head[i] = -1;

    int* liftDest = new int[numLifts];
    int* liftTime = new int[numLifts];
    int* liftInterval = new int[numLifts];
    int* nextLift = new int[numLifts];

    for (int i = 0; i < numLifts; i++) {
        int lx, ly, dx, dy, t, interval;
        scanf_s("%d %d %d %d %d %d", &lx, &ly, &dx, &dy, &t, &interval);
        int u = ly * width + lx;
        int v = dy * width + dx;
        liftDest[i] = v;
        liftTime[i] = t;
        liftInterval[i] = interval;
        nextLift[i] = head[u];
        head[u] = i;
    }

    // Read terrain heights
    int* h = new int[N];
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            scanf_s("%d", &h[y * width + x]);

    // Distance array
    int* dist = new int[N];
    for (int i = 0; i < N; i++) dist[i] = INF;

    int start = sy * width + sx;
    int goal = ty * width + tx;
    dist[start] = 0;

    // Min-heap (time,node) starting with (0,start)
    int heapCap = (N + 1) * 5 + 10;
    int heapSize = 0;
    int* heapTime = new int[heapCap];
    int* heapNode = new int[heapCap];
    heapPush(0, start, heapTime, heapNode, heapSize, heapCap);

    const int DX[4] = { 1, -1, 0, 0 };
    const int DY[4] = { 0,  0, 1,-1 };

    while (heapSize > 0) {
        // Pop min
        int curTime = heapTime[1], u = heapNode[1];
        // remove root
        int lastT = heapTime[heapSize];
        int lastN = heapNode[heapSize--];
        int pos = 1;
        while (pos * 2 <= heapSize) {
            int c = pos * 2;
            if (c < heapSize && heapTime[c + 1] < heapTime[c]) c++;
            if (heapTime[c] < lastT) {
                heapTime[pos] = heapTime[c];
                heapNode[pos] = heapNode[c];
                pos = c;
            }
            else break;
        }
        heapTime[pos] = lastT;
        heapNode[pos] = lastN;

        if (curTime != dist[u]) continue;
        if (u == goal) break;

        int ux = u % width, uy = u / width;
        int hu = h[u];

        // Walk to 4 neighbors
        for (int d = 0; d < 4; d++) {
            int vx = ux + DX[d], vy = uy + DY[d];
            if (vx < 0 || vx >= width || vy < 0 || vy >= height) continue;
            int v = vy * width + vx;
            int cost = h[v] > hu ? (h[v] - hu + 1) : 1;
            int nt = curTime + cost;
            if (nt < dist[v]) {
                dist[v] = nt;
                heapPush(nt, v, heapTime, heapNode, heapSize, heapCap);
            }
        }

        // Use any lift(s) from u
        for (int li = head[u]; li != -1; li = nextLift[li]) {
            int v = liftDest[li];
            int interval = liftInterval[li];
            int wait = (interval - (curTime % interval)) % interval;
            int nt = curTime + wait + liftTime[li];
            if (nt < dist[v]) {
                dist[v] = nt;
                heapPush(nt, v, heapTime, heapNode, heapSize, heapCap);
            }
        }
    }

    printf("%d\n", dist[goal]);

    // Clean up
    delete[] head;
    delete[] liftDest;
    delete[] liftTime;
    delete[] liftInterval;
    delete[] nextLift;
    delete[] h;
    delete[] dist;
    delete[] heapTime;
    delete[] heapNode;
    return 0;
}
