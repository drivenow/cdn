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

int groupnum = 40;			//种群大小
float mortality = 0.6;		//每次产生子代的概率
int best_cost = INF;
float mutationrate = 0.6;	//基因突变率
float maxiteration = 30;	//允许杂交的最大代数

int m,n,s,t,line_num;
int customer_demand = 0;
int transfer_cost = 0;
int fit_cost = 0;
int fit_sum = 0;
vector <int> fit(groupnum);				//种群对应基因的适应度
vector <int> best_server;
vector <vector<int>> popu(groupnum);	//种群
vector<vector<int>> paths;
vector <vector<int>> best_path;
vector <vector<float>> w_graph;
map<int,int>  valid_server;
bool find_so_all = false, find_so = false;
//int agency_choose[1000];
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

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
	start = std::chrono::system_clock::now();
	int node_num, link_num, customer_num, server_unit_cost;
	void print_vector(vector<int> vec);

	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");

	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	server_unit_cost = std::atoi(topo[2]);

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

	srand(time(0));
	//init edge of the network
	cl = dataLoad(edge,flow, topo,line_num,n,s,t,pointer,cl, customer_demand);
	vector<vector<int>> custsNodes = allCustomerNodes(edge, pointer, cl, t, agency_map, 1);
	float dist_sum;
	for(int i = 0; i < agency_map.size(); i++){
		dist_sum = 0;
		for(int j = 0;j<custsNodes[i].size()/2;j++){
			dist_sum += custsNodes[i][2*j+1];
		}
		vector<float>  w_graph_ele;
		for(int j = 0;j<custsNodes[i].size()/2;j++){
			w_graph_ele.push_back(custsNodes[i][2*j]);
			w_graph_ele.push_back(custsNodes[i][2*j+1]/dist_sum+0.1);
		}
		w_graph.push_back(w_graph_ele);
	}

//	for(int i = 0; i<agency_map.size(); i++){
//		vector<int> t(node_num, 1);
//		w_graph.push_back(t);
//	}

	//预处理产生候选祖先
	int gene_ans_len = agency_map.size()*2;
	int gene_len = agency_map.size();
	vector <int> gene_ans(agency_map.size()*2);

	for(int i = 0; i < groupnum; i++){
		find_so = false;
		while(!find_so){
			gene_ans = creatGene(w_graph, gene_ans_len, node_num, i);
			find_so = computeFit(topo, gene_ans, paths, valid_server, transfer_cost, fit_cost, server_unit_cost);
		}
//		for(map<int,int>::iterator it = valid_server.begin();it!=valid_server.end();it++){
//			cout<<"first:"<<it->first<<"\t"<<"second:"<<it->second<<endl;
//		}
//		print_vector(gene_ans);
		popu[i] = createGeneValid(w_graph, valid_server ,gene_len, -i);
		print_vector(popu[i]);
	}
	//正式祖先
	for(int i = 0; i < groupnum; i++){
		find_so = computeFit(topo, popu[i], paths, valid_server, transfer_cost, fit_cost, server_unit_cost);
		fit[i] = fit_cost;
//		cout<<"gene ans fit size:"<<fit.size()<<endl;

		if(find_so){
			find_so_all = true;
			if(fit_cost < best_cost){
				best_cost = fit_cost;
				best_server = popu[i];
				best_path = paths;
				cout<<"best_cost:"<<best_cost<<endl;

//				for(int i = 0; i<best_path.size(); i++){
//					for(int j: best_path[i]){
//						cout<<j<<" ";
//					}
//					cout<<endl;
//				}
		}
		fit_sum += fit_cost;
		}
	}

	for(int iter = 2; iter < maxiteration; iter++ ){
		cout<<"iterator: "<<iter<<"*******************************************************"<<endl;
		int weedupnum = weedGenes(popu, fit, fit_sum, mortality, iter*2);
		mutation(topo, w_graph, popu, fit, best_server, best_path, best_cost, node_num, 0, mutationrate, fit_sum, server_unit_cost,iter*4);
//		cout<<"popu size: "<< popu.size()<<"淘汰个数： "<<weedupnum<<endl;
		int mu_idx = cross(popu, fit, fit_sum, weedupnum, iter*3);
//		cout<<"mu_idx: "<<mu_idx<<endl;
		mutation(topo, w_graph, popu, fit, best_server, best_path, best_cost, node_num, mu_idx, mutationrate, fit_sum, server_unit_cost,iter*4);
//		for(int i = 0;i<popu.size();i++){
//			print_vector(popu[i]);
//		}
		print_vector(fit);
		tend = std::chrono::system_clock::now();
		elapsed_seconds = tend - start;

		if (elapsed_seconds.count() > 40)
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
