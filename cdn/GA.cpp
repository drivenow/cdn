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
extern int child_ave;
extern int community;
extern int mortality;		//每次产生子代的概率
extern float mutationrate;	//基因突变率

extern int m,n,s,t,line_num;
extern vector<vector<int>> paths;
extern bool find_so_all;
extern map<int,int>  valid_server;
extern map<int, vector<int>> valid_node;
extern int customer_demand;
extern int server_unit_cost;
extern int base_cost;


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
	srand((unsigned)(time(0))+rseed);
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

void createGeneValid(vector<vector<float> > w_graph, map<int,int> valid_server, vector<int> &gene,int gene_len, int rseed){
	srand((unsigned)(time(0))+rseed);
	int v_size = valid_server.size();
	int w_size = w_graph.size();
	int loc,loc_ele;

	if (v_size <= gene_len){
		int i;
		for(i = 0; i < gene_len-v_size; i++){
			loc = rand()%w_size;
			loc_ele = rand()%(w_graph[loc].size()/2);
			gene[i] = w_graph[loc][2*loc_ele];
		}
		for(map<int,int>::iterator it = valid_server.begin(); it!=valid_server.end(); it++){
			gene[i] = it->first;
			i++;
		}
	}
	else{
		int count = 0;
		for(map<int,int>::iterator it = valid_server.begin(); count!=gene_len; it++,count++){
			gene[count] = it->first;
		}
	}
}


//计算一个基因的适应度。 返回false表示没有该基因无解
bool computeFit(vector<int> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node,int &transfer_cost, int &fit_cost, int unit_server_cost){
	bool find_so = false;

	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, gene, s, iter_pointer, cl);
	find_so = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);

	fit_cost =  base_cost - transfer_cost-unit_server_cost*valid_server.size();

	if(find_so==false)
		fit_cost = 0;

	return find_so;
}

//indNum当前种群的人口总数
void weedGenes(vector<Unit> &popula, int &indNum, int &fit_sum, float mortality,int rseed){
	srand((unsigned)(time(0))+rseed);
	Unit tunit;
	int rand_sr, max_sr = 0;

	for(int i;i <indNum ;i++){
		popula[i].sr = int(10000*popula[i].fn/float(fit_sum));
		max_sr = max_sr>popula[i].sr?max_sr:popula[i].sr;
	}

	while(indNum > groupnum){
		rand_sr = rand()%max_sr;
		for(int i = 0;i < indNum; i++){
			if(rand_sr >= popula[i].sr){
				//命中
				if (i==0){
					cout<<"shot 0: ";
					cout<<"bullet "<<rand_sr<<" kr "<<popula[0].sr;
					cout<<" max_sr"<<max_sr<<endl;
				}
				fit_sum-=popula[i].fn;
				tunit.gn = popula[indNum-1].gn;
				tunit.fn = popula[indNum-1].fn;
				tunit.sr = popula[indNum-1].sr;
				popula[indNum-1].gn = popula[i].gn;
				popula[indNum-1].fn = popula[i].fn;
				popula[indNum-1].sr = popula[i].sr;
				popula[i].gn = tunit.gn;
				popula[i].fn = tunit.fn;
				popula[i].sr = tunit.sr;
				indNum--;
				break;
			}
		}
	}
}

//从种群中随机挑选父代，生成child_num个孩子加入popu,返回交叉前种群大小,便于后面对该索引之后数据做变异 在后面变异操作时判断基因有没有解
int cross(vector<vector<int>> &popu, vector<int> &fit, int &fit_sum, int child_num, int rseed){
	srand(unsigned(time(0))+rseed);
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
void mutation(vector<vector<float> > w_graph, vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx, int &indNum, int mu_idx, float mutationrate, int &fit_sum, int rseed){
//	int count = 0;
	bool find_so=false;
	int transfer_cost;
	int fit_cost;
	indNum = groupnum;//当前种群个数
	Unit tgene;

	for(int k = 0; k < groupnum; k++){
		for(int j = 0; j < child_ave; j++){
			if(indNum==community)  cout<<"indNum:"<<k<<" "<<j<<endl;
			doVariant(w_graph, popula[k], popula[indNum],1,rseed);
//			for(auto iii: popula[indNum].gn){
//				cout<<iii<<" ";
//			}
			find_so = computeFit(popula[indNum].gn, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);
			popula[indNum].set_valid(valid_server);
			popula[indNum].fn = fit_cost;
			fit_sum += fit_cost;

			if(find_so){
				find_so_all = true;
				if(fit_cost > best_cost){
					best_cost = fit_cost;
					best_idx = indNum;
//					best_server = popula[indNum].gn;
					best_path = paths;
					cout<<"total_cost:"<<base_cost-best_cost<<endl;
	//				for(int i = 0; i<best_path.size(); i++){
	//					for(int j: best_path[i]){
	//						cout<<j<<" ";
	//					}
	//					cout<<endl;
//					}
				}
			}
		}
		indNum++;
	}

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
		cout<<best_idx<<endl;

	}
}

