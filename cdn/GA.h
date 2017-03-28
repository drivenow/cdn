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
	vector<int> gn;//基因能
	map<int,vector<int>> vn;//有效基因
	int vn_len;
	int fn;//适应度
	int sr;//kill rate

	Unit(){}
	Unit(vector<int> gene, map<int,vector<int>> valid_node){
		gn = gene;
		vn = valid_node;
		vn_len = 0;
		fn = 0;
		sr = 0;
	}
};

extern int node_num;

//************************************************************************************************************************
void selectAngencyNodes(map<int,vector<float> > w_graph, int mute_loc, int &choice);

//生成一个长度是gene.size()的基因
vector<int> creatGene(map<int,vector<float> > w_graph, int gene_size, int node_num, int rseed);
void createGeneValid(map<int,vector<float> > w_graph, map<int,int> valid_server, vector<int> &gene, int gene_len, int rseed);

//计算一个基因的适应度,若该基因不能找到路径，返回false
bool computeFit(vector<int> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node, int &transfer_cost, int &fit_cost, int unit_server_cost);

//利用轮盘淘汰种群中的个体，返回淘汰个体数目
void weedGenes(vector<Unit> &popula, int &indNum, int &fit_sum, float mortality,int rseed);

//对种群中从mu_idx位置开始的基因进行变异
void mutation(map<int,vector<float> > w_graph, vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx,int &indNum, int &fit_sum, int rseed);

void cross(vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx, int &indNum, int &fit_sum, int rseed);


inline void doVariant(map<int,vector<float> > w_graph, Unit &gsource, Unit &gdest,  int variant_it,int rseed){
	srand((unsigned)(time(0))+rseed);
	vector<int> sgene;
	vector<int> dgene;
//	cout<<"doVariant1"<<endl;

	for(int vit = 0;vit < variant_it;vit ++){
//		cout<<"doVariant2"<<endl;
//		int op = rand()%2;
		int op = 0;
		switch(op){
			case 0:{
				sgene = gsource.gn;
				dgene = gdest.gn;
				map<int,vector<int>> valid_node;

				for(map<int,vector<int>>::iterator it = gsource.vn.begin();it!=gsource.vn.end();it++){
					valid_node[it->first] = it->second;
				}
				for(int i = 0;i < sgene.size();i++){
					dgene[i] = sgene[i];
				}
				int mute_loc_i, mute_loc_j, mute_choice,tmp_gene;//要变异的位置
				if(gsource.vn_len!=0){
					mute_loc_i = rand()%gsource.vn_len;
					map<int,vector<int>>::iterator it =gsource.vn.begin();
					for(int midx = 0; midx< mute_loc_i; midx++) it++;

					mute_loc_j = rand()%it->second.size();
					selectAngencyNodes(w_graph, it->first, mute_choice);
		//			mute_choice = rand()%node_num;

					tmp_gene = dgene[mute_loc_i];
					dgene[mute_loc_i] = mute_choice;
					dgene[mute_loc_i+gsource.vn_len] = tmp_gene;

//					for(int k=0;k < dgene.size();k++){
//						if(dgene[k] == it->second[mute_loc_j]){
//							dgene[k] = mute_choice;
////							break;
//						}
//					}
	//				cout<<"mute_choice1:"<<mute_choice<<endl;
				}
				else{
	//				cout<<"valid_gn_len"<<endl;
					mute_choice = rand()%node_num;
					dgene[rand()%dgene.size()] = mute_choice;
	//				cout<<"mute_choice2:"<<mute_choice<<endl;
				}

				gdest.gn = dgene;
			}
		}
	}
}

inline void doCross( Unit &gsource, Unit &gdest, Unit &child1, Unit &child2, int cross_it,int rseed){
	int gene_len = gsource.gn.size();

	int p1,p2;
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
