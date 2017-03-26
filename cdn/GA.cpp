/*
 * GA.cpp

 *
 *  Created on: Mar 20, 2017
 *      Author: full
 */
#include "GA.h"
#include "util.h"
#include "MCMF.h"

using namespace std;
#define N 1000
/*"
 * node_num
	link_num
	customer_num
	server_unit_cost
	vector<Customer> customers(customer_num)
	map<int, int> agency_map;

	int m,n,s,t;
 	int customer_demand;
	int pointer[10000];
	SETZR(pointer);
	vector<vector<int>> paths;
	bool find_so = true;
	map<int,int>  valid_server;
 * */

extern int groupnum;			//种群大小
extern int mortality;		//每次产生子代的概率
extern float mutationrate;	//基因突变率

extern int m,n,s,t,line_num;
extern vector<vector<int>> paths;
extern bool find_so_all;
extern map<int,int>  valid_server;
extern int customer_demand;


void selectAngencyNodes(vector<vector<float> > w_graph, int mute_loc, int &choice){
	srand((int)(time(NULL)));
	int min_p = 0;
	int tmp;

	for(int j = 0; j < w_graph[mute_loc].size()/2; j++){
		tmp = w_graph[mute_loc][2*j+1]*(rand()%N);
		if(j==0) {min_p = tmp;choice=w_graph[mute_loc][0];}
		if(tmp < min_p){
			choice = w_graph[mute_loc][2*j];
			min_p = tmp;
		}
	}
}
//w_weight 为权重值，不只是距离，因要多次使用因此直接保存。 gene 要初始化
vector<int> creatGene(vector<vector<float> > w_graph, int gene_size, int node_num, int rseed){
	srand((unsigned int)(time(0))+rseed);
	vector<int> gene(gene_size);
	for(int i = 0; i < w_graph.size(); i++){

		float tmp,p = 0.0;
		int idx = 0;
		int b = w_graph[i].size()/2;
		for(int j = 0; j < b; j++){
			tmp = w_graph[i][2*j+1]*(rand()%N/((float)N));
			if(j==0) p = tmp;
			if(tmp < p){
				idx = 2*j;
				p = tmp;
			}
		}
		gene[i] = w_graph[i][idx];
	}
	for(int i = w_graph.size(); i < gene_size;i++){
		srand(time(0)+i);
		gene[i] = rand()%node_num;
//		cout<<"createGene idx:"<<gene[i]<<endl;
	}
	return gene;
}

vector<int> createGeneValid(vector<vector<float> > w_graph, map<int,int> valid_server, int gene_len, int rseed){
	srand((int)(time(0))+rseed);
	int v_size = valid_server.size();
	int w_size = w_graph.size();
	int loc,loc_ele;
	vector<int> gene;

	if (v_size <= gene_len){
		for(int i = 0; i<w_size-v_size; i++){
			loc = rand()%w_size;
			loc_ele = rand()%(w_graph[loc].size()/2);
			gene.push_back(w_graph[loc][2*loc_ele]);
		}
		for(map<int,int>::iterator it = valid_server.begin(); it!=valid_server.end(); it++){
			gene.push_back(it->first);
		}
	}
	else{
		int count = 0;
		for(map<int,int>::iterator it = valid_server.begin(); count!=gene_len; it++,count++){
			gene.push_back(it->first);
		}
	}
	return gene;
}


//计算一个基因的适应度。 返回false表示没有该基因无解
bool computeFit(char * topo[MAX_EDGE_NUM], vector<int> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, int &transfer_cost, int &fit_cost, int unit_server_cost){
	bool find_so = false;
	int punish = 100000;
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, gene, s, iter_pointer, cl);
	find_so = findCost(edge, iter_flow, paths, valid_server, iter_pointer, customer_demand, n, s,t,transfer_cost);
	if(find_so)
		fit_cost =  transfer_cost+unit_server_cost*valid_server.size();
	else
		fit_cost = transfer_cost+punish;
	return find_so;
}

