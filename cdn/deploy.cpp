#include "deploy.h"
#include "lib_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <cmath>
#include <math.h>
#include <map>
#include <limits>
#include <time.h>
#include <algorithm>
#include "MCMF.h"
#include "GA.h"
#include "util.h"
using namespace std;

void print_vector(vector<bool> vec){
	for(int i=0;i<vec.size();i++){
		if(vec[i])
			cout<<i<<" ";
	}
	cout<<endl;
}


int groupnum = 30;			//种群中间力量大小
int gene_len;
float child_rate = 0.5;
float cross_rate = 0.3;
int community = groupnum*(1+child_rate)+int(groupnum*cross_rate)*2;
float nearby_rate = 0.5;		//产生祖先时，选择代理点附近点的权重
float maxiteration = 200;	//允许杂交的最大代数
vector<Unit> popula;
const int destroy_iter = 3;
int destroy_count_down = destroy_iter;
vector<int> comu_trans_cost(community);
vector<int> comu_server_cost(community);
int tc_mid=0,lc_mid = 0;
int punish = 0;

int m,n,s,t,line_num;
int customer_demand = 0;
int transfer_cost = 0;
int base_cost = 0;
int best_fit = 0;
int fit_cost = 0;
int fit_sum = 0;


vector<int> dif_nodes;
vector<int> total_nodes;
vector<int> alter_nodes;

int best_cost = 0;
int best_idx = 0;
vector <int> best_server;
vector<vector<int>> paths;
vector <vector<int>> best_path;
map <int, vector<float>> w_graph;
map<int,int>  valid_server;
map<int, vector<int>> valid_node;
map<int, vector<int>> valid_agency;
bool find_so_all = false, find_so = false;
int node_num, link_num, customer_num, server_unit_cost;
vector<int> agencys;



std::chrono::time_point<std::chrono::system_clock> start, tend;
std::chrono::duration<double> elapsed_seconds;


void PrintPath(const vector<int> & nodes_on_path)
{
	for (auto & index : nodes_on_path) {
		cout << index << " ";
	}
	cout << endl;
}

void PrintCustomer(const Customer & customer)
{
	cout << "index: "  << customer.index << endl;
	cout << "demand: " << customer.demand << endl;
}

