/*
 * SelectLoc.h
 *
 *  Created on: Mar 7, 2017
 *      Author: full
 */

#ifndef CDN_SERVER_H_
#define CDN_SEEVER_H_

#include <vector>
#include <iostream>
#include "util.h"

struct structnode
{
	int index;
	int out_degree;
	int nerb_agencynum; //节点相邻节点代理点个数

	structnode() {}
	structnode(int id, int od, int custnum) : index(id), out_degree(od), nerb_agencynum(custnum) {}
};

struct Edge
{
	int index;
	Edge *next_edge;
	int band_width;
	int unit_cost;
};

struct NetGraph
{
	int vertex_num;
	int edge_num;
	std::vector<Edge *> vertices;
	NetGraph(int node_num) : vertex_num(node_num), edge_num(0)
	{
		for (int i = 0; i != node_num; ++i)
		{
			vertices.push_back(nullptr);
		}
	}

	//从文件缓冲区读取网络
	void CreateFromBuf(char **buf, NetGraph &netgraph, int line_num);
};

struct Customer
{
	int index;  //消费者
	int agency; //消费者相连的代理
	int demand; //消费者需求
};

class ServerSelector
{

      private:
	int server_num;
	int backup_num;
	int server_rollid; //被交换的服务器id
	int backup_rollid; //被交换的备选服务器的id

      public:
	std::vector<structnode> servers;
	std::vector<structnode> backups;
	std::vector<structnode> evalue_sheet;

	//初始化服务器数量
	ServerSelector(int server_num, int node_num)
	    : server_num(server_num), backup_num(node_num - server_num) {}

	//按agency对customer 排序
	void SortCustomersByAgency(vector<Customer> &customers);

	//得到评估表
	void GenEvalueSheet(const NetGraph &g, vector<structnode> &evalu_sheet, const map<int, int> &agency_map);

	//选择服务器位置, 写入servers, 返回备用服务器集合
	void Select(vector<structnode> &evalu_sheet);

	//挑选备用服务器节点，（随机）替换已有服务器节点
	void Mutate(void);

	//计算适应度
	//vector<int>

	//退火算法调用Mutate时，将servers, backups回退到mute之前的状态。
	void Rollback(void);

	std::vector<int> get_servers(void);

	void PrintServers(void);
};

#endif /* CDN_SELECTLOC_H_ */
