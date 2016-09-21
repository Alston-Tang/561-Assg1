#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <stack>
#include <functional>

typedef unsigned long routeIndex;

using namespace std;

struct path
{
    routeIndex ind;
    int dis;
    path(routeIndex _ind, int _dis) : ind(_ind), dis(_dis) {}
};

class Map {
    unordered_map<string, routeIndex> *hash;
    vector<vector<pair<routeIndex, int>>*> *distanceList;
    vector<int> *sunday;
    vector<string> *reverse;
public:
    Map() {
        hash = new unordered_map<string, routeIndex>();
        reverse = new vector<string>();
        distanceList = new vector<vector<pair<routeIndex, int>>*>();
        sunday = new vector<int>();
    }
    ~Map() {
        delete hash;
        delete reverse;
        for (auto m : *distanceList) {
            delete m;
        }
        delete distanceList;
        delete sunday;
    }
    routeIndex queryAddPlace(string &s) {
        auto res = hash->find(s);
        routeIndex ind;
        if (res == hash->end()) {
            ind = hash->size();
            hash->insert(pair<string, routeIndex>(s, ind));
            reverse->push_back(s);
            distanceList->push_back(new vector<pair<routeIndex, int>>());
            sunday->push_back(-1);
            return ind;
        }
        else {
            ind = res->second;
            return ind;
        }
    }

    void addRoute(string &p1, string &p2, int dis) {
        routeIndex p1routeIndex = queryAddPlace(p1);
        routeIndex p2routeIndex = queryAddPlace(p2);
        distanceList->at(p1routeIndex)->push_back(pair<routeIndex, int>(p2routeIndex, dis));
    }

    pair<vector<pair<routeIndex, int>>::iterator, vector<pair<routeIndex, int>>::iterator> queryAllDistance(routeIndex i1) {
        auto cont = distanceList->at(i1);
        return pair<vector<pair<routeIndex, int>>::iterator, vector<pair<routeIndex, int>>::iterator>(cont->begin(), cont->end());
    }

    void setSunday(string &p, int dis) {
        routeIndex i = queryAddPlace(p);
        sunday->at(i) = dis;
    }

    int querySunday(routeIndex i) {
        return sunday->at(i);
    }

    void printAll() {
        for (routeIndex i = 0; i < distanceList->size(); i++) {
            cout << reverse->at(i) << " -> ";
            for (auto p2 : *distanceList->at(i)) {
                cout << " " << reverse->at(p2.first) << ":" << p2.second << " |";
            }
            cout << endl;
        }
        for (routeIndex i = 0; i < sunday->size(); i++) {
            cout << reverse->at(i) << " : " << sunday->at(i) << endl;
        }
    }

    int size() {
        return static_cast<int>(hash->size());
    }

    string queryName(routeIndex i) {
        return reverse->at(i);
    }

    void keepOrder() {
        for (auto it : *distanceList) {
            sort(it->begin(), it->end());
        }
    }
};

stack<path>* dfs(routeIndex ori, routeIndex dest, Map &m) {
    stack<routeIndex> s;
    stack<path> *rv = new stack<path>();
    bool *visited = new bool[m.size()];
    routeIndex *pre = new routeIndex[m.size()];

    fill(visited, visited + m.size(), 0);

    pre[ori] = ori;
    visited[ori] = true;
    s.push(ori);

    while (!visited[dest]) {
        routeIndex curIndex = s.top();
        s.pop();
        auto range = m.queryAllDistance(curIndex);
        if (range.first == range.second) continue;
        for (auto it = range.second - 1; it >= range.first; it--) {
            routeIndex nextIndex = it->first;
            if (!visited[nextIndex]){
                pre[it->first] = curIndex;
                visited[it->first] = true;
                s.push(it->first);
            }
        }
    }
    int cur = dest;
    while (cur != ori) {
        rv->push(path(cur, 0));
        cur = pre[cur];
    }
    rv->push(path(ori, 0));
    return rv;
}

struct aStarNode {
    routeIndex ind, prev;
    int estDis, oriDis;
    unsigned int ser;
    bool operator>(const aStarNode &r) const {
        return (estDis > r.estDis) || (estDis == r.estDis && ser > r.ser);
    }
    aStarNode(routeIndex _ind, routeIndex _prev, int _estDis, int _oriDis, unsigned int _ser)
            : ind(_ind), prev(_prev), estDis(_estDis), oriDis(_oriDis), ser(_ser) {};
};

