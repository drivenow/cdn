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
extern int community;
extern float child_rate;
extern float cross_rate;


extern int m,n,s,t,line_num;
extern vector<vector<int>> paths;
extern bool find_so_all;
extern map<int,int>  valid_server;
extern map<int, vector<int>> valid_node;
extern int customer_demand;
extern int server_unit_cost;
extern int base_cost;
extern vector<int> alter_nodes;


void selectAngencyNodes(map<int, vector<float> > w_graph, int agency, int &choice){
	int min_p = 0;
	int tmp;
	auto search = w_graph.find(agency);

	for(int j = 0; j < search->second.size()/2; j++){
		tmp = search->second[2*j+1]*(rand()%N);
		if(j==0) {min_p = tmp; choice=search->second[0];}
		if(tmp < min_p){
			choice = search->second[2*j];
			min_p = tmp;
		}
	}
}
//w_weight 为权重值，不只是距离，因要多次使用因此直接保存。 gene 要初始化
vector<int> creatGene(map<int, vector<float> > w_graph, int gene_size, int node_num, int rseed){
	srand((unsigned)(time(0))+rseed);
	vector<int> gene(gene_size);

	int i =0;
	for(map<int, vector<float> >::iterator it = w_graph.begin(); it != w_graph.end(); it++){
		int loc = 2*(rand()%(it->second.size()/2));
		gene[i] = it->second[loc];
		i++;
	}
	return gene;
}

