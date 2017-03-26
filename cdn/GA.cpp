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
extern const int destroy_iter;
extern int destroy_count_down;
extern map <int, vector<float>> w_graph;
extern vector<int> comu_trans_cost;
extern vector<int> comu_server_cost;
extern  int tc_mid,lc_mid;
extern int punish;


extern int m,n,s,t,line_num;
extern vector<vector<int>> paths;
extern bool find_so_all;
extern map<int,int>  valid_server;
extern map<int, vector<int>> valid_node;
extern map<int, vector<int>> valid_agency;
extern int customer_demand;
extern int server_unit_cost;
extern int base_cost;
extern vector<int> alter_nodes;
extern vector<int> dif_nodes;
extern vector<int> total_nodes;
extern vector<int> agencys;

extern int best_idx;
extern vector <vector<int>> best_path;
extern int best_cost;
extern int best_idx;
extern int best_fit;



//agency：代理的节点编号
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
vector<bool> creatGene(map<int, vector<float> > w_graph, int gene_size, int node_num, float nearby, int rseed){
	srand((unsigned)(time(0))+rseed);
	vector<bool> gene(node_num);
	float distribute;
	int count = 0;
	while(count < gene_size){
		distribute = (rand()%10)/float(10);
		if(distribute < nearby){
			//产生nearby附近的点
			gene[alter_nodes[rand()%alter_nodes.size()]] = 1;
		}
		else{
			gene[dif_nodes[rand()%dif_nodes.size()]] = 1;
		}
		count++;
	}
	return gene;
}

vector<int> createGeneValid(vector<vector<float> > w_graph, map<int,int> valid_server, int rseed){
	srand((int)(time(0))+rseed);
	int v_size = valid_server.size();
	int w_size = w_graph.size();
	int loc,loc_ele;
	vector<int> gene(node_num);

	for(map<int,int>::iterator it = valid_server.begin(); it!=valid_server.end(); it++){
		gene[it->first] = 1;
	}
	for(int i = 0;i<node_num/3;i++){
		gene[rand()%node_num] = 1;
	}

//		for(int i = 0; i<w_size-v_size; i++){
//			loc = rand()%w_size;
//			loc_ele = rand()%(w_graph[loc].size()/2);
//			gene.push_back(w_graph[loc][2*loc_ele]);
//		}

	return gene;
}

void exchangeUnit(Unit &a,Unit &b){
	Unit tgene;

	tgene.gn = a.gn;
	tgene.fn = a.fn;
	tgene.vn = a.vn;
	tgene.va = a.va;
	tgene.sr = a.sr;
	tgene.tc = a.tc;
	tgene.lc = a.lc;

	a.gn = b.gn;
	a.fn = b.fn;
	a.vn = b.vn;
	a.va = b.va;
	a.sr = b.sr;
	a.tc = b.tc;
	a.lc = b.lc;

	b.gn = tgene.gn;
	b.fn = tgene.fn;
	b.vn = tgene.vn;
	b.va = tgene.va;
	b.sr = tgene.sr;
	b.tc = tgene.tc;
	b.lc = tgene.lc;
};

void changeBest(int &best_fit, int &fit_cost,int &best_cost,int unit_cost, int &best_idx, int indNum, vector<vector<int>> &best_path, vector<vector<int>>  paths){
	best_fit = fit_cost;
	best_cost = unit_cost;
	best_idx = indNum;
	best_path = paths;
	cout<<"find_best_cost:"<<best_cost<<endl;
//				for(int i = 0; i<best_path.size(); i++){
//					for(int j: best_path[i]){
//						cout<<j<<" ";
//					}
//					cout<<endl;
//					}
}


