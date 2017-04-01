#include "deploy.h"
#include "lib_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <limits>
#include <chrono>
#include <random>
#include <algorithm>
#include <iterator>
#include <fstream>
#include "util.h"
#include "MCMF.h"
#include "graph.h"
using namespace std;

#define SETZR(a) memset(a, 0, sizeof(a))
#define INF std::numeric_limits<int>::max()

std::chrono::time_point<std::chrono::system_clock> start, tend;
std::chrono::duration<double> elapsed_seconds;

void PrintPath(const vector<int> &nodes_on_path)
{
	for (auto &index : nodes_on_path)
	{
		cout << index << " ";
	}
	cout << endl;
}
int node_num, link_num, customer_num, server_price;

//你要完成的功能总入口
void deploy_server(char *topo[MAX_EDGE_NUM], int line_num, char *filename)
{

	start = std::chrono::system_clock::now();

	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");
	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	server_price = std::atoi(topo[2]);


	// customers
	vector<Customer> customers(customer_num);
	map<int, int> agency_map;
	vector<int> agencies;
	char **base_pos = topo + 4 + link_num + 1;
	vector<string> numbers;
	map<int,int> cust_demand;//代理点需求大小
	for (int i = 0; i < customer_num; i++)
	{
		string line(base_pos[i]);
		numbers = str_split(line, " ");
		customers[i].index = std::stoi(numbers[0]);
		customers[i].agency = std::stoi(numbers[1]);
		customers[i].demand = std::stoi(numbers[2]);
		agency_map[customers[i].agency] = customers[i].index;
		agencies.push_back(customers[i].agency);
		cust_demand[customers[i].agency] = customers[i].demand;
	}

	int base_price = customer_num * server_price;
	cout << "number of nodes:\t" << node_num << endl;
	cout << "number of links:\t" << link_num << endl;
	cout << "number of customers:\t" << customer_num << endl;
	cout << "cost of a server:\t" << server_price << endl;
	cout << "base_price:\t" << base_price << endl;

	srand(time(NULL));
	bool found_all = false;
	vector<int> servers;

	vector<vector<int>> best_paths_all;
	map<int, int> valid_server;
	map<int,vector<int>> valid_node;
	vector<vector<int>> paths;
	int  m, n, s, t, require_demand=0, customer_demand = 0,transfer_cost, cost = 0;
	Graph g(node_num);
	g.CreateFromBuf(topo + 4, link_num);
	cout<<"************************"<<endl;

	//代理点需求从小到大排列
	vector<pair<int, int>> cust_demand_vec(cust_demand.begin(), cust_demand.end());
	sort(cust_demand_vec.begin(), cust_demand_vec.end(), CmpByValue());
	cout<<"cust_demand"<<endl;
	for (int i = 0; i != cust_demand_vec.size(); ++i) {
		cout << cust_demand_vec[i].first<<" "<<cust_demand_vec[i].second<< endl;
	}
	//构建图，找初始点
	cl = dataLoad(edge,flow, topo,line_num,n,s,t,pointer,cl, customer_demand);
	int before_cl = cl;
	cout<<"customer_demand"<<customer_demand<<endl;
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
//	serverLoad(edge ,iter_flow, servers, s, iter_pointer, cl);
	require_demand = findCost_v2(cust_demand_vec, edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
	if (require_demand!=0){
		cout<<"找不到路径"<<endl;
		found_all = false;
		string output = pathString(best_paths_all, agency_map, found_all);
		char *topo_file = new char[output.size() + 1];
		std::copy(output.begin(), output.end(), topo_file);
		topo_file[output.size()] = '\0'; // don't forget the terminating 0
		//直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
		write_result(topo_file, filename);
		delete[] topo_file;
		return;
	}
	cost = transfer_cost+ server_price * valid_server.size();
	cout<<"before cost:"<<cost<<endl;
	for(map<int,int>::iterator it = valid_server.begin(); it!=valid_server.end();it++){
		servers.push_back(it->first);
	}
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
	findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
	cost = transfer_cost+ server_price * valid_server.size();
	cout<<"transfer"<<transfer_cost<<endl;
	cout<<"after cost:"<<cost<<endl;
	cout<<"best_cost"<<cost<<endl;
	cout<<"valid_server"<<valid_server.size()<<endl;
	found_all = true;




	//计算每个节点的传输费用（辅助）
	//利用agency_transfer_cost， 找出传输费用大于部署费用的点
	map<int,int> agency_transfer_cost;
	int path_agency=0;
	int path_flow = 0;
	int path_cost = 0;
	float rate = 1.2;

	for(int i=0;i<paths.size();i++){
		path_agency = paths[i][1];
		path_flow = paths[i][0];
		path_cost = 0;
		for(int j = paths[i].size()-1;j>1;j--){
			path_cost+=g.GetEdgeWithIndex(paths[i][j],paths[i][j-1])->unit_cost;
		}
		if(agency_transfer_cost.find(path_agency)==agency_transfer_cost.end()){
			agency_transfer_cost[path_agency] = path_cost*path_flow;
		}
		else{
			agency_transfer_cost[path_agency]+=path_cost*path_flow;
		}
	}

	for(map<int,int>::iterator it = agency_transfer_cost.begin(); it!=agency_transfer_cost.end();it++){
		if(it->second>server_price*rate){
			servers.push_back(it->first);
		}
	}
	//重新计算最小传输费用
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
	findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
	cost = transfer_cost+ server_price * valid_server.size();
	cout<<"transfer"<<transfer_cost<<endl;
	cout<<"final cost:"<<cost<<endl;
	cout<<"valid_server"<<valid_server.size()<<endl;

	//servers是引用
	optimizer1(paths, valid_server, valid_node , cost, cust_demand, transfer_cost, customer_demand, servers, n , s, t, before_cl);
	cost = transfer_cost+ server_price * valid_server.size();
	cout<<"transfer"<<transfer_cost<<endl;
	cout<<"final1 cost:"<<cost<<endl;
	cout<<"valid_server"<<valid_server.size()<<endl;

	//测试加入某些服务器效果会比变好
	vector<int> in = {5};
	vector<int> out ={};
	for(int i = 0; i <in.size();i++)
		servers.push_back(in[i]);
	for(int i = 0;i<servers.size();i++){
		for(int j =0;j<out.size();j++){
			if(servers[i] ==out[j]){
				cout<<"remove"<<out[j]<<endl;
				servers.erase(servers.begin()+i);
				out.erase(out.begin()+j);
				i = i-1;
				j = j-1;
			}
		}
	}
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));

	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
	findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
	optimizer1(paths, valid_server, valid_node , cost, cust_demand, transfer_cost, customer_demand, servers, n , s, t, before_cl);
	cost = transfer_cost+ server_price * valid_server.size();
	cout<<"transfer"<<transfer_cost<<endl;
	cout<<"final2 cost:"<<cost<<endl;
	cout<<"valid_server"<<valid_server.size()<<endl;


	//写入cost信息
