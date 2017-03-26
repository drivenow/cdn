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
#include <iostream>
using namespace std;

//************************************************************************************************************************
void selectAngencyNodes(vector<vector<float> > w_graph, int mute_loc, int &choice);

//生成一个长度是gene.size()的基因
vector<int> creatGene(vector<vector<float> > w_graph, int gene_size, int node_num, int rseed);
vector<int> createGeneValid(vector<vector<float> > w_graph, map<int,int> valid_server, int gene_len, int rseed);

//计算一个基因的适应度,若该基因不能找到路径，返回false
bool computeFit(char * topo[MAX_EDGE_NUM], vector<int> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, int &transfer_cost, int &fit_cost, int unit_server_cost);

//利用轮盘淘汰种群中的个体，返回淘汰个体数目
int weedGenes(vector<vector<int>> &popu, vector<int> &fit, int &fit_sum, float mortality,int rseed);

//从种群中随机挑选父代，生成child_num个孩子,若孩子无解则丢弃，重新creatGene, 同时改变fit popu fit_sum;
int cross(vector<vector<int>> &popu, vector<int> &fit, int &fit_sum, int child_num,int rseed);

//对种群中从mu_idx位置开始的基因进行变异
void mutation(char * topo[MAX_EDGE_NUM],vector<vector<float> > w_graph, vector<vector<int>> &popu, vector<int> &fit, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int node_num, int mu_idx, float mutationrate, int &fit_sum, int server_unit_cost,int rseed);

#endif /* CDN_GA_H_ */
