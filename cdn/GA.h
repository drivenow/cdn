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
	vector<int> valid_gn;//有效基因
	int valid_gn_len;//有效基因的长度
	int fn;//适应度
	int sr;//kill rate

	Unit(){}
	Unit(vector<int> gene, vector<int> valid_gene){
		gn = gene;
		valid_gn = valid_gene;
		valid_gn_len = valid_gene.size();
		fn = 0;
		sr = 0;
	}


	void set_valid( map<int, int> &valid_server){
		valid_gn_len = valid_gn.size()>valid_server.size()?valid_server.size():valid_gn.size();
		int i = 0;
		for(map<int,int>::iterator it = valid_server.begin(); i < valid_gn_len; it++)
		{
			valid_gn[i] = it->first;
			i++;
		}
	}
};

extern int node_num;

//************************************************************************************************************************
void selectAngencyNodes(vector<vector<float> > w_graph, int mute_loc, int &choice);

//生成一个长度是gene.size()的基因
vector<int> creatGene(vector<vector<float> > w_graph, int gene_size, int node_num, int rseed);
void createGeneValid(vector<vector<float> > w_graph, map<int,int> valid_server, vector<int> &gene, int gene_len, int rseed);

//计算一个基因的适应度,若该基因不能找到路径，返回false
bool computeFit(vector<int> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node, int &transfer_cost, int &fit_cost, int unit_server_cost);

//利用轮盘淘汰种群中的个体，返回淘汰个体数目
void weedGenes(vector<Unit> &popula, int &indNum, int &fit_sum, float mortality,int rseed);

//从种群中随机挑选父代，生成child_num个孩子,若孩子无解则丢弃，重新creatGene, 同时改变fit popu fit_sum;
int cross(vector<vector<int>> &popu, vector<int> &fit, int &fit_sum, int child_num,int rseed);

//对种群中从mu_idx位置开始的基因进行变异
void mutation(vector<vector<float> > w_graph, vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx,int &indNum, int mu_idx, float mutationrate, int &fit_sum, int rseed);

inline void doVariant(vector<vector<float> > w_graph, Unit &gsource, Unit &gdest,  int variant_it,int rseed){
	srand((unsigned)(time(0))+rseed);
	vector<int> sgene;
	vector<int> dgene;
	map<int,vector<int>> valid_node;

	for(int vit = 0;vit < variant_it;vit ++){
		int op = rand()%2;
		switch(op){
		case 0:{
			sgene = gsource.valid_gn;
			dgene = gdest.valid_gn;
			for(int i = 0;i < sgene.size();i++){
				dgene[i] = sgene[i];
			}
			int mute_loc_i, mute_loc_j, mute_choice;//要变异的位置
			if(valid_node.size()!=0){
				mute_loc_i = rand()%valid_node.size();
				mute_loc_j = rand()%valid_node[mute_loc_i].size();
				map<int,vector<int>>::iterator it = valid_node.begin();
				for(int midx = 0; midx< mute_loc_i; midx++)
				{
					it++;
				}
				selectAngencyNodes(w_graph, it->second[mute_loc_j], mute_choice);
	//			mute_choice = rand()%node_num;

				for(int k=0;k < dgene.size();k++){
					if(dgene[k] == it->first)
						dgene[k] = mute_choice;
				}
			}
			else{
//				cout<<"valid_gn_len"<<endl;
				mute_choice = rand()%node_num;
				dgene[rand()%gsource.valid_gn_len] = mute_choice;
			}

			gdest.gn = dgene;
		}
		}
	}
}

#endif /* CDN_GA_H_ */
