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

#include <algorithm>

#define MAX_MEDIAN 4 //对同一个start_segment选择几条median_segment路线


void SortRoute_states(std::vector<Route_states> & routes)
{
	std::sort(routes.begin(), routes.end(), [](const Route_states & a, const Route_states & b){
		return a.traffic < b.traffic;
	});
}

/*
 * 返回从消费者出发到服务器的最短路径
 * Customer cust:单个消费者节点
 * Servers：服务器列表
 * */
vector<Route_states> get_route(Graph & g, const Customer & cust, const vector<int> & servers)
{
	Edge *edge = g.vertices[cust.agency];
	std::vector<Route_states> routes;
	int server_num = servers.size();

	while (edge != nullptr) {
		int start_point = edge->index;    //代理上一个点
		int cost_init = g.GetEdgeWithIndex(start_point, cust.agency)->unit_cost;  //起始代价（代理上一个点到代理的代价）
		std::vector<vector<int>> median_segments;  //临时储存到每个服务器的路径
		std::vector<vector<int>> distance_bounds;

		//int dist_min2_max;
		//int dist_min2_min;

		int push_num = 0;  //加到median_segments中的数据个数

		int is_server = false;

		for (int i = 0; i < server_num; i++) {
			if (start_point == servers[i]) {
				is_server = true;
				push_num += 1;
				continue;
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
			if (push_num < MAX_MEDIAN) {
				median_segments.push_back(nodes_on_path);
				distance_bounds.push_back(distance_bound);
				push_num += 1;
			} else {
				auto iter_max_distance = std::max_element(distance_bounds.begin(), distance_bounds.end(),
						[](const vector<int> & a, const vector<int> & b) {
						return a[0] < b[0];
				});
				auto index_max_distance = iter_max_distance - distance_bounds.begin();
				if ((*iter_max_distance)[0] > distance_bound[0]) {
					*iter_max_distance = distance_bound;
					median_segments[index_max_distance] = nodes_on_path;
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
				route.traffic = cost_init;
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
 * 返回单条路径传输的代价,同时更新邻接矩阵
 *
 * int start_point: 消费者节点
 * agency_point: 消费者节点上溯一个点的代理节点
 * int demand： 消费者的需求
 * Route_states route_state： 消费者到服务器的路径
 * */
vector<int> get_route_traffic(Customer & cust, const Route_states & route_state, Graph & g)
{
	vector<int> traffic_transfer(2);
	// 数据传输量，取消费者需求和路径供给上限的较小值
	int transfer = (route_state.limit_all < cust.demand) ? route_state.limit_all : cust.demand;

	// 更新消费者需求
	cust.demand -= transfer;
	update_Graph(g, route_state, transfer);        //更新邻接矩阵
	int traffic_num = route_state.traffic * transfer;  // 传输代价

	traffic_transfer[0] = traffic_num;
	traffic_transfer[1] = transfer;
	return traffic_transfer;
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
	if (mediean_edge_num != -1) {
		for (int i = 0; i != mediean_edge_num; ++i) {
			edge = g.GetEdgeWithIndex(route_state.median_segment[i], route_state.median_segment[i + 1]);
			edge->band_width = edge->band_width - transfer;
		}
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

/*选择路径并计算代价
 *int & total_traffic: 总的传输代价
 *int vector<Route_transfer> route_transfer： 总的传输路径
 **/
void select_route(vector<Customer> & customers, const vector<int> & servers,Graph & g,int & total_traffic,vector<Route_transfer> & route_transfers){

	//int customer_num = customers.size();
	//Customer customer;

	for (auto & customer : customers){
		//customer = customers[i];
		cout << "------------------" << endl;
		cout << "demand: " << customer.demand << endl;
		vector<Route_states> routes = get_route(g, customer, servers);
		SortRoute_states(routes);
		//cout << "before while" << endl;
		int route_num = routes.size();

		for (int i = 0; i != route_num; ) {
		//while(customer.demand>0 && route_num<routes.size()){
			//cout << "within while" << endl;
			if (i > 0) {
				routes[i].limit_entra = g.get_edge(routes[i].start_segment[0], customer.agency)->band_width;
				if (routes[i].median_segment.empty()) {
					routes[i].limit_all = routes[i].limit_entra;
				} else {
					vector<int> distance_bound = g.RetrieveDistanceBound(routes[i].median_segment);
					if (routes[i].limit_entra > distance_bound[1]) {
						routes[i].limit_all = distance_bound[1];//有共享路段时，更新上限
					} else {
						routes[i].limit_all = routes[i].limit_entra;
					}
				}
			}
			if (routes[i].limit_all == 0) {
				i += 1;
				continue;
			}
			//routes[i];
			//cout << "within while" << endl;
			vector<int> traffic_transfer = get_route_traffic(customer, routes[i], g);
			//cout << "within while" << endl;
			total_traffic+=traffic_transfer[0];
			Route_transfer route_transfer;
			//cout << "within while" << endl;
			route_transfer.segment = routes[i].median_segment;
			route_transfer.segment.push_back(routes[i].start_segment[1]);
			route_transfer.segment.push_back(customer.start);
			route_transfer.transfer = traffic_transfer[1];
			route_transfers.push_back(route_transfer);
			i += 1;

			cout<< "demand: "  << customer.demand << endl;
			cout<< "traffic: " << traffic_transfer[0] << endl;
			cout<< "transfer: "<< traffic_transfer[1] << endl;
			for (const auto & j : route_transfer.segment){
				cout << j << " to ";
			}
			cout << endl;
			if (customer.demand == 0) {
				break;
			}
		}
	}
}

