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
 * Customer cust:单个消费者节点
 * int[] Servers：服务器列表
 * */
//vector<Route_states> get_route(Graph g,Customer cust,int servers[]){
//	std::vector<Route_states> route_states;
//	for(int i=0;i<sizeof(servers)/sizeof(servers[0]);i++){
//		vector<int> nodes_on_path = g.DijkstraShortestPath(cust.agency, servers[i]);//从代理到服务器的反向路径
//		Route_states route;
//		route.mediean_segment = nodes_on_path;
//
//		route_states.push_back(route);
//	}
//
//
//
//	return route_states;
//}

/*
 * 返回单条路径传输的代价(不包括代理节点到消费这节点段),同时更新邻接矩阵
 *
 * int start_point: 消费者节点
 * agency_point: 消费者节点上溯一个点的代理节点
 * int demand： 消费者的需求
 * Route_states route_state： 消费者到服务器的路径
 * */
int get_traffic_route(Customer cust,Route_states route_state,Graph g,vector<Customer> customers){
	int traffic_num;//代价
	int transfer = route_state.limit_all<cust.demand?route_state.limit_all:cust.demand;//数据传输量，取消费者需求和路径供给上限的较小值

	traffic_num = route_state.traffic*transfer;//传输代价
	//update_Graph(g,route_state,transfer);//更新邻接矩阵
	//update_costomer(customers,cust,transfer);//更新消费者



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
	Edge *edge = g.get_edge(route_state.start_segment[1],route_state.start_segment[0]);//反向路径
	edge->band_width = edge->band_width-transfer;
	for (int i=0;i<route_state.median_segment.size();i++){
		Edge *edge = g.get_edge(route_state.median_segment[i+1],route_state.median_segment[i]);
		edge->band_width = edge->band_width-transfer;
	}
}

/*
 * 更新消费者需求*/
void update_costomer(vector<Customer> customers,Customer cust,int transfer){
	customers[cust.start].demand = customers[cust.start].demand-transfer;
}