void createGeneValid(map<int,vector<float> > w_graph, map<int,int> valid_server, vector<int> &gene,int gene_len, int rseed){
	srand((unsigned)(time(0))+rseed);
	int v_size = valid_server.size();
	int w_size = w_graph.size();
	int loc,loc_ele;

	if (v_size <= gene_len){
		int i;
		for(i = 0; i < gene_len-v_size; i++){
			loc = rand()%w_size;
			map<int,vector<float> >::iterator oit = w_graph.begin();
			for(int ii=0;ii<loc;ii++) oit++;
			loc_ele = rand()%(oit->second.size()/2);
			gene[i] = oit->second[2*loc_ele];
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
	int require_demand;

	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, gene, s, iter_pointer, cl);
	require_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);

	fit_cost =  base_cost - transfer_cost-unit_server_cost*valid_server.size()-require_demand*10;
	fit_cost = fit_cost>0?fit_cost:0;

//	if(find_so==false)
//		fit_cost = 0;
	if(require_demand==0)
		find_so = true;

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
//		cout<<"weedGenes:"<<indNum<<endl;
//		for(auto ge:popula[0].gn)
//			cout<<ge<<" ";
//		cout<<endl;
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


//从下标mu_idx处开始的基因进行变异，mu_idx为种群淘汰之后的个数
void mutation(map<int,vector<float> > w_graph, vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx, int &indNum, int &fit_sum, int rseed){
	srand(unsigned (time(0))+rseed);
	bool find_so=false;
	int transfer_cost,fit_cost,mute_idx;
	Unit tgene;

	for(int k = 0; k < int(groupnum*child_rate); k++){
		cout<<"k:"<<k<<endl;
		mute_idx = rand()%groupnum;
		doVariant(w_graph, popula[mute_idx], popula[indNum],1,rseed+k);
//			for(auto iii: popula[indNum].gn){
//				cout<<iii<<" ";
//			}
		find_so = computeFit(popula[indNum].gn, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);
//			cout<<fit_cost<<endl;;
//			for(auto ge:popula[indNum].gn)
//				cout<<ge<<" ";
//			cout<<endl;
//			for(map<int,vector<int>>::iterator vn = valid_node.begin();vn!=valid_node.end();vn++){
//				cout<<vn->first<<":";
//				for(auto ge:vn->second){
//					cout<<ge<<" ";
//				}
//				cout<<endl;
//			}
		popula[indNum].vn = valid_node;
		popula[indNum].vn_len = valid_node.size();
		popula[indNum++].fn = fit_cost;
		fit_sum += fit_cost;

		if(find_so){
			find_so_all = true;
			if(fit_cost > best_cost){
				best_cost = fit_cost;
				best_idx = indNum;
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



	if(best_idx != 0)
	{
		tgene.gn = popula[0].gn;
		tgene.fn = popula[0].fn;
		tgene.vn = popula[0].vn;
		tgene.vn_len = popula[0].vn_len;

		popula[0].gn = popula[best_idx].gn;
		popula[0].fn = popula[best_idx].fn;
		popula[0].vn = popula[best_idx].vn;
		popula[0].vn_len = popula[best_idx].vn_len;

		popula[best_idx].gn = tgene.gn;
		popula[best_idx].fn = tgene.fn;
		popula[best_idx].vn = tgene.vn;
		popula[best_idx].vn_len = tgene.vn_len;

		best_idx = 0;
		cout<<best_idx<<endl;
	}
//	else{
//		//已经陷入局部最优，灾变
//		cout<<"灾变"<<endl;
//		doVariant(w_graph, popula[0], popula[0],5,rseed);
//		for(int k =0;k<indNum;k++){
//			doVariant(w_graph, popula[k], popula[k],5,rseed+k);
//			find_so = computeFit(popula[k].gn, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);
////			cout<<fit_cost<<endl;;
////			for(auto ge:popula[k].gn)
////				cout<<ge<<" ";
////			cout<<endl;
////			for(map<int,vector<int>>::iterator vn = valid_node.begin();vn!=valid_node.end();vn++){
////				cout<<vn->first<<":";
////				for(auto ge:vn->second){
////					cout<<ge<<" ";
////				}
////				cout<<endl;
////			}
//			popula[k].vn = valid_node;
//			popula[k].vn_len = valid_node.size();
//			popula[k].fn = fit_cost;
//			fit_sum += fit_cost;
//
//			if(find_so){
//				find_so_all = true;
//				if(fit_cost > best_cost){
//					best_cost = fit_cost;
//					best_idx = k;
////					best_server = popula[indNum].gn;
//					best_path = paths;
//					cout<<"total_cost:"<<base_cost-best_cost<<endl;
//	//				for(int i = 0; i<best_path.size(); i++){
//	//					for(int j: best_path[i]){
//	//						cout<<j<<" ";
//	//					}
//	//					cout<<endl;
////					}
//				}
//			}
//		}
//		if(best_idx != 0)
//			{
//				tgene.gn = popula[0].gn;
//				tgene.fn = popula[0].fn;
//				tgene.vn = popula[0].vn;
//				tgene.vn_len = popula[0].vn_len;
//
//				popula[0].gn = popula[best_idx].gn;
//				popula[0].fn = popula[best_idx].fn;
//				popula[0].vn = popula[best_idx].vn;
//				popula[0].vn_len = popula[best_idx].vn_len;
//
//				popula[best_idx].gn = tgene.gn;
//				popula[best_idx].fn = tgene.fn;
//				popula[best_idx].vn = tgene.vn;
//				popula[best_idx].vn_len = tgene.vn_len;
//
//				best_idx = 0;
//				cout<<best_idx<<endl;
//			}
//	}
}


void cross(vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx, int &indNum, int &fit_sum, int rseed){
	srand(unsigned(time(0))+rseed);
	int cross_ave = int(cross_rate*groupnum);
	int fa,ma;
	int fit_cost,transfer_cost;
	bool find_so =false;

	for(int j = 0; j < cross_ave; j++){
		fa = rand()%groupnum;
		ma = rand()%groupnum;
		while(fa==ma){
			ma = rand()%groupnum;
		}
		cout<<"cross"<<j<<endl;
		if(indNum==community)  {cout<<"indNum:"<<j<<endl;}
		doCross(popula[ma], popula[fa], popula[indNum],popula[indNum+1],1,rseed+j);
		find_so = computeFit(popula[indNum].gn, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);
		popula[indNum].vn = valid_node;
		popula[indNum].vn_len = valid_node.size();
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

		find_so = computeFit(popula[indNum+1].gn, paths, valid_server, valid_node, transfer_cost, fit_cost, server_unit_cost);
		popula[indNum+1].vn = valid_node;
		popula[indNum+1].vn_len = valid_node.size();
		popula[indNum+1].fn = fit_cost;
		fit_sum += fit_cost;
		if(find_so){
			find_so_all = true;
			if(fit_cost > best_cost){
				best_cost = fit_cost;
				best_idx = indNum+1;
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
		indNum = indNum+2;
	}
}

