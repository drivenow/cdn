/*
 * SelectLoc.h
 *
 *  Created on: Mar 7, 2017
 *      Author: full
 */

#ifndef CDN_SELECTLOC_H_
#define CDN_SELECTLOC_H_
#include "graph.h"
#include <vector>
using std::vector;

struct servertuple{
	int server_id;
	int server_outdeg;
};

class ServerLoc{

public:
	int server_num;
	int serverbak_num;
	vector<servertuple>  servers ;
	vector<servertuple> bestservers;
	vector<servertuple> serversbak;

	//初始化服务器数量
	ServerLoc(int servernum, int node_num){
		server_num = servernum;
		serverbak_num = node_num-servernum;

		servers = std::move(vector<servertuple>(server_num));
		bestservers = std::move(vector<servertuple>(server_num));
		serversbak = std::move(vector<servertuple>(serverbak_num));

	};

	//挑选备用服务器节点，（随机）替换已有服务器节点
	void serverMutation(int bak_num);

	//选择服务器位置, 写入servers, 返回备用服务器集合
	void SelectLoc(Graph g, int node_num, int servernum);

	int getServers(vector<int> & servers);

};
#endif /* CDN_SELECTLOC_H_ */
