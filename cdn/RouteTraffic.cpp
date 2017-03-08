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

#define MAX_MEDIAN 2//对同一个start_segment选择几条median_segment路线

/*
 * 返回从消费者出发到服务器的最短路径
 * Customer cust:单个消费者节点
 * int[] Servers：服务器列表
 * */
vector<Route_states> get_route(Graph g,Customer cust,int servers[]){
	Edge *edge = g.vertices[cust.agency];//和agency相连的线段
	std::vector<Route_states> route_states;

	while(edge!=nullptr){
		int start_point = edge->index;//代理上一个点
		int cost = edge->unit_cost;//起始代价（代理到代理上一个点的代价）
		std::vector<vector<int>> median_segments(MAX_MEDIAN);//临时储存到每个服务的路径
		std::vector<vector<int>> distance_bounds(MAX_MEDIAN);//

		int dist_min2_max = INF;
		int dist_min2_min = INF;
		int push_num = 0;//加到median_segments中的数据个数

		vector<int> nodes_on_path1;
		int server_num = sizeof(servers)/sizeof(servers[0]);

		for(int i=0;i<server_num;i++){
			vector<int> nodes_on_path = g.DijkstraShortestPath(start_point, servers[i]);//从代理上一个点到服务器的反向路径
			vector<int> distance_bound = g.RetrieveDistanceBound(nodes_on_path);
			distance_bound[0] +=g.get_edge(start_point,cust.agency)->unit_cost;
			//判断路径中是否有服务器不在端点
			bool is_there_server = false;
			int node_num = nodes_on_path.size();
			for (int ii = 0; ii != server_num; ++ii) {
				for (int jj = 0; jj != node_num - 1; ++jj) {
					if (nodes_on_path[jj] == servers[ii]) {
						is_there_server = true;
						break;
					}
				}
				if (is_there_server) {
					break;
				}
			}
			if (is_there_server) {
				continue;
			}
			if (push_num==0){
				median_segments[0] = nodes_on_path;
				distance_bounds[0] = distance_bound;
				dist_min2_max = distance_bound[0];
				dist_min2_min = distance_bound[0];
				push_num = push_num+1;
			}
			else {
				if(push_num<MAX_MEDIAN){

					if(distance_bound[0]>dist_min2_min){
						median_segments[1] = nodes_on_path;
						distance_bounds[1] = distance_bound;
						dist_min2_max = distance_bound[0];
					}
					else{
						median_segments[1] = median_segments[0];
						distance_bounds[1] = distance_bounds[0];
						median_segments[0] = nodes_on_path;
						distance_bounds[0] = distance_bound;
						dist_min2_min = distance_bound[0];
					}
					push_num = push_num+1;
				}
				else{
					if (dist_min2_max > distance_bound[0]){
						if(distance_bound[0] > dist_min2_min){
							median_segments[1] = nodes_on_path;
							distance_bounds[1] = distance_bound;
							dist_min2_max = distance_bound[0];
						}
						else{
							median_segments[1] = median_segments[0];
							distance_bounds[1] = distance_bounds[0];
							median_segments[0] = nodes_on_path;
							distance_bounds[0] = distance_bound;
							dist_min2_max = dist_min2_min;
							dist_min2_min = distance_bound[0];
						}
					}
				}
			}
		}
		for(int i=0;i<push_num;i++){
			Route_states route;
			route.start_segment[0] = cust.agency;route.start_segment[1] = start_point;
			route.limit_entra = edge->band_width;
			route.traffic = distance_bounds[i][0];
			route.limit_all = distance_bounds[i][1];
			route.median_segment = median_segments[i];
			route_states.push_back(route);
		}
		edge = edge->next_edge;
	}

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
int get_traffic_route(const Customer & cust, const Route_states & route_state,
                      Graph & g, vector<Customer> & customers)
{
	// 数据传输量，取消费者需求和路径供给上限的较小值
	int transfer = (route_state.limit_all < cust.demand) ? route_state.limit_all : cust.demand;

	customers[cust.start].demand -= transfer;       // 更新消费者需求
	update_Graph(g, route_state, transfer);        //更新邻接矩阵
	int traffic_num = route_state.traffic * transfer;  // 传输代价

	return traffic_num;
}

/*在选完路径输送流量之后，更新邻接矩阵
 * Graph g：邻接矩阵
 * Route_states route_state：路径信息
 * int transfer: 传输量
 * */
void update_Graph(Graph & g, const Route_states & route_state, int transfer)
{
	Edge *edge = g.get_edge(route_state.start_segment[1], route_state.start_segment[0]);//反向路径
	edge->band_width = edge->band_width - transfer;
	int mediean_edge_num = route_state.median_segment.size() - 1;

	for (int i = 0; i != mediean_edge_num; ++i) {
		edge = g.get_edge(route_state.median_segment[i + 1], route_state.median_segment[i]);
		edge->band_width = edge->band_width - transfer;
	}
}
