/*
 * route_traffic.h
 *
 *  Created on: Mar 6, 2017
 *      Author: full
 */
#include <vector>
#include <graph.h>
using namespace std;

#ifndef CDN_ROUTETRAFFIC_H_
#define CDN_ROUTETRAFFIC_H_
#define MAX_SEGMENT 6

struct Route_states{
	int start_segment[2];//路径开始段
	std::vector<int> median_segment;//路径连接到服务器的中间段（反向）,注意搜索路径时从消费者到服务器（反向），更新邻接阵时从服务器到消费者（正向）
	int traffic; //整段路径的传输单价
	int limit_all;//路径的流量上限
	int limit_entra;//入口段的流量上限
};


struct Customer_route{
	int start_point;//消费者节点
	int agency_point;//消费者节点的代理节点
	vector<Route_states> route_states;//多条路径信息
};

int get_traffic_edge(Customer cust, Graph g);

#endif /* CDN_ROUTETRAFFIC_H_ */
