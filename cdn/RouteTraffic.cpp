/*
 * route_traffic.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: full
 */
#include <vector>
using std::vector;

#include <iostream>
using std::cout; using std::endl;

#include "graph.h"
#include "RouteTraffic.h"

#define MAX_MEDIAN 2  //对同一个start_segment选择几条median_segment路线

/*
 * 返回从消费者出发到服务器的最短路径
 * Customer cust:单个消费者节点
 * int[] Servers：服务器列表
 * */
vector<Route_states> get_route(Graph & g, const Customer & cust, int  servers[])
{
	Edge *edge = g.vertices[cust.agency];
	std::vector<Route_states> routes;

	while (edge != nullptr) {
		int start_point = edge->index;    //代理上一个点
		int cost_init = g.GetEdgeWithIndex(start_point, cust.agency)->unit_cost;  //起始代价（代理上一个点到代理的代价）
		std::vector<vector<int>> median_segments(MAX_MEDIAN);  //临时储存到每个服务器的路径
		std::vector<vector<int>> distance_bounds(MAX_MEDIAN);

		int dist_min2_max;
		int dist_min2_min;
		int push_num = 0;  //加到median_segments中的数据个数

		int server_num = sizeof(servers) / sizeof(servers[0]);
		int is_server = false;

		for (int i = 0; i < server_num; i++) {
			if (start_point == servers[i]) {
				is_server = true;
			}
			// 从服务器到代理上一个点的路径
			vector<int> nodes_on_path  = g.DijkstraShortestPath(servers[i], start_point);
			vector<int> distance_bound = g.RetrieveDistanceBound(nodes_on_path);
			distance_bound[0] += cost_init;
			// 判断路径中是否有服务器不在端点
			bool is_there_server = false;
			int node_num = nodes_on_path.size();
			for (int j = 0; j != server_num; ++j) {
				for (int k = 1; k != node_num; ++k) {
					if (nodes_on_path[k] == servers[j]) {
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
			if (push_num == 0) {
				median_segments[0] = nodes_on_path;
				distance_bounds[0] = distance_bound;
				dist_min2_max = distance_bound[0];
				dist_min2_min = distance_bound[0];
				push_num += 1;
			} else if (push_num < MAX_MEDIAN) {
				if (distance_bound[0] > dist_min2_min) {
					median_segments[1] = nodes_on_path;
					distance_bounds[1] = distance_bound;
					dist_min2_max = distance_bound[0];
				} else {
					median_segments[1] = median_segments[0];
					distance_bounds[1] = distance_bounds[0];
					median_segments[0] = nodes_on_path;
					distance_bounds[0] = distance_bound;
					dist_min2_min = distance_bound[0];
				}
				push_num += 1;
			} else {
				if (dist_min2_max > distance_bound[0]) {
					if(distance_bound[0] > dist_min2_min) {
						median_segments[1] = nodes_on_path;
						distance_bounds[1] = distance_bound;
						dist_min2_max = distance_bound[0];
					} else {
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

		for (int i = 0; i < push_num; i++){
			Route_states route;
			route.start_segment[0] = start_point;
			route.start_segment[1] = cust.agency;
			route.limit_entra = g.GetEdgeWithIndex(start_point, cust.agency)->band_width;
			if (!is_server) {
				route.traffic   = distance_bounds[i][0];
				if (distance_bounds[i][1] < route.limit_entra) {
					route.limit_all = distance_bounds[i][1];
				} else {
					route.limit_all = route.limit_entra;
				}
				route.median_segment = median_segments[i];
			} else {
				route.limit_all = route.limit_entra;
				route.median_segment = {};
			}
			routes.push_back(route);
		}
		edge = edge->next_edge;
	}

	return routes;
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
	Edge *edge = g.GetEdgeWithIndex(route_state.start_segment[0], route_state.start_segment[1]);//反向路径
	edge->band_width = edge->band_width - transfer;
	int mediean_edge_num = route_state.median_segment.size() - 1;
	for (int i = 0; i != mediean_edge_num; ++i) {
		edge = g.GetEdgeWithIndex(route_state.median_segment[i], route_state.median_segment[i + 1]);
		edge->band_width = edge->band_width - transfer;
	}
}

void PrintRouteStates(const Route_states & route)
{
	for (auto i : route.median_segment) {
		cout << i << " to ";
	}
	cout << route.start_segment[1] << endl;
	cout << "cost: " << route.traffic << endl;
	cout << "limit all: " << route.limit_all << endl;
	cout << "limit entra: " << route.limit_entra << endl;
}

/*选择路径并计算代价*/
void select_route(vector<Customer> customers,int servers[],Graph & g){

}

