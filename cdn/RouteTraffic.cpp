/*
 * route_traffic.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: full
 */
#include <RouteTraffic.h>
#include <vector>
using namespace std;

/*
 * 返回从消费者出发到服务器的最短路径
 * */
vector<Route_states> get_route(){
	vector<Route_states> route_states;



	return route_states;
}

/*
 * 返回单条路径传输的代价(不包括代理节点到消费这节点段)
 *
 * int start_point: 消费者节点
 * agency_point: 消费者节点上溯一个点的代理节点
 * int demand： 消费者的需求
 * Route_states route_state： 消费者到服务器的路径
 * */
int get_traffic_route(int demand,Route_states route_state){
	int traffic_num;
	if (route_state.limit_all<demand){
		//消费者的需求大于整段路径的供给
	}
	else{
		//消费者的需求小于整段路径的供给
		traffic_num = route_state.traffic*demand;
	}
	return traffic_num;
}


/*返回代理节点到消费者节点段的代价
 *
 * */
int get_traffic_edge(int start_point,int agency_point){

}

/*在选完路径输送流量之后，更新邻接矩阵
 * */
void update_Graph(){

}

