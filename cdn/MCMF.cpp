/*
 * MCMF.cpp

 *
 *  Created on: Mar 17, 2017
 *      Author: full
 */
#include <iostream>
#include <queue>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
#include <limits>
#include <random>
#include <stdlib.h>
#include <algorithm>
#include "MCMF.h"
#include "util.h"
using std::string;
using std::stoi;
using namespace std;

const int MAXEDGE = 20000;
const int MAXNODE = 4000;

record edge[MAXEDGE];
int flow[MAXEDGE];
int pointer[MAXNODE];
int iter_flow[MAXEDGE];
int iter_pointer[MAXNODE];
int cl;

static int dist[MAXNODE];
static bool vis[MAXNODE];
static int pre[MAXNODE];
//用于优先队列比较
 struct cmp{
     bool operator()(int a,int b){
         return dist[a]>dist[b];}
 };
static priority_queue<int,vector<int>,cmp> q;


void connect(record* edge, int flow[],int a, int b, int f, int c,int &cl,int pointer[]){
    cl++;
    edge[cl].next = pointer[a];
    edge[cl].v = b;
    edge[cl].aug = f;
    flow[cl] = 0;
    edge[cl].c = c;
    pointer[a] = cl;
    cl++;
    edge[cl].next = pointer[b];
    edge[cl].v = a;
    edge[cl].aug = 0;
    flow[cl] = 0;
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
int dataLoad(record* edge,int flow[], char ** topo, int line_num, int &n ,int &s,int &t, int pointer[], int &cl, int &customer_demand) {
	//
	cl = 1;
	string line;
	vector<string> n_strs;

	line = string(topo[0]);
	n_strs = str_split(line, " ");
	int node_num = stoi(n_strs[0]);
	int edge_num = stoi(n_strs[1]);
	int cust_num = stoi(n_strs[2]);
	n = node_num+2;
	s = node_num;
	t = node_num+1;

	line = topo[2];
	n_strs = str_split(line, " ");
	int s_cost = stoi(n_strs[0]);

	int lineno = 4;
	for (;(line = topo[lineno])!="\n"; lineno++) {

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
//		cout<<lineno<<": "<<line;
		n_strs = str_split(line, " ");
		int p, k, f, c;
		p = stoi(n_strs[0]);
		k = stoi(n_strs[1]);
		f = stoi(n_strs[2]);
		c = stoi(n_strs[3]);
		connect(edge,flow, p, k, f, c, cl, pointer);
		connect(edge, flow, k, p, f, c,cl,pointer);
	}

	lineno++;
	for (line = topo[lineno]; lineno != line_num ; lineno++) {
		line = topo[lineno];
		n_strs = str_split(line, " ");
		int p, k, f, c;
		p = stoi(n_strs[1]);
		k = t;
		f = stoi(n_strs[2]);
		c = 0;
		customer_demand += f;
		connect(edge,flow,p, k, f, c,cl,pointer);
	}

	return cl;
}

/*
 * pointer不是引用，每次调用函数返回新的pointer
 * */
void serverLoad(record* edge, int flow[], vector<int> servers, int s, int  pointer[], int cl){
	//参数重置
	for(auto i=0;i<servers.size();i++){
		int p, k, f, c;
		p = s;
		k = servers[i];
		f = INF;
		c = 0;
		connect(edge, flow , p, k, f, c,cl,pointer);
//		cout<<"cl"<<cl<<endl;
	}
}

void serverLoadOne(record* edge, int flow[], int server, int s, int  pointer[], int cl){
	//参数重置
		int p, k, f, c;
		p = s;
		k = server;
		f = INF;
		c = 0;
		connect(edge, flow , p, k, f, c,cl,pointer);
//		cout<<"cl"<<cl<<endl
}

/*
 * ans: dist
 *
 * */

bool spfa(record* edge, int flow[], vector<int> & path, int &ans, int pointer[], int n, int s,int t,map<int,int> &valid_server, map<int,vector<int>> &valid_node) {
	int  aug, k, p;
    memset(vis, 0, sizeof (vis));
    for (int i = 0; i < n; i++) dist[i] = INF;

    dist[s] = 0;
    pre[s] = 0;
    q.push(s);

    while (!q.empty())
    	{
    		k = q.top();
    		q.pop();
    		vis[k] = 0;
    		for (p = pointer[k]; p; p = edge[p].next)
    			if ((edge[p].aug-flow[p]) > 0 && (edge[p].c + dist[k] < dist[edge[p].v]))
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
    for (p=pre[t]; p; p=pre[edge[p^1].v])
        aug = min(aug, edge[p].aug - flow[p]);

    int server,agency=0;
    int agency_idx = 0;
    for (p=pre[t]; p; p=pre[edge[p^1].v]) {
    	if (edge[p].v!=t){
    		path.push_back(edge[p].v);
    		server = edge[p].v;
    		if(agency_idx==0){
    			agency = edge[p].v;
    		}
    		agency_idx++;
    	}
    	else
    		path.push_back(aug);
        flow[p] += aug;
        flow[p^1] -= aug;
    }
    ans += dist[t] * aug;
    if (valid_server.find(server)!=valid_server.end()){
    	valid_server[server]+=aug;
    }
    else{
    	valid_server[server] = aug;
    }

    if (valid_node.find(server)!=valid_node.end()){
    	vector<int> nodes;

    	nodes.push_back(agency);
    	valid_node[server] = nodes;
    }
    else{
    	valid_node[server].push_back(agency);
    }

    return true;
}


bool spfa_v3(int &customer_demand, vector <bool> &nodes_viz, vector<pair<int, int>> &cust_demand_vec, record* edge, int flow[], vector<int> & path, int &ans, int pointer[], int n, int s,int t,map<int,int> &valid_server, map<int,vector<int>> &valid_node, int count) {
		int  aug, k, p;
		int transfer_cost = 0;
		int locate_cost = 0;
		memset(vis, 0, sizeof (vis));
		for (int i = 0; i < n; i++) dist[i] = INF;

		dist[s] = 0;
		pre[s] = 0;
		q.push(s);
		int count2 = 0;

		while (!q.empty())
			{
				k = q.top();
				q.pop();
				vis[k] = 0;

				for (p = pointer[k]; p; p = edge[p].next){
					if ((edge[p].aug-flow[p]) > 0 && (edge[p].c + dist[k] < dist[edge[p].v]))
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
			}
		if (dist[t] == INF){
			//需求不满足，开辟代理点
			cout<<"demand not meet"<<endl;
			return false;
		}
		 //找出server，agency，传输aug
		aug = INF;//传输流量
		for (p=pre[t]; p; p=pre[edge[p^1].v])
			aug = min(aug, (edge[p].aug-flow[p]));


		int server,agency=0;
		int agency_idx = 0;
		for (p=pre[t]; p; p=pre[edge[p^1].v]) {
			if (edge[p].v!=t){
				server = edge[p].v;
				if(agency_idx==0){
					agency = edge[p].v;
				}
				agency_idx++;
			}
		}

		ans += dist[t] * aug;//总的传输费用
		//选择改传输方式，更新流量和路径表
		int agency_idx_sub = 0;
		for (p=pre[t]; p; p=pre[edge[p^1].v]) {
			if (edge[p].v!=t){
				path.push_back(edge[p].v);
				server = edge[p].v;
				if(agency_idx_sub==0){
					agency = edge[p].v;
				}
				agency_idx_sub++;
			}
			else{
				path.push_back(aug);
			}
			flow[p] += aug;
			flow[p^1] -= aug;
		}

		if (valid_server.find(server)!=valid_server.end()){
			valid_server[server]+=aug;
		}
		else{
			valid_server[server] = aug;
		}
		//更新代理店需求，重新排列需求表
		for(int i = 0;i<cust_demand_vec.size();i++){
			if(cust_demand_vec[i].first==agency){
				cust_demand_vec[i].second -=aug;
			}
		}
		sort(cust_demand_vec.begin(), cust_demand_vec.end(), CmpByValue());
		customer_demand-=aug;
//        cout<<"from "<<server<<" to customer "<<agency<<" transfer "<<aug<<endl;
		return true;
}

bool spfa_v2(vector<int> &cust_transfer_vec, int &customer_demand, vector <bool> &nodes_viz, vector<pair<int, int>> &cust_demand_vec, record* edge, int flow[], vector<int> & path, int &ans, int pointer[], int n, int s,int t,map<int,int> &valid_server, map<int,vector<int>> &valid_node, int count) {
	if(node_num<=700){
		int  aug, k, p;
		int transfer_cost = 0;
		int locate_cost = 0;
		memset(vis, 0, sizeof (vis));
		for (int i = 0; i < n; i++) dist[i] = INF;

		dist[s] = 0;
		pre[s] = 0;
		q.push(s);
	//    cout<<1<<endl;
		int count2 = 0;

		while (!q.empty())
			{
				k = q.top();
				q.pop();
				vis[k] = 0;

				for (p = pointer[k]; p; p = edge[p].next){
	//    			cout<<"p:"<<p<<endl;
					if ((edge[p].aug-flow[p]) > 0 && (edge[p].c + dist[k] < dist[edge[p].v]))
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
			}
	//    cout<<2<<" "<<dist[t]<<endl;
		if (dist[t] == INF){
			//需求不满足，开辟代理点
			cout<<"demand not meet"<<endl;
			if(nodes_viz[cust_demand_vec[0].first]==1){
				cout<<"repalce server error"<<endl;
			}

			nodes_viz[cust_demand_vec[0].first] = 1;
			serverLoadOne(edge ,flow, cust_demand_vec[0].first, s, pointer, cl);
			cl+=2;
	//		cout<<"v"<<edge[295].v<<" "<<flow[295]<<" "<<edge[295].c<<endl;
			cout<<"add server"<<cust_demand_vec[0].first<<endl;
			if(nodes_viz[cust_demand_vec[0].second]==0){
							cout<<"demand meet"<<endl;
							return true;
						}
			return false;
		}
		 //找出server，agency，传输aug
		aug = INF;//传输流量
		for (p=pre[t]; p; p=pre[edge[p^1].v])
			aug = min(aug, (edge[p].aug-flow[p]));


		int server,agency=0;
		int agency_idx = 0;
		for (p=pre[t]; p; p=pre[edge[p^1].v]) {
			if (edge[p].v!=t){
				server = edge[p].v;
				if(agency_idx==0){
					agency = edge[p].v;
				}
				agency_idx++;
			}
		}
	//    cout<<"maybe from "<<server<<" to customer "<<agency<<" transfer "<<aug<<endl;
		//
		transfer_cost = cust_transfer_vec[agency]+dist[t] * aug;//当前的传输费用，加上新传输费用
	//    transfer_cost = dist[t] * aug;
		locate_cost = server_price;//部署费用
		if(1.2*locate_cost < transfer_cost){
			//路径终点加入服务器列表
			cout<<"high tranfer cost"<<endl;
	//    	nodes_viz[cust_demand_vec[0].first] = 1;
	//    	serverLoadOne(edge ,flow, cust_demand_vec[0].first, s, pointer, cl);
			nodes_viz[agency] = 1;
			serverLoadOne(edge ,flow, agency, s, pointer, cl);
			cl+=2;
	//		cout<<"add server"<<cust_demand_vec[0].first<<endl;
			return false;
		}
		else{
			ans += dist[t] * aug;//总的传输费用
			cust_transfer_vec[agency] += dist[t] * aug;//传送到customer的费用
			//选择改传输方式，更新流量和路径表
			int agency_idx_sub = 0;
			for (p=pre[t]; p; p=pre[edge[p^1].v]) {
				if (edge[p].v!=t){
					path.push_back(edge[p].v);
					server = edge[p].v;
					if(agency_idx_sub==0){
						agency = edge[p].v;
					}
					agency_idx_sub++;
				}
				else{
					path.push_back(aug);
				}
				flow[p] += aug;
//				flow[p^1] -= aug;
			}

			if (valid_server.find(server)!=valid_server.end()){
				valid_server[server]+=aug;
			}
			else{
				valid_server[server] = aug;
			}
			//更新代理店需求，重新排列需求表
			for(int i = 0;i<cust_demand_vec.size();i++){
				if(cust_demand_vec[i].first==agency){
					cust_demand_vec[i].second -=aug;
				}
			}
			sort(cust_demand_vec.begin(), cust_demand_vec.end(), CmpByValue());
			customer_demand-=aug;
	//        cout<<"from "<<server<<" to customer "<<agency<<" transfer "<<aug<<endl;
			return true;
		}
	}
	else{
		//高级用例
		int  aug, k, p;
		int transfer_cost = 0;
		int locate_cost = 0;
		memset(vis, 0, sizeof (vis));
		for (int i = 0; i < n; i++) dist[i] = INF;

		dist[s] = 0;
		pre[s] = 0;
		q.push(s);
		cout<<1<<endl;
		int count2 = 0;

		while (!q.empty())
			{
				k = q.top();
				q.pop();
				vis[k] = 0;

				for (p = pointer[k]; p; p = edge[p].next){
	//    			cout<<"p:"<<p<<endl;
					if ((edge[p].aug-flow[p])  > 0 && (edge[p].c + dist[k] < dist[edge[p].v]))
					{
						dist[edge[p].v] = edge[p].c + dist[k];
						if(count==11&&count2<1){
	//    				cout<<"v"<<edge[p].v<<" flow[p] "<<p<<" "<<flow[p]<<" dist:"<<dist[edge[p].v]<<endl;
						count2++;
						}
						pre[edge[p].v] = p;
						if (!vis[edge[p].v])
						{
							vis[edge[p].v] = 1;
							q.push(edge[p].v);
						}
					}
				}
			}
		cout<<2<<" "<<dist[t]<<endl;
		if (dist[t] == INF){
			//需求不满足，开辟代理点
			cout<<"demand not meet"<<endl;
			if(nodes_viz[cust_demand_vec[0].first]==1){
				cout<<"repalce server error"<<endl;
			}

			nodes_viz[cust_demand_vec[0].first] = 1;
			serverLoadOne(edge ,flow, cust_demand_vec[0].first, s, pointer, cl);
			cl+=2;
	//		cout<<"v"<<edge[295].v<<" "<<flow[295]<<" "<<edge[295].c<<endl;
			cout<<"add server"<<cust_demand_vec[0].first<<endl;
			if(nodes_viz[cust_demand_vec[0].second]==0){
							cout<<"demand meet"<<endl;
							return true;
						}
			return false;
		}
		 //找出server，agency，传输aug
		aug = INF;//传输流量
		for (p=pre[t]; p; p=pre[edge[p^1].v])
			aug = min(aug, (edge[p].aug-flow[p]));


		int server,agency=0;
		int agency_idx = 0;
		for (p=pre[t]; p; p=pre[edge[p^1].v]) {
			if (edge[p].v!=t){
				server = edge[p].v;
				if(agency_idx==0){
					agency = edge[p].v;
				}
				agency_idx++;
			}
		}
		cout<<"maybe from "<<server<<" to customer "<<agency<<" transfer "<<aug<<endl;
		//
		transfer_cost = cust_transfer_vec[agency]+dist[t] * aug;//当前的传输费用，加上新传输费用
	//    transfer_cost = dist[t] * aug;
		locate_cost = server_price;//部署费用
		if(locate_cost < 1.2*transfer_cost){
			//路径终点加入服务器列表
			cout<<"high tranfer cost"<<endl;
	//    	nodes_viz[cust_demand_vec[0].first] = 1;
	//    	serverLoadOne(edge ,flow, cust_demand_vec[0].first, s, pointer, cl);
			nodes_viz[agency] = 1;
			serverLoadOne(edge ,flow, agency, s, pointer, cl);
			cl+=2;
			cout<<"add server"<<agency<<endl;
			return false;
		}
		else{
			ans += dist[t] * aug;//总的传输费用
			cust_transfer_vec[agency] += dist[t] * aug;//传送到customer的费用
			//选择改传输方式，更新流量和路径表
			int agency_idx_sub = 0;
			for (p=pre[t]; p; p=pre[edge[p^1].v]) {
				if (edge[p].v!=t){
					path.push_back(edge[p].v);
					server = edge[p].v;
					if(agency_idx_sub==0){
						agency = edge[p].v;
					}
					agency_idx_sub++;
				}
				else
					path.push_back(aug);
				flow[p] += aug;
	//            flow[p^1] += aug;
			}

			if (valid_server.find(server)!=valid_server.end()){
				valid_server[server]+=aug;
			}
			else{
				valid_server[server] = aug;
			}
			//更新代理店需求，重新排列需求表
			for(int i = 0;i<cust_demand_vec.size();i++){
				if(cust_demand_vec[i].first==agency){
					cust_demand_vec[i].second -=aug;
				}
			}
			sort(cust_demand_vec.begin(), cust_demand_vec.end(), CmpByValue());
			customer_demand-=aug;
//			cout<<"from "<<server<<" to customer "<<agency<<" transfer "<<aug<<endl;
			return true;
		}
	}
}


/*
 * return: 路径传输的代价
 * s: 源点
 * t： 汇点
 * vector<vector<int>> paths: 最短路径
 * int &pointer[maxn]
 * */
int findCost_v2(vector<pair<int, int>> cust_demand_vec, record* edge, int flow[], vector<vector<int>> &paths, map<int,int> &valid_server, map<int,vector<int>> &valid_node, int pointer[], int customer_demand, int n, int s,int t,int &transfer_cost){
	//参数重置
	transfer_cost = 0;
	paths.clear();
	valid_server.clear();
	valid_node.clear();
	memset(pre, 0, sizeof (pre));

	int count = 0;
	bool flag = true;
	bool find_so = true;
	vector <bool> nodes_viz(node_num);//已经部署的服务器
	for(int i =0;i < nodes_viz.size();i++){
		nodes_viz[i] = 0;
	}

	//最大需求代理

	serverLoadOne(edge ,flow, cust_demand_vec[0].first, s, pointer, cl);
	cl+=2;
	nodes_viz[cust_demand_vec[0].first] = 1;

	vector<int> cust_transfer_vec(node_num);


	while (customer_demand!=0){
		vector<int> path;
//		cout<<"spfa:"<<count++<<"cl:"<<cl<<endl;
		flag = spfa_v2(cust_transfer_vec, customer_demand,nodes_viz,cust_demand_vec,edge, flow , path, transfer_cost, pointer,n,s,t,valid_server,valid_node,count);
		if (flag==true){
			paths.push_back(path);
		}
	}

	if (customer_demand!=0){
//		cout<<"inadquete:"<<customer_demand<<endl;
		cout<<"findCost no solution!!"<<endl;
		find_so = false;
	}
	else{
		cout<<"findCost solution"<<endl;
	}
//	printf("total cost: %d\n", ans);
	return customer_demand;
}
/*
 * return: 路径传输的代价
 * s: 源点
 * t： 汇点
 * vector<vector<int>> paths: 最短路径
 * int &pointer[maxn]
 * */
int findCost_v3(Graph &g,vector<pair<int, int>> cust_demand_vec, record* edge, int iter_flow[], vector<vector<int>> &paths, map<int,int> &valid_server, map<int,vector<int>> &valid_node, int iter_pointer[], int customer_demand, int n, int s,int t,  int &transfer_cost,int before_cl){
	//参数重置
	transfer_cost = 0;
	paths.clear();
	valid_server.clear();
	valid_node.clear();
	memset(pre, 0, sizeof (pre));

	int count = 0;
	bool flag = true;
	bool find_so = true;
	map<int,int> inTrans, outFlow;
	vector<pair<int, int>> cust_demand_vec_init = cust_demand_vec;
	vector<int> cust_transfer_vec(node_num);
	vector<int> servers;//服务器个数

	int customer_demand_init = customer_demand;
	vector <bool> nodes_viz(node_num);//已经部署的服务器
	for(int i =0;i < nodes_viz.size();i++){
		nodes_viz[i] = 0;
	}

	//最大需求代理
	serverLoadOne(edge ,iter_flow, cust_demand_vec[0].first, s, iter_pointer, before_cl);
	servers.push_back(cust_demand_vec[0].first);
	cl+=2;
	nodes_viz[cust_demand_vec[0].first] = 1;



	while (customer_demand!=0){
		while(flag==true){
			vector<int> path;
			flag = spfa_v3(customer_demand,nodes_viz,cust_demand_vec, edge, iter_flow , path, transfer_cost, iter_pointer,n,s,t,valid_server,valid_node,count);
			if (flag==true){
				paths.push_back(path);
			}
			else{
				if(customer_demand==0){
					cout<<"v3 finish!"<<endl;
					break;
				}
				findFlow(g, paths, inTrans, outFlow);
				vector<pair<int,int>> inTrans_vec(inTrans.begin(), inTrans.end());
				sort(inTrans_vec.begin(), inTrans_vec.end(), CmpByValue());
				float rate = 30;//衡量传输费用和消费者需求的比值
				int select = 0;
				//前百分之五十需求由代理点满足
				if (customer_demand_init*0.3 < customer_demand){
					//选择需求最大的服务器
					select = cust_demand_vec[0].first;
					cout<<"*";
				}
				else if (inTrans_vec[0].second/float(cust_demand_vec[0].second)>rate){
					//选择传输费用最大的中间点
					select = inTrans_vec[0].first;
					cout<<"#";
				}
				else{
					//选择需求最大的服务器
					select = cust_demand_vec[0].first;
					cout<<"*";
				}
				//加入服务器重新选路
				servers.push_back(select);
				cout<<"v3 valid:"<<valid_server.size()<<" add server"<<select<<" inTrans:"<<inTrans_vec[0].second<<" cust_demand:"<<cust_demand_vec[0].second<<"all_demand:"<<customer_demand<<endl;
				memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
				memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
				serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
				//参数重置
				flag = true;
				customer_demand = customer_demand_init;
				cust_demand_vec = cust_demand_vec_init;
				paths.clear();
				inTrans.clear();
			}
		}
	}

	if (customer_demand!=0){
//		cout<<"inadquete:"<<customer_demand<<endl;
		cout<<"findCost no solution!!"<<endl;
		find_so = false;
	}
	else{
		cout<<"findCost solution"<<endl;
	}
//	printf("total cost: %d\n", ans);
	return customer_demand;
}


int findCost(record* edge, int iter_flow[], vector<vector<int>> &paths, map<int,int> &valid_server, map<int,vector<int>> &valid_node, int pointer[], int customer_demand, int n, int s,int t,int &transfer_cost){
	//参数重置
	transfer_cost = 0;
	paths.clear();
	valid_server.clear();
	valid_node.clear();
	memset(pre, 0, sizeof (pre));

	int count = 0;
	bool flag = true;
	bool find_so = true;


	while (flag){
		vector<int> path;
//		cout<<"spfa:"<<count++<<"cl:"<<cl<<endl;
		flag = spfa(edge, iter_flow , path, transfer_cost, iter_pointer,n,s,t,valid_server,valid_node);
		if (flag==true){
			paths.push_back(path);
		}
	}
	for (auto i=0;i<paths.size();i++){
			customer_demand-=paths[i][0];
	}

	if (customer_demand!=0){
//		cout<<"inadquete:"<<customer_demand<<endl;
		cout<<"findCost no solution!!"<<endl;
		find_so = false;
	}
	else{
//		cout<<"findCost solution"<<endl;
	}
//	printf("total cost: %d\n", ans);
	return customer_demand;
}

//获取最小流算法结束得到的路径
void GetPath(vector<vector<int>> &path, int superS,int superT, int iter_flow[], int pointer[]){
    path.clear();
    vector<int> nodelist;//运行栈
    vector<int> edgeID;//走过边的ID
    int flowMin,flowMax;
    nodelist.push_back(superS);
    bool isFind=false;
    while(!nodelist.empty()){
        isFind=false;
        int cur=nodelist.back();
         for(int i=pointer[cur];i!=0;i=edge[i].next){   //DFS
            if(iter_flow[i]>0){//走过
                isFind=true;
                nodelist.push_back(edge[i].v);
                edgeID.push_back(i);//记下边ID
                if(edge[i].v==superT){//递归最底层
                    int flow=1000000000;//无穷大
                    for(int k=0;k < edgeID.size();k++){//计算整条路径上的最小可行流
                        flow= flow<iter_flow[edgeID[k]]?flow:iter_flow[edgeID[k]];
                    }
                    for(int k=0;k<edgeID.size();k++){//计算整条路径上的最小可行流
                    	iter_flow[edgeID[k]]-=flow;
                    }
                    nodelist.push_back(flow);
                    path.push_back(nodelist);//记下路径
                    nodelist.pop_back();
                    //for(int k=0;k<nodelist.size();k++){//输出路径
                    //    printf("%d ",nodelist[k]);
                    //}
                    //printf("\n");
                    nodelist.pop_back();
                    edgeID.pop_back();
                    flowMin=1000000000;//无穷大
                    for(int k=0;k<edgeID.size();k++){//计算整条路径上的最小可行流
                        flowMin= flowMin<iter_flow[edgeID[k]]?flowMin:iter_flow[edgeID[k]];
                    }
                    if(flowMin==0){//路被切断从头开始搜
                        edgeID.clear();
                        nodelist.clear();
                        nodelist.push_back(superS);//从头开始
                        isFind=true;
                    }
                }else{
                    goto CON;//DFS
                }
                break;
            }
         }
         CON:
         if(!isFind){//没有新的可走节点
             nodelist.pop_back();
         }
    }
}