void printFit(const vector<Unit> & popula,int indNum){
	cout<<"fit_ness:";
	for(int i=0;i<indNum;i++){
		cout<<popula[i].fn<<" ";
	}
	cout<<endl;
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
	start = std::chrono::system_clock::now();

	void print_vector(vector<bool> vec);

	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");

	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	server_unit_cost = std::atoi(topo[2]);

	base_cost = 4*server_unit_cost*customer_num;
	punish = server_unit_cost*customer_num;

	cout << "number of nodes:\t" << node_num << endl;
	cout << "number of links:\t" << link_num << endl;
	cout << "number of customers:\t" << customer_num << endl;
	cout << "cost of a server:\t" << server_unit_cost << endl;
	
	map<int, int> agency_map;
	char **base_pos = topo + 4 + link_num + 1;
	vector<string> numbers;

	for(int i = 0; i < customer_num; i++) {
		string line(base_pos[i]);
		numbers = str_split(line, " ");
		agency_map[std::stoi(numbers[1])] = std::stoi(numbers[0]);
		agencys.push_back(std::stoi(numbers[1]));
	}

//**************************************************************************************
//计算候选节点和它的补集,并将补集加入选址
	srand(time(0));
	cl = dataLoad(edge,flow, topo,line_num,n,s,t,pointer,cl, customer_demand);
	vector<vector<int>> custsNodes = allCustomerNodes(edge, pointer, cl, t, agency_map, 1);
	bool alter_viz[node_num];
	int alter_node;
	dif_nodes.resize(node_num);
	for(int i = 0;i<node_num;i++){
		alter_viz[i] = false;
	}
	for(int i=0;i<node_num;i++){
		total_nodes.push_back(i);
	}
	for(int i = 0;i < custsNodes.size();i++){
		for(int j =0;j<custsNodes[i].size()/2;j++){
			alter_node = custsNodes[i][j*2];
			if(alter_viz[alter_node]==false){
				alter_nodes.push_back(alter_node);
				alter_viz[alter_node] = true;
			}
		}
	}
	sort(alter_nodes.begin(),alter_nodes.end());
	vector<int>::iterator retEndPos = set_difference(total_nodes.begin(),total_nodes.end(),alter_nodes.begin(),alter_nodes.end(),dif_nodes.begin());
	dif_nodes.resize(retEndPos - dif_nodes.begin());

//	serverLoad(edge ,flow, dif_nodes, s, pointer, cl);
//	cl+=dif_nodes.size()*2;
//	cout<<"server_load"<<endl;
//************************************************
//	计算节点权重
	float dist_sum;
	map <int,int>::iterator amp_it = agency_map.begin();

	for(int i = 0; i < agencys.size(); i++){
		dist_sum = 0;
		for(int j = 0;j < custsNodes[i].size()/2;j++){
			dist_sum += custsNodes[i][2*j+1];
		}
		vector<float>  w_graph_ele;
		for(int j = 0;j < custsNodes[i].size()/2;j++){
			w_graph_ele.push_back(custsNodes[i][2*j]);
			w_graph_ele.push_back(custsNodes[i][2*j+1]/dist_sum+0.01);
		}
		w_graph[amp_it->first] = w_graph_ele;
		amp_it++;
	}

	//分配空间
	cout<<"community:"<<community<<endl;
	gene_len = node_num;

	for(int i = 0;i < community; i++){
		vector<bool> gn(node_num);
		for(int i = 0;i<node_num;i++)
			gn[i] = 0;
		map<int, vector<int>> valid_node;
		map<int, vector<int>> valid_agency;
		Unit unit = Unit(gn, valid_node, valid_agency);
		popula.push_back(unit);
	}

	//预处理产生候选祖先
	///home/full/Desktop/SDK-gcc-submit/case_example2/batch2/0/case0.txt out0.txt
	int search_len = int(customer_num*2);
	vector <bool> gene_ans(node_num);
	Unit tgene;
	int indNum = 0;

	for(int i = 0; i < groupnum; i++){
		find_so = false;
		int rseed = 0;
		while(!find_so){
			rseed--;
			gene_ans = creatGene(w_graph, search_len, node_num, nearby_rate, (i+1)*rseed);
//			print_vector(gene_ans);
			find_so = computeFit(gene_ans, paths, valid_server, valid_node, valid_agency, transfer_cost, fit_cost, server_unit_cost);
			cout<<1;
		}
//
		cout<<"gene_ans"<<endl;
		popula[i].gn[1]=1;
		for(map<int,int>::iterator it = valid_server.begin();it!=valid_server.end();it++)
			popula[i].gn[it->first] = 1;
	}
	//正式祖先
	for(int i = 0; i < groupnum; i++){
		find_so = computeFit(popula[i].gn, paths, valid_server, valid_node, valid_agency, transfer_cost, fit_cost, server_unit_cost);
		popula[i].tc = transfer_cost;
		popula[i].lc = server_unit_cost*valid_server.size();
		popula[i].vn = valid_node;
		popula[i].va = valid_agency;
		popula[i].fn = fit_cost;

		fit_sum += fit_cost;
		if(i==0){
			best_cost = fit_cost;
			best_idx = 0;
			best_path = paths;
		}
		if(find_so){
			find_so_all = true;
			if(fit_cost > best_fit){
				changeBest(best_fit,fit_cost,best_cost,popula[i].tc+ popula[i].lc, best_idx, indNum, best_path, paths);
			}
		}
		indNum++;
	}
	// 移动最优基因
	if(best_idx != 0)
	{
		cout<<"change"<<endl;
		exchangeUnit(popula[0],popula[best_idx]);
		best_idx = 0;
	}
	//种群传输费用和部署费用从小到大排序

	vector<int> server = {5,6,13,15,22,32,34,37,38};
//	find_so = computeFit(server, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);

	for(int iter = 2; iter < maxiteration; iter++ ){
		findTop(popula, tc_mid, lc_mid);
		cout<<"iterator: "<<iter<<"*******************************************************"<<endl;
		weedGenes(popula, indNum, fit_sum, iter*2);
		printFit(popula,indNum);
		cout<<indNum<<"indNum"<<endl;
		mutation(w_graph, popula, best_server, best_path, best_cost, best_idx,indNum, fit_sum,iter*3);
		cout<<indNum<<"indNum"<<endl;
		cross(popula, best_server, best_path, best_cost, best_idx, indNum, fit_sum, iter*5);
		cout<<indNum<<"indNum"<<endl;
		printFit(popula,indNum);
		cout<<"total cost:"<<best_cost<<endl;
		tend = std::chrono::system_clock::now();
		elapsed_seconds = tend - start;
		if (elapsed_seconds.count() > 10000)
		{
			break;
		}

}
	string output = pathString(best_path, agency_map, find_so_all);
	cout<<"total cost: "<<best_cost<<endl;
	char * topo_file = new char[output.size() + 1];
	std::copy(output.begin(), output.end(), topo_file);
	topo_file[output.size()] = '\0'; // don't forget the terminating 0

	 //直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
	delete[] topo_file;
}