stack<path>* aStar(routeIndex ori, routeIndex dest, Map &m) {
    unsigned int count = 0;
    priority_queue<aStarNode, vector<aStarNode>, greater<aStarNode>> q;
    stack<path> *rv = new stack<path>();
    aStarNode node = aStarNode(ori, ori, m.querySunday(ori), 0, count++);
    int *curMin = new int[m.size()];
    bool *visited = new bool[m.size()];
    routeIndex *pre = new routeIndex[m.size()];
    int *dis = new int[m.size()];

    fill(visited, visited + m.size(), 0);

    q.push(node);

    while (true) {
        node = q.top();
        q.pop();
        if (visited[node.ind]) continue;
        visited[node.ind] = true;
        curMin[node.ind] = node.oriDis;
        pre[node.ind] = node.prev;
        dis[node.ind] = node.oriDis;
        if (node.ind == dest) {
            break;
        }
        auto range = m.queryAllDistance(node.ind);
        for (auto it = range.first; it != range.second; it++) {
            routeIndex ind = it->first;
            int oriDis = node.oriDis + it->second;
            if (!visited[ind]) {
                q.push(aStarNode(ind, node.ind, oriDis + m.querySunday(ind), oriDis, count++));
            } else if(oriDis < curMin[ind]){
                visited[ind] = false;
                curMin[ind] = oriDis;
                q.push(aStarNode(ind, node.ind, oriDis + m.querySunday(ind), oriDis, count++));
            }
        }
    }

    int cur = dest;
    while (cur != ori) {
        rv->push(path(cur, dis[cur]));
        cur = pre[cur];
    }
    rv->push(path(ori, 0));
    return rv;
}

struct ucsNode {
    routeIndex ind, prev;
    int dis;
    unsigned int ser;
    bool operator>(const ucsNode &r) const {
        return (dis > r.dis) || (dis == r.dis && ser > r.ser);
    }
    ucsNode(routeIndex _ind, routeIndex _prev, int _dis, unsigned int _ser) : ind(_ind), dis(_dis), prev(_prev), ser(_ser) {};
};

stack<path>* ucs(routeIndex ori, routeIndex dest, Map &m) {
    unsigned int count = 0;
    priority_queue<ucsNode, vector<ucsNode>, greater<ucsNode>> q;
    stack<path> *rv = new stack<path>();
    ucsNode node = ucsNode(ori, ori, 0, count++);
    bool *visited = new bool[m.size()];
    routeIndex *pre = new routeIndex[m.size()];
    int *dis = new int[m.size()];

    fill(visited, visited + m.size(), 0);

    q.push(node);

    while (true) {
        node = q.top();
        q.pop();
        if (visited[node.ind]) continue;
        visited[node.ind] = true;
        pre[node.ind] = node.prev;
        dis[node.ind] = node.dis;
        if (node.ind == dest) {
            break;
        }
        auto range = m.queryAllDistance(node.ind);
        for (auto it = range.first; it != range.second; it++) {
            int ind = it->first;
            if (!visited[ind]) {
                q.push(ucsNode(ind, node.ind, node.dis + it->second, count++));
            }
        }
    }

    int cur = dest;
    while (cur != ori) {
        rv->push(path(cur, dis[cur]));
        cur = pre[cur];
    }
    rv->push(path(ori, 0));
    return rv;
}

stack<path>* bfs(routeIndex ori, routeIndex dest, Map &m) {

    queue<int> q;
    stack<path> *rv = new stack<path>();
    int p;
    bool *visited = new bool[m.size()];
    routeIndex *pre = new routeIndex[m.size()];

    fill(visited, visited + m.size(), 0);

    pre[ori] = ori;
    visited[ori] = true;
    q.push(ori);

    while (!visited[dest]) {
        if (q.empty()) {
            return NULL;
        }
        p = q.front();
        auto range = m.queryAllDistance(p);
        for (auto it = range.first; it != range.second; it++) {
            int to = it->first;
            if (!visited[to]) {
                pre[to] = p;
                visited[to] = true;
                q.push(to);
            }
        }
        q.pop();
    }

    int cur = dest;
    while (cur != ori) {
        rv->push(path(cur, 0));
        cur = pre[cur];
    }
    rv->push(path(cur, 0));
    return rv;
}