//利用轮盘淘汰种群中的个体，返回淘汰个体数目
int weedGenes(vector<vector<int>> &popu, vector<int> &fit, int &fit_sum, float mortality,int rseed){
	srand((int)(time(0))+rseed);
	int weednum = popu.size()*mortality;
//	cout << "weednum: " << weednum << endl;
	if(weednum%2 == 1){
		weednum++;
	}
	int populen = popu.size();
	float p_arr[populen];
	int max_idx = 0;
	float max_p = 0;


	for(int i = 0; i < weednum; i++){
		max_p = 0;
		max_idx = 0;
		for(int j = 0; j< fit.size(); j++){
			p_arr[j] = (fit[j]/float(fit_sum))*(rand()%N);
			if(max_p < p_arr[j]){
				max_p = p_arr[j];
				max_idx = j;
			}
		}
		popu.erase(popu.begin()+max_idx);
//		cout<<"weed popu size:"<<popu.size()<<endl;
		fit_sum -= fit[max_idx];
//		cout<<max_idx<<endl;
//		cout<<fit[max_idx]<<endl;
		fit.erase(fit.begin()+max_idx);
//		cout<<"weed fit size:"<<fit.size()<<endl;
	}
	return weednum;
}

//从种群中随机挑选父代，生成child_num个孩子加入popu,返回交叉前种群大小,便于后面对该索引之后数据做变异 在后面变异操作时判断基因有没有解
int cross(vector<vector<int>> &popu, vector<int> &fit, int &fit_sum, int child_num, int rseed){
	srand(time(0)+rseed);
	if(child_num%2!=0){
		cout<<"illegle number"<<endl;
		return -1;
	}
	int n = popu.size();

	for(int i = 0; i<child_num/2; i++){
		int male = rand()%n;
		int female = rand()%n;
		int p1, p2;
		int elem_len = popu[0].size();
		vector<int> child1;
		vector<int> child2;
		child1 = popu[male];
		child2 = popu[female];
		p1 = rand() % elem_len;
		do
		{
			p2 = rand() % elem_len;
		} while (p1 == p2);

		//保证 p1<p2
		if (p1 > p2)
		{
			p2= p1+p2;
			p1= p2-p1;
			p2= p2-p1;
		}
		int tmp;
		for(int i = p1; i<p2; i++){
			tmp = child1[i];
			child1[i] = child2[i];
			child2[i] =tmp;
		}
		popu.push_back(child1);
		popu.push_back(child2);
	}
	return n;
}

//从下标mu_idx处开始的基因进行变异，mu_idx为种群淘汰之后的个数
void mutation(char * topo[MAX_EDGE_NUM],vector<vector<float> > w_graph, vector<vector<int>> &popu, vector<int> &fit, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int node_num, int mu_idx, float mutationrate, int &fit_sum, int server_unit_cost, int rseed){
	srand((int)(time(0))+rseed);
//	int count = 0;
	bool find_so=false;
	int transfer_cost;
	int fit_cost;

	int mute_loc,mute_choice;//要变异的位置
	for(int k = mu_idx; k < popu.size(); k++){
//		cout<<"mute:"<<endl;
//		if(rand()%N/(float)N < mutationrate){
//			popu[k][rand()%(popu[0].size())] = rand()%node_num;
//		}
		if(rand()%N/(float)N < mutationrate){
			mute_loc = rand()%(popu[0].size());
			selectAngencyNodes(w_graph, mute_loc, mute_choice);
			popu[k][mute_loc] = mute_choice;
//			cout<<"mute:"<<endl;
		}

		find_so = computeFit(topo, popu[k], paths, valid_server, transfer_cost, fit_cost, server_unit_cost);
		if(fit.size()>k){
			fit[k] = fit_cost;
		}
		else{
			fit.push_back(fit_cost);
		}

		if(find_so){
//			cout<<"mute succss:"<<endl;
			find_so_all = true;
			if(fit_cost < best_cost){
				best_cost = fit_cost;
				best_server = popu[k];
				best_path = paths;
				cout<<"best_cost:"<<best_cost<<endl;
//
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
}

