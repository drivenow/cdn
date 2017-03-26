/*
 * GA.h
 *
 *  Created on: Mar 20, 2017
 *      Author: full
 */

#ifndef CDN_GA_H_
#define CDN_GA_H_
#include <vector>
#include "MCMF.h"
#include "lib_io.h"
#include "util.h"
#include <iostream>
using namespace std;

class Unit{
public:
	vector<bool> gn;//基因能
	map<int,vector<int>> vn;//有效基因
	map<int,vector<int>> va;//有效基因
	int fn;//适应度
	int sr;//kill rate
	int tc;//传输费用
	int lc;//部署费用

	Unit(){}
	Unit(vector<bool> gene, map<int,vector<int>> valid_node, map<int,vector<int>> valid_agency){
		gn = gene;
		vn = valid_node;
		va = valid_agency;
		fn = 0;
		sr = 0;
		tc = -1;
		lc = -1;
	}
};

extern int node_num;
extern vector<int> alter_nodes;
extern vector<int> total_nodes;

//************************************************************************************************************************
void selectAngencyNodes(map<int,vector<float> > w_graph, int mute_loc, int &choice);
void exchangeUnit(Unit &a,Unit &b);
void changeBest(int &best_fit, int &fit_cost,int &best_cost,int unit_cost, int &best_idx, int indNum, vector<vector<int>> &best_path, vector<vector<int>>  paths);
void destroy(map<int,vector<float> > w_graph, vector<Unit> &popula,  int &indNum, int &fit_sum, int rseed);

//生成一个长度是gene.size()的基因
//vector<int> creatGene(map<int,vector<float> > w_graph, int gene_size, int node_num, float nearby, int rseed);
vector<bool> creatGene(map<int, vector<float> > w_graph, int gene_size, int node_num, float nearby, int rseed);
//void createGeneValid(map<int,vector<float> > w_graph, map<int,int> valid_server, vector<int> &gene, int gene_len, int rseed);

//计算一个基因的适应度,若该基因不能找到路径，返回false
//bool computeFit(vector<int> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node, int &transfer_cost, int &fit_cost, int unit_server_cost);
bool computeFit(vector<bool> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node, map<int,vector<int>> &valid_agency, int &transfer_cost, int &fit_cost, int unit_server_cost);
void findTop(vector<Unit> &popula, int &tc_mid, int &lc_mid);
//利用轮盘淘汰种群中的个体，返回淘汰个体数目
void weedGenes(vector<Unit> &popula, int &indNum, int &fit_sum,int rseed);

//对种群中从mu_idx位置开始的基因进行变异
void mutation(map<int,vector<float> > w_graph, vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx,int &indNum, int &fit_sum, int rseed);

void cross(vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx, int &indNum, int &fit_sum, int rseed);

inline void doVariant(map<int,vector<float> > w_graph, Unit &gsource, Unit &gdest,  int variant_it,int rseed){
	srand((unsigned)(time(0))+rseed);
	vector<bool> sgene;
	vector<bool> dgene;
	int loc;
	sgene = gsource.gn;
	dgene = gdest.gn;
	map<int,vector<int>> valid_node;

	for(map<int,vector<int>>::iterator it = gsource.vn.begin();it!=gsource.vn.end();it++){
		valid_node[it->first] = it->second;
	}
	for(int i = 0;i < sgene.size();i++){
		dgene[i] = sgene[i];
	}
	for(int i = 0;i<variant_it;i++){
		loc = rand()%alter_nodes.size();
		dgene[alter_nodes[loc]] = dgene[alter_nodes[loc]] ==true?0:1;
	}
}

inline void doCross( Unit &gsource, Unit &gdest, Unit &child1, Unit &child2, int cross_it,int rseed){
	srand((unsigned)(time(0))+rseed);
	int gene_len = gsource.gn.size();

	int p1, p2;
	p1 = rand()%gene_len;
	p2 = rand()%gene_len;
	while(p1==p2){
		p2 = rand()%gene_len;
	}

	if (p1 > p2)
	{
		p2= p1+p2;
		p1= p2-p1;
		p2= p2-p1;
	}

	for(int i = 0;i < p1; i++){
		child1.gn[i] = gsource.gn[i];
		child2.gn[i] = gdest.gn[i];
	}
	for(int i = p1;i<p2;i++){
		child1.gn[i] = gdest.gn[i];
		child2.gn[i] = gsource.gn[i];
	}
	for(int i = p2;i<gene_len;i++){
		child1.gn[i] = gsource.gn[i];
		child2.gn[i] = gdest.gn[i];
	}

}

#endif /* CDN_GA_H_ */