void printIndexStack(stack<path> *s, Map &m, ofstream *out) {
    if (!s) {
        if (out){
            (*out) << "No Solution" << endl;
        } else {
            cout << "No Solution" << endl;
        }
        return;
    }
    int count = 0;
    while (!s->empty()) {
        path &i = s->top();
        if (out) {
            (*out) << m.queryName(i.ind) << " " << count << endl;
        } else {
            cout << m.queryName(i.ind) << " " << count << endl;
        }
        count++;
        s->pop();
    }
}

void printIndexStackDistance(stack<path> *s, Map &m, ofstream *out) {
    if (!s) {
        if (out){
            (*out) << "No Solution" << endl;
        } else {
            cout << "No Solution" << endl;
        }
        return;
    }
    while (!s->empty()) {
        path &i = s->top();
        if (out){
            (*out) << m.queryName(i.ind) << " " << i.dis << endl;
        } else {
            cout << m.queryName(i.ind) << " " << i.dis << endl;
        }
        s->pop();
    }
}
/*
void test() {
    const int testRange = 7;
    const int testStart = 1;
    for (int i = testStart; i <= testRange; i++) {
        ifstream in;
        Map m;
        in.open("in/in" + to_string(i));
        if (!in.good()) {
            cout << "Input file error";
            exit(-1);
        }
        int numRoute, numSunday;
        int dis;
        string p1, p2;
        string method, origin, destination, place1, place2;
        in >> method;
        in >> origin >> destination;
        in >> numRoute;
        for (int i = 0; i < numRoute; i++) {
            in >> p1 >> p2 >> dis;
            m.addRoute(p1, p2, dis);
        }
        in >> numSunday;
        for (int i = 0; i < numSunday; i++) {
            in >> p1 >> dis;
            m.setSunday(p1, dis);
        }
        routeIndex originIndex = m.queryAddPlace(origin);
        routeIndex destinationIndex = m.queryAddPlace(destination);

        stack<path> *res = NULL;
        if (method == "BFS" || method == "ALL") {
            ofstream out;
            out.open(string("in/in") + to_string(i) + string("_BFS"));
            res = bfs(originIndex, destinationIndex, m);
            printIndexStack(res, m, &out);
        }
        if (method == "DFS" || method == "ALL") {
            ofstream out;
            out.open(string("in/in") + to_string(i) + string("_DFS"));
            res = dfs(originIndex, destinationIndex, m);
            printIndexStack(res, m, &out);
        }
        if (method == "UCS" || method == "ALL") {
            ofstream out;
            out.open(string("in/in") + to_string(i) + string("_USC"));
            res = ucs(originIndex, destinationIndex, m);
            printIndexStackDistance(res, m, &out);
        }
        if (method == "A*" || method == "ALL") {
            ofstream out;
            out.open(string("in/in") + to_string(i) + string("_A"));
            res = aStar(originIndex, destinationIndex, m);
            printIndexStackDistance(res, m, &out);
        }
        in.close();
    }
}
*/

int main() {
    ifstream in;
    Map m;
    in.open("input.txt");
    if (!in.good()) {
        cout << "Input file error";
        exit(-1);
    }
    int numRoute, numSunday;
    int dis;
    string p1, p2;
    string method, origin, destination, place1, place2;
    in >> method;
    in >> origin >> destination;
    in >> numRoute;
    for (int i = 0; i < numRoute; i++) {
        in >> p1 >> p2 >> dis;
        m.addRoute(p1, p2, dis);
    }
    in >> numSunday;
    for (int i = 0; i < numSunday; i++) {
        in >> p1 >> dis;
        m.setSunday(p1, dis);
    }
    routeIndex originIndex = m.queryAddPlace(origin);
    routeIndex destinationIndex = m.queryAddPlace(destination);

    stack<path> *res = NULL;
    ofstream out;
    out.open("output.txt");
    if (!out.good()) {
        cout << "Output file error";
        exit(-1);
    }
    if (method == "BFS") {
        res = bfs(originIndex, destinationIndex, m);
        printIndexStack(res, m, &out);
    }
    else if (method == "DFS") {
        res = dfs(originIndex, destinationIndex, m);
        printIndexStack(res, m, &out);
    }
    else if (method == "UCS") {
        res = ucs(originIndex, destinationIndex, m);
        printIndexStackDistance(res, m, &out);
    }
    else if (method == "A*") {
        res = aStar(originIndex, destinationIndex, m);
        printIndexStackDistance(res, m, &out);
    }
    return 0;
}