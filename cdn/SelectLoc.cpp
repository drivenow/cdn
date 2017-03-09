//选择服务器地址
#include "SelectLoc.h"
#include "graph.h"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
using std::vector;
using std::swap;
using namespace std;


//根据网络节点出度选定一个出度最大的集合当作最初初始服务器位置
//param: servers, bestservers, serversbak等参数均为this对象的私有变量
void ServerLoc:: SelectLoc(Graph g, int node_num, int servernum){
	int server_num = servernum;
	vector<servertuple> servertuples(node_num);
	for(int i =0; i < g.vertex_num; i++){
		int cur_outdegree = 0;
		Edge * next = g.vertices[i];
		while(next!=nullptr){
			cur_outdegree ++;
			next = next->next_edge;
		}
		servertuple tuple{i, cur_outdegree};
		servertuples[i] = tuple;
	}

	std::sort(servertuples.begin(), servertuples.end(), [](const servertuple & a,
			const servertuple & b) {
		return a.server_outdeg > b.server_outdeg;
	});

	servers.assign(servertuples.begin(), servertuples.begin() + server_num);
	bestservers.assign(servertuples.begin(), servertuples.begin() + server_num);
	serversbak.assign(servertuples.begin() + server_num, servertuples.end());

}


//分别给servers 和 servers_bak 一个参考了节点出度比例的随机数，并进行替换
//server中节点代价分别为 [a,b,c,d] 替换a的概率p=1-(a-min(server)+1)/(sum(server)-(min(server-1))*4);
void ServerLoc::serverMutation(int serversbak_num){
	int server_num = this->server_num;
	int bak_num = serversbak_num;

	vector<servertuple>::iterator minit = servers.begin();
	vector<servertuple>::iterator minproservers_it = servers.begin();
	int servers_outdegsum = (*minit).server_outdeg;
	for(vector<servertuple>::iterator it = servers.begin()+1; it<servers.end(); it++){
		servers_outdegsum += (*it).server_outdeg;
		if((*it).server_outdeg < (*minit).server_outdeg)
			minit = it;
	}

	//最可能被替换servers数组下标
	float minpro_sev = rand()%30/(float)31*(servers[0].server_outdeg - (*minit).server_outdeg +1)/(servers_outdegsum-server_num*((*minit).server_outdeg-1))*1.0;
	//TODO: 如果severs 只有一个元素下面循环会不会越界
	for(vector<servertuple>::iterator i = servers.begin()+1; i< servers.end(); i++){
		float pro = rand()%30/(float)31*((*i).server_outdeg - (*minit).server_outdeg +1)/(servers_outdegsum-server_num*((*minit).server_outdeg-1))*1.0;
		if (pro<minpro_sev){
			minpro_sev = pro;
			minproservers_it = i;
		}
	}

	vector<servertuple>::iterator minit_bak = serversbak.begin();
	vector<servertuple>::iterator maxproseversbak_it = serversbak.begin();
	int serversbak_outdegsum = (*minit_bak).server_outdeg;
	for(vector<servertuple>::iterator it =  minit_bak+1; it<serversbak.end(); it++){
		serversbak_outdegsum += (*it).server_outdeg;
		if((*it).server_outdeg <(*minit_bak).server_outdeg)
			minit_bak = it;
	}

	float maxpro_sevbak = rand()%30/(float)31*(serversbak[0].server_outdeg - (*minit_bak).server_outdeg +1)/(serversbak_outdegsum- bak_num*((*minit_bak).server_outdeg-1))*1.0;
	//TODO: 如果severs 只有一个元素下面循环会不会越界
	for(vector<servertuple>::iterator i = serversbak.begin()+1; i< serversbak.end(); i++){
		float pro = rand()%30/(float)31*((*i).server_outdeg - (*minit_bak).server_outdeg +1)/(serversbak_outdegsum-bak_num*((*minit_bak).server_outdeg-1))*1.0;
		if (pro > maxpro_sevbak){
			maxpro_sevbak = pro;
			maxproseversbak_it = i;
		}
	}


	servertuple st = *minproservers_it;
	*(minproservers_it) = *(maxproseversbak_it);
	*(maxproseversbak_it) = st;

}

