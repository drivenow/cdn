/*
 * MCMF.cpp

 *
 *  Created on: Mar 17, 2017
 *      Author: full
 */

#include "util.h"
#include <iostream>
#include <queue>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
#include <limits>
#include <random>
using namespace std;

extern const int maxn = 10000;
extern const int maxm = 1000000;
extern const int INF = 1000000000;

struct record
{
	int v, f, c, next;
} edge[maxm];

void connect(int a, int b, int f, int c, int &cl, int *pointer)
{
	cl++;
	edge[cl].next = pointer[a];
	edge[cl].v = b;
	edge[cl].f = f;
	edge[cl].c = c;
	pointer[a] = cl;
	cl++;
	edge[cl].next = pointer[b];
	edge[cl].v = a;
	edge[cl].f = 0;
	edge[cl].c = -c;
	pointer[b] = cl;
}

/*
 * return: customer_demand客户需求
 * n: 节点数（包括源和汇）
 * m： 总边数
 * s: 源点
 * t： 汇点
 * pointer[maxn]: 记录边信息
 * */
int dataLoad(char **topo, vector<int> servers, int line_num, int &n, int &m, int &s, int &t, int *pointer)
{
	int cl = 1;
	int customer_demand = 0;

	string line;
	vector<string> n_strs;

	line = string(topo[0]);
	//	cout << line << endl;
	n_strs = str_split(line, " ");
	int node_num = stoi(n_strs[0]);
	int edge_num = stoi(n_strs[1]);
	int cust_num = stoi(n_strs[2]);
	n = node_num + 2;
	m = edge_num * 2 + (cust_num + servers.size());
	s = node_num;
	t = node_num + 1;

	line = topo[2];
	n_strs = str_split(line, " ");
	// int s_cost = stoi(n_strs[0]);

	int lineno = 4;
	for (; (line = topo[lineno]) != "\n"; lineno++)
	{

				if (line.empty()){
					cout<<"empty";
					break;
				}
				if(line=="\r\n"){
					cout<<"1";
					break;
				}
				if(line=="\r"){
					cout<<"2";
					break;
				}
				if(line=="\n"){
					cout<<"3";
					break;
				}
		line = topo[lineno];
		n_strs = str_split(line, " ");
		int p, k, f, c;
		p = stoi(n_strs[0]);
		k = stoi(n_strs[1]);
		f = stoi(n_strs[2]);
		c = stoi(n_strs[3]);
		connect(p, k, f, c, cl, pointer);
		connect(k, p, f, c, cl, pointer);
	}

	lineno++;
	for (line = topo[lineno]; lineno != line_num; lineno++)
	{
		line = topo[lineno];
		n_strs = str_split(line, " ");
		int p, k, f, c;
		p = stoi(n_strs[1]);
		k = t;
		f = stoi(n_strs[2]);
		c = 0;
		customer_demand += f;
		connect(p, k, f, c, cl, pointer);
	}

	for (unsigned i = 0; i < servers.size(); i++)
	{
		int p, k, f, c;
		p = s;
		k = servers[i];
		f = INF;
		c = 0;
		connect(p, k, f, c, cl, pointer);
	}

	return customer_demand;
}

int dist[maxn], pre[maxn];
bool vis[maxn];
//用于优先队列比较
struct cmp
{
	bool operator()(int a, int b)
	{
		return dist[a] > dist[b];
	}
};
priority_queue<int, vector<int>, cmp> q;

/*
 * ans: dist
 *
 * */
bool spfa(vector<int> &path, int &ans, int *pointer, int n, int s, int t, map<int, int> &valid_server)
{
	int aug, k, p;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < n; i++)
		dist[i] = INF;

	dist[s] = 0;
	pre[s] = 0;
	q.push(s);

	while (!q.empty())
	{
		k = q.top();
		q.pop();
		vis[k] = 0;
		for (p = pointer[k]; p; p = edge[p].next)
			if ((edge[p].f > 0) && (edge[p].c + dist[k] < dist[edge[p].v]))
			{
				dist[edge[p].v] = edge[p].c + dist[k];
				pre[edge[p].v] = p;
				if (!vis[edge[p].v])
				{
					vis[edge[p].v] = 1;
					q.push(edge[p].v);
				}
			}
	}
	if (dist[t] == INF)
		return false;
	aug = INF;
	for (p = pre[t]; p; p = pre[edge[p ^ 1].v])
		aug = min(aug, edge[p].f);

	int server;
	for (p = pre[t]; p; p = pre[edge[p ^ 1].v])
	{
		if (edge[p].v != t)
		{
			path.push_back(edge[p].v);
			server = edge[p].v;
		}
		else
			path.push_back(aug);
		edge[p].f -= aug;
		edge[p ^ 1].f += aug;
	}
	ans += dist[t] * aug;
	if (valid_server.find(server) != valid_server.end())
	{
		valid_server[server] += aug;
	}
	else
	{
		valid_server[server] = aug;
	}
	return true;
}

/*
 * return: 路径传输的代价
 * s: 源点
 * t： 汇点
 * vector<vector<int>> paths: 最短路径
 * int &pointer[maxn]
 * */
int findCost(vector<vector<int>> &paths, map<int, int> &valid_server, int *pointer, int n, int s, int t)
{
	int ans = 0; //传输费用
	int count = 1;
	bool flag = true;

	while (flag)
	{
		//cout << "spfa:" << count++ << endl;
		vector<int> path;
		flag = spfa(path, ans, pointer, n, s, t, valid_server);
		if (flag == true)
		{
			paths.push_back(path);
		}
	}
	return ans;
}
