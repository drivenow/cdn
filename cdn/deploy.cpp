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

void print_vector(vector<int> vec){
	for(int i=0;i<vec.size();i++){
		cout<<vec[i]<<" ";
	}
	cout<<endl;
}


int groupnum = 40;			//种群中间力量大小
int gene_len;
int child_ave = 1;
int community = groupnum*(1+child_ave);
float mortality = 0.6;		//每次产生子代的概率
float mutationrate = 0.6;	//基因突变率
float maxiteration = 100;	//允许杂交的最大代数
vector<Unit> popula;

int m,n,s,t,line_num;
int customer_demand = 0;
int transfer_cost = 0;
int base_cost = 0;
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
vector <vector<float>> w_graph;
map<int,int>  valid_server;
map<int, vector<int>> valid_node;
bool find_so_all = false, find_so = false;
int node_num, link_num, customer_num, server_unit_cost;



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

	void print_vector(vector<int> vec);

	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");

	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	server_unit_cost = std::atoi(topo[2]);

	base_cost = 2*server_unit_cost*customer_num;

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
//************************************************
//	计算节点权重
	float dist_sum;
	for(int i = 0; i < agency_map.size(); i++){
		dist_sum = 0;
		for(int j = 0;j < custsNodes[i].size()/2;j++){
			dist_sum += custsNodes[i][2*j+1];
		}
		vector<float>  w_graph_ele;
		for(int j = 0;j < custsNodes[i].size()/2;j++){
			w_graph_ele.push_back(custsNodes[i][2*j]);
			w_graph_ele.push_back(custsNodes[i][2*j+1]/dist_sum+0.1);
		}
		w_graph.push_back(w_graph_ele);
	}

	//分配空间
	cout<<"community:"<<community<<endl;
	gene_len = int(customer_num);
	cout<<"gene_len"<<gene_len<<endl;

	for(int i = 0;i < community; i++){
		vector<int> gn(gene_len);
		vector<int> valid_gn(gene_len);
		Unit unit = Unit(gn, valid_gn);
		popula.push_back(unit);
	}

	//预处理产生候选祖先
	///home/full/Desktop/SDK-gcc-submit/case_example/batch2/0/case0.txt out0.txt
	int gene_ans_len = customer_num*2;
	cout<<"gene_ans_len:"<<gene_ans_len<<endl;
	vector <int> gene_ans(gene_ans_len);
	Unit tgene;
	int indNum = 0;

	for(int i = 0; i < groupnum; i++){
		find_so = false;
		while(!find_so){
			gene_ans = creatGene(w_graph, gene_ans_len, node_num, -2*i);
			find_so = computeFit(gene_ans, paths, valid_server, valid_node,transfer_cost, fit_cost, server_unit_cost);
			cout<<1;
		}
//		print_vector(gene_ans);
		 createGeneValid(w_graph, valid_server, popula[i].gn , gene_len, -3*i);
	}
	//正式祖先
	for(int i = 0; i < groupnum; i++){
		indNum++;
		find_so = computeFit(popula[i].gn, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);
		popula[i].set_valid(valid_server);
		popula[i].fn = fit_cost;
		fit_sum += fit_cost;
		if(i==0){
			best_cost = fit_cost;
			best_idx = 0;
		}
		if(find_so){
			find_so_all = true;
			if(fit_cost > best_cost){
				best_cost = fit_cost;
				best_idx = i;
				best_server = popula[i].gn;
				best_path = paths;
				cout<<"total_cost:"<<base_cost-best_cost<<endl;

//				for(int i = 0; i<best_path.size(); i++){
//					PrintPath(best_path[i]);
//				}
			}
		}
	}
	// 移动最优基因
	if(best_idx != 0)
	{
		tgene.gn = popula[0].gn;
		tgene.fn = popula[0].fn;
		tgene.valid_gn = popula[0].valid_gn;
		tgene.valid_gn_len = popula[0].valid_gn_len;
		popula[0].gn = popula[best_idx].gn;
		popula[0].fn = popula[best_idx].fn;
		popula[0].valid_gn = popula[best_idx].valid_gn;
		popula[0].valid_gn_len = popula[best_idx].valid_gn_len;
		popula[best_idx].gn = tgene.gn;
		popula[best_idx].fn = tgene.fn;
		popula[best_idx].valid_gn = tgene.valid_gn;
		popula[best_idx].valid_gn_len = tgene.valid_gn_len;
		best_idx = 0;
	}
	vector<int> server = {5,6,13,15,22,32,34,37,38};
//	find_so = computeFit(server, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);

	for(int iter = 2; iter < maxiteration; iter++ ){
		cout<<"iterator: "<<iter<<"*******************************************************"<<endl;
		weedGenes(popula, indNum, fit_sum, mortality, iter*2);
		printFit(popula,indNum);
		mutation(w_graph, popula, best_server, best_path, best_cost, best_idx,indNum, 0, mutationrate, fit_sum,iter*3);
		printFit(popula,indNum);
		cout<<"total cost:"<<base_cost-best_cost<<endl;
		tend = std::chrono::system_clock::now();
		elapsed_seconds = tend - start;
		if (elapsed_seconds.count() > 40)
		{
			break;
		}

}
	string output = pathString(best_path, agency_map, find_so_all);
	cout<<"total cost: "<<base_cost-best_cost<<endl;
	char * topo_file = new char[output.size() + 1];
	std::copy(output.begin(), output.end(), topo_file);
	topo_file[output.size()] = '\0'; // don't forget the terminating 0

	 //直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
	delete[] topo_file;
}