//计算一个基因的适应度。 返回false表示没有该基因无解
bool computeFit(vector<bool> &gene, vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node, map<int, vector<int>> &valid_agency,int &transfer_cost, int &fit_cost, int unit_server_cost){
	bool find_so = false;
	int require_demand;
	int tc_weight=1,lc_weight=1;
	vector<int> servers;
	for(int i = 0;i<gene.size();i++){
		if(gene[i]){
			servers.push_back(i);
		}
	}
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, servers, s, iter_pointer, cl);
	require_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, valid_agency, iter_pointer, customer_demand, n, s,t,transfer_cost);

	if(transfer_cost>tc_mid) tc_weight=2;
	if(unit_server_cost*valid_server.size()>lc_mid) lc_weight = 2;

	fit_cost =  base_cost - transfer_cost-unit_server_cost*valid_server.size();
	fit_cost = fit_cost > 0?fit_cost:0;

	if(require_demand==0)
		find_so = true;
	if(find_so==false)
		fit_cost = 0;

	return find_so;
}
//找到传输费用和部署费用的中间值
void findTop(vector<Unit> &popula, int &tc_mid, int &lc_mid){
	for(int i = 0;i<groupnum;i++){
		comu_trans_cost[i] = popula[i].tc;//传输费用
		comu_server_cost[i] = popula[i].lc;//部署费用
	}
	sort(comu_trans_cost.begin(), comu_trans_cost.begin()+groupnum, less<int>() );
	sort(comu_server_cost.begin(), comu_server_cost.begin()+groupnum, less<int>() );
	tc_mid = comu_trans_cost[int(0.4*groupnum)];
	lc_mid = comu_server_cost[int(0.4*groupnum)];
}

//void destroy_gene(map<int,vector<float> > w_graph, Unit &popula){
//	int mute_loc_i, mute_loc_j, mute_choice,tmp_gene;//要变异的位置
//
//	if(popula.gn.size()!=0){
//		mute_loc_i = rand() % popula.vn.size();
//		map<int,vector<int>>::iterator it =popula.vn.begin();
//		for(int midx = 0; midx< mute_loc_i; midx++) it++;
//		mute_loc_j = rand()%it->second.size();
//
//		selectAngencyNodes(w_graph, it->second[mute_loc_j], mute_choice);
//
//		//替换服务器
//		popula.gn[it->first] = 0;
//		popula.gn[mute_choice] = 1;
//	}
//	else{
//		popula.gn[alter_nodes[rand()%alter_nodes.size()]] = 1;
//	}
//
//}

void destroy(map<int,vector<float> > w_graph, vector<Unit> &popula, int &indNum, int &fit_sum, int rseed){
	//已经陷入局部最优，灾变
	int fit_cost,transfer_cost;
	bool find_so=false;
	doVariant(w_graph, popula[0], popula[0],5,rseed);
	for(int k =0; k < indNum;k++){
		fit_sum -= popula[k].fn;
		doVariant(w_graph, popula[k], popula[k],2,rseed+k);
		find_so = computeFit(popula[k].gn, paths, valid_server, valid_node, valid_agency,transfer_cost, fit_cost, server_unit_cost);
		popula[k].vn = valid_node;
		popula[k].va = valid_agency;
		popula[k].fn = fit_cost;
		fit_sum += fit_cost;

		if(find_so){
			find_so_all = true;
			if(fit_cost > best_fit){
				cout<<"destroy_find:"<<k<<endl;
				changeBest(best_fit,fit_cost,best_cost,popula[k].lc+popula[k].tc, best_idx, k, best_path,paths);
			}
		}
	}
	if(best_idx != 0){
		exchangeUnit(popula[0], popula[best_idx]);
		best_idx = 0;
	}
}