//	char *filePath = "/home/full/Desktop/cost.txt";
//	char cost_char[20];
//	sprintf(cost_char,"%d",cost);
//	write_cost(filePath, cost_char);

//	//写入服务器信息
//	char *filePath1 = "/home/full/Desktop/servers1.txt";
//	for(map<int,int>::iterator it = valid_server.begin();it!=valid_server.end();it++){
//		char cost_char[20];
//		sprintf(cost_char,"%d",it->first);
//		write_cost(filePath1, cost_char);
//
//		sprintf(cost_char,"%d",it->second);
//		write_cost(filePath1, cost_char);
//	}

	best_paths_all = paths;
	string output = pathString(best_paths_all, agency_map, found_all);
	char *topo_file = new char[output.size() + 1];
	std::copy(output.begin(), output.end(), topo_file);
	topo_file[output.size()] = '\0'; // don't forget the terminating 0

	//直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
	delete[] topo_file;
}

//去除multi_server的情况
//	map<int,int> srevers_map;
//	for(map<int,int>::iterator it = valid_server.begin(); it!=valid_server.end(); it++){
//		srevers_map[it->first] = 0;
//	}
//	for(int i = 0;i<paths.size();i++){
//		vector<int> tmp_path;
//		bool path_flag = false;
//		for(int j = paths[i].size()-1;j>0;j--){
//			if(j<paths[i].size()-1 && srevers_map.find(paths[i][j])!=srevers_map.end()){
//				path_flag = true;
//				cout<<"***********************multi_server!**********************"<<endl;
//				PrintPath(paths[i]);
//				tmp_path.clear();
//				tmp_path.push_back(paths[i][j]);
//			}
//		}
//		if(path_flag==true){
//			paths[i] = tmp_path;
//		}
//	}

//	//去除不往外传输流量的代理店
//	int alone_agency_cost = 0;//删除后流量的大小
//	map<int,int> valid_server_alone = valid_server;
//	int alone_demand = 0,alone_cost = cost;
//
//	for(map<int,int>::iterator it = valid_server_alone.begin(); it!=valid_server_alone.end(); it++){
//		if(it->second==cust_demand.find(it->first)->second){
//			//代理店只给自己输送流量
//			for(int i = 0;i<servers.size();i++){
//				if(servers[i]==it->first){
//					//删除该服务器
//					cout<<"erase:"<<it->first<<endl;
//					servers.erase(servers.begin()+i);
////					PrintPath(servers);
//					cout<<"server size"<<servers.size()<<endl;
//				}
//			}
//			memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
//			memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
//			serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
//			alone_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
//			if(alone_demand!=0){
//				cout<<"important node!"<<it->first<<endl;
//				servers.push_back(it->first);
//				continue;
//			}
//			cout<<"server size"<<servers.size()<<endl;
//			alone_agency_cost = transfer_cost+server_price*valid_server.size();
//			if(alone_agency_cost > alone_cost){
//				servers.push_back(it->first);
//			}
//			else{
//				cout<<"remove: "<<it->first<<", reduce cost:"<<alone_cost-alone_agency_cost<<endl;
//				alone_cost = alone_agency_cost;
//			}
//		}
//	}
//
//
//
//	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
//	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
//	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
//	alone_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);

//vector<int> servers1 = {11,17,20,22,30,36,37,46,49,56,57,60,61,66,72,77,84,89,97,103,106,114,123,126,128,135,137,147,155,159,81,85,108,4,25,54,69,131};
//memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
//memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
//
//serverLoad(edge ,iter_flow, servers1, s, iter_pointer, before_cl);
//findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
//cout<<"cost1"<<transfer_cost+ server_price * valid_server.size()<<endl;
//
//vector<int> servers2 = {11,17,20,22,30,36,37,46,49,56,57,60,61,66,72,77,84,89,97,103,106,114,123,126,128,135,137,147,155,159,4,25,54,69,81,85,108,131};
//memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
//memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
//
//serverLoad(edge ,iter_flow, servers2, s, iter_pointer, before_cl);
//findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
//cout<<"cost2"<<transfer_cost+ server_price * valid_server.size()<<endl;
