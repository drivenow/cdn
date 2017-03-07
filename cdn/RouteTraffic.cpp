/*
 * route_traffic.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: full
 */
#include <RouteTraffic.h>
#include <vector>
#include <graph.h>
using namespace std;

/*
 * 返回从消费者出发到服务器的最短路径
 * */
vector<Route_states> get_route(){
	vector<Route_states> route_states;



	return route_states;
}

/*
 * 返回单条路径传输的代价(不包括代理节点到消费这节点段),同时更新邻接矩阵
 *
 * int start_point: 消费者节点
 * agency_point: 消费者节点上溯一个点的代理节点
 * int demand： 消费者的需求
 * Route_states route_state： 消费者到服务器的路径
 * */
int get_traffic_route(Customer cust,Route_states route_state,Graph g){
	int traffic_num;
	if (route_state.limit_all<cust.demand){
		//消费者的需求大于整段路径的供给上限
		traffic_num = route_state.traffic*route_state.limit_all;
	}
	else{
		//消费者的需求小于整段路径的供给
		traffic_num = route_state.traffic*cust.demand;
	}
	return traffic_num;
}


/*返回代理节点到消费者节点段的代价
 *
 * */
int get_traffic_edge(Customer cust, Graph g){
	int start_point = cust.start;
	int agency_point = cust.agency;

	Edge *next = g.get_edge(agency_point,start_point);
	int traffic = next->unit_cost*cust.demand;

	return traffic;
}

/*在选完路径输送流量之后，更新邻接矩阵
 * Graph g：邻接矩阵
 * Route_states route_state：路径信息
 * int transfer: 传输量
 * */
void update_Graph(Graph g,Route_states route_state,int transfer){


}

