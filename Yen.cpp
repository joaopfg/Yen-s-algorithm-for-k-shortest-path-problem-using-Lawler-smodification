#include <bits/stdc++.h>

#define rep(i,begin,end) for(int i=begin;i<=end;i++)
#define repi(i,end,begin) for(int i=end;i>=begin;i--)
#define fio ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0);

using namespace std;
using namespace std::chrono;

typedef pair<int,int> ii;

#define MAXN 102

struct wayNode{
	int dway;
	vector<int> way;
};

struct comp{
	bool operator()(const wayNode (&a), const wayNode (&b)){
		return b.dway < a.dway;
		//return a.dway < b.dway;
	}
}; 

const int INF = 1000000009;
int n, m, k;
int d[MAXN], par[MAXN], sz[MAXN];
int pos[MAXN][MAXN], w[MAXN][MAXN];
vector<ii> g[MAXN];
vector<wayNode> A;
priority_queue<wayNode, vector<wayNode>, comp> B;
//multiset<wayNode> B;

void Dijkstra(int s){
	rep(i,1,n){
		if(d[i] != -1) d[i] = INF;
		par[i] = -1;
	} 

	d[s] = 0;

	set<ii> q;
	q.insert({d[s],s});

	int cur, to, weight;

	while(!q.empty()){
		cur = q.begin()->second;
		q.erase(q.begin());

		rep(i,0,(int)g[cur].size() - 1){
			to = g[cur][i].first;
			weight = g[cur][i].second;

			if(weight != -1 && d[to] != -1 && d[cur] + weight < d[to]){
				q.erase({d[to],to});
				d[to] = d[cur] + weight;
				par[to] = cur;
				q.insert({d[to],to});
			}
		}
	}
}

wayNode getWay(int spur){
	wayNode myWay;
	myWay.dway = 0;

	int crawl = n;
	myWay.way.push_back(crawl);

	while(par[crawl] != -1){
		myWay.way.push_back(par[crawl]);
		myWay.dway += w[par[crawl]][crawl];
		crawl = par[crawl];
	}

	reverse(myWay.way.begin(),myWay.way.end());

	return myWay;
}

wayNode unionWay(wayNode (&way1), wayNode (&way2)){
	wayNode Union;
	Union.dway = way1.dway + way2.dway;

	rep(i,0,(int)way1.way.size() - 1) Union.way.push_back(way1.way[i]);
	rep(i,1,(int)way2.way.size() - 1) Union.way.push_back(way2.way[i]);

	return Union;
}

bool equalWay(wayNode (&way1), wayNode (&way2)){
	if(way1.dway != way2.dway || (int)way1.way.size() != (int)way2.way.size()) return false;

	rep(i,0,(int)way1.way.size() - 1){
		if(way1.way[i] != way2.way[i]) return false;
	}

	return true;
}

int spurBegin(wayNode (&way1), wayNode (&way2)){
	int ind = 0;
	int minSz = min((int)way1.way.size(),(int)way2.way.size());

	while(ind < minSz && way1.way[ind] == way2.way[ind]) ind++;

	return ind-1;
}

wayNode getRoot(wayNode (&myWay), int spurInd){
	wayNode root;
	root.dway = 0;

	for(int i=0;i<(int)myWay.way.size() - 1;i++){
		root.way.push_back(myWay.way[i]);

		if(i != 0) root.dway += w[myWay.way[i-1]][myWay.way[i]];
		if(i == spurInd) break;
	}

	return root;
}

int main(){
fio

cin >> n >> m >> k;

int aux;

rep(i,1,2*k+n) cin >> aux;

int u, v, weight;

rep(i,1,m){
	cin >> u >> v >> weight >> aux;
	g[u].push_back({v,weight});
	sz[u]++;
	pos[u][v] = sz[u] - 1;
	w[u][v] = weight;
}

auto start = high_resolution_clock::now();

Dijkstra(1);

if(d[n] != INF) A.push_back(getWay(1));

int sz = 0;

while(true){
	sz++;

	int spurB = 0, spurNode;

	///Lawler's modification
	if(sz != 1) spurB = spurBegin(A[sz-2],A[sz-1]);
	///For each spur node
	rep(i,spurB,(int)A[sz-1].way.size() - 2){
		spurNode = A[sz-1].way[i];
		wayNode rootPath = getRoot(A[sz-1],i);

		rep(j,0,sz-1){
			wayNode pPath = getRoot(A[j],i);

			///Turn off the possibility of picking a way already computed
			if(equalWay(rootPath,pPath)) g[A[j].way[i]][pos[A[j].way[i]][A[j].way[i+1]]].second = -1;
		}

		///Turn off the vertex in the spur way
		rep(j,0,(int)rootPath.way.size() - 2) d[rootPath.way[j]] = -1;

		///Gets the smallest way from the spurNode
		Dijkstra(spurNode);

		///Puts the candidate in the heap
		if(d[n] != INF){
			wayNode spurPath = getWay(spurNode);
		    wayNode totalPath = unionWay(rootPath,spurPath);

		    B.push(totalPath);  
		    //B.insert(totalPath);
		}

		///Restoring edges and vertex
		rep(j,0,sz-1){
			wayNode pPath = getRoot(A[j],i);

			if(equalWay(rootPath,pPath)) g[A[j].way[i]][pos[A[j].way[i]][A[j].way[i+1]]].second = w[A[j].way[i]][A[j].way[i+1]];
		}

		rep(j,0,(int)rootPath.way.size() - 2) d[rootPath.way[j]] = INF;
	}

	if(B.empty()) break;

	wayNode temp = B.top();

	bool notEqual = true;
	int crawl = sz-1;

	while(crawl > 0 && A[crawl].dway == temp.dway){
		if(equalWay(A[crawl],temp)) notEqual = false;
		crawl--;
	}

	//if(!equalWay(A[sz-1],temp)) A.push_back(B.top());
	if(notEqual) A.push_back(B.top()); 
	else sz--;
 
	B.pop();
	//A.push_back(*B.begin());
	//B.erase(*B.begin());

	if(sz == 100) break;
} 

auto stop = high_resolution_clock::now();
auto duration = duration_cast<microseconds>(stop - start); 

cout << "Time taken by function: "
         << duration.count()/1000000.0 << "  seconds" << endl;

rep(i,0,sz-1){
	cout << A[i].dway << endl;
	rep(j,0,(int)A[i].way.size() - 1) cout << A[i].way[j] << " ";
	cout << endl;
}
return 0;
}