//indNum当前种群的人口总数
void weedGenes(vector<Unit> &popula, int &indNum, int &fit_sum, int rseed){
	srand((unsigned)(time(0))+rseed);
	int count = 0;
	Unit tunit;
	int rand_sr, max_sr = 0;

	if(destroy_count_down<=0){
		//种群抱团，灾变
		cout<<"灾变"<<endl;
		destroy_count_down = 3;
//		destroy(w_graph, popula, indNum, fit_sum, -100);
	}

	for(int i = 0;i < indNum ;i++){
		popula[i].sr = int(10000*popula[i].fn/float(fit_sum));
		max_sr = max_sr>popula[i].sr?max_sr:popula[i].sr;
	}


	while(indNum > groupnum){
		count++;
		rand_sr = rand()%max_sr;
		cout<<"weedGenes:"<<indNum<<endl;
		int i = 0;
		if(count>indNum){
				i = 1;
				rand_sr = rand_sr*1.2;
			}


		for(i; i < indNum; i++){
			if(rand_sr > popula[i].sr){
				//命中
				if (i==0){
					cout<<"shot 0: ";
					cout<<"bullet "<<rand_sr<<" sr "<<popula[0].sr;
					cout<<" max_sr"<<max_sr<<endl;
				}
				fit_sum -=popula[i].fn;
				exchangeUnit(popula[i],popula[indNum-1]);
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
		mute_idx = rand()%groupnum;
		doVariant(w_graph, popula[mute_idx], popula[indNum],1,rseed+k);
		find_so = computeFit(popula[indNum].gn, paths, valid_server, valid_node, valid_agency,transfer_cost, fit_cost, server_unit_cost);
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
		popula[indNum].va = valid_agency;
		popula[indNum].fn = fit_cost;
		popula[indNum].tc = transfer_cost;
		popula[indNum].lc = valid_node.size()*server_unit_cost;
		fit_sum += fit_cost;
		//子代父代竞争
		if(popula[indNum].fn > popula[k].fn){
			exchangeUnit(popula[k], popula[indNum]);
			if(find_so){
				find_so_all = true;
				if(fit_cost > best_fit){
					cout<<"Mute find"<<indNum<<endl;
					changeBest(best_fit, fit_cost,best_cost,popula[k].tc+ popula[k].lc, best_idx, k, best_path,paths);
				}
			}
		}
		indNum++;
	}
	if(best_idx != 0){
		exchangeUnit(popula[0], popula[best_idx]);
		best_idx = 0;
	}
	else{
		destroy_count_down--;
	}
}


void cross(vector<Unit> &popula, vector<int> &best_server, vector<vector<int>> &best_path, int &best_cost, int &best_idx, int &indNum, int &fit_sum, int rseed){
	srand(unsigned(time(0))+rseed);
	int cross_ave = int(cross_rate*groupnum);
	int fa,ma=0;
	int smaller = 0;
	int fit_cost,transfer_cost;
	bool find_so =false;

	for(int j = 0; j < cross_ave; j++){
		ma = rand()%groupnum;
		fa = rand()%groupnum;
			while(fa==ma){
				ma = rand()%groupnum;
		}
//		cout<<"cross"<<j<<endl;
		if(indNum==community-1)  {cout<<"indNum:"<<j<<endl;}
		doCross(popula[ma], popula[fa], popula[indNum],popula[indNum+1],1,rseed+j);
		find_so = computeFit(popula[indNum].gn, paths, valid_server, valid_node, valid_agency,transfer_cost, fit_cost, server_unit_cost);
		popula[indNum].vn = valid_node;
		popula[indNum].va = valid_agency;
		popula[indNum].fn = fit_cost;
		popula[indNum].tc = transfer_cost;
		popula[indNum].lc = valid_node.size()*server_unit_cost;
		fit_sum += fit_cost;
		//子代父代竞争
		if(popula[fa].fn > popula[ma].fn){
			smaller = ma;
		}
		else{
			smaller = fa;
		}
		if(popula[indNum].fn > popula[smaller].fn){
//			cout<<"excange:"<<popula[indNum].fn<<endl;
			exchangeUnit(popula[smaller],popula[indNum]);
			if(find_so){
				find_so_all = true;
				if(fit_cost > best_fit){
					cout<<"cross find:"<<indNum<<endl;
					cout<<popula[smaller].fn;
					changeBest(best_fit, fit_cost, best_cost,popula[smaller].tc+ popula[smaller].lc, best_idx, smaller, best_path, paths);
				}
			}
		}

		find_so = computeFit(popula[indNum+1].gn, paths, valid_server, valid_node, valid_agency, transfer_cost, fit_cost, server_unit_cost);
		popula[indNum+1].vn = valid_node;
		popula[indNum+1].va = valid_agency;
		popula[indNum+1].fn = fit_cost;
		popula[indNum+1].tc = transfer_cost;
		popula[indNum+1].lc = valid_node.size()*server_unit_cost;
		fit_sum += fit_cost;

		//子代父代竞争
		if(popula[fa].fn > popula[ma].fn){
			smaller = ma;
		}
		else{
			smaller = fa;
		}
		if(popula[indNum].fn > popula[smaller].fn){
			exchangeUnit(popula[smaller],popula[indNum+1]);
			if(find_so){
				find_so_all = true;
				if(fit_cost > best_fit){
					cout<<"cross find"<<indNum+1<<endl;
					changeBest(best_fit,fit_cost,best_cost,popula[smaller].tc+ popula[smaller].lc, best_idx, smaller, best_path,paths);
				}
			}
		}
		indNum = indNum+2;
	}
	if(best_idx!=0){
		exchangeUnit(popula[0],popula[best_idx]);
		best_idx = 0;
	}
	else{
		destroy_count_down--;
	}

}

