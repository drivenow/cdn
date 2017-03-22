#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include "server.h"

using namespace std;

#define MAXOUTDEG 1000
#define MAXCUSNUM 500

void NetGraph::CreateFromBuf(char **buf, NetGraph &netgraph, int line_num)
{
	//vector<string> lines;
	//int comp = 0;
	for (int i = 0; i != line_num; ++i)
	{
		string line(buf[i]);
		//lines.push_back(line);
		vector<string> numbers = str_split(line, " ");
		int src = std::stoi(numbers[0]);
		int dst = std::stoi(numbers[1]);
		int band_width = std::stoi(numbers[2]);
		int unit_cost = std::stoi(numbers[3]);

		Edge *edge_src = new Edge, *edge_dst = new Edge;
		edge_src->index = dst;
		edge_dst->index = src;
		edge_src->band_width = edge_dst->band_width = band_width;
		edge_src->unit_cost = edge_dst->unit_cost = unit_cost;
		edge_src->next_edge = edge_dst->next_edge = nullptr;
		if (netgraph.vertices[src] == nullptr)
		{
			netgraph.vertices[src] = edge_src;
		}
		else
		{
			edge_src->next_edge = netgraph.vertices[src];
			netgraph.vertices[src] = edge_src;
		}
		if (netgraph.vertices[dst] == nullptr)
		{
			netgraph.vertices[dst] = edge_dst;
		}
		else
		{
			edge_dst->next_edge = netgraph.vertices[dst];
			netgraph.vertices[dst] = edge_dst;
		}
		netgraph.edge_num++;
	}
}

void ServerSelector::GenEvalueSheet(const NetGraph &g, vector<structnode> &evalu_sheet, const map<int, int> &agency_map)
{

	int node_num = server_num + backup_num;
	int out_degree, nerb_custnum;
	int max_outdeg = 1;
	int max_nerb_custnum = 0;
	int min_outdeg = MAXOUTDEG;
	int min_nerb_custnum = MAXCUSNUM;
	Edge *edge = nullptr;
	evalu_sheet.resize(node_num);

	for (int i = 0; i < node_num; i++)
	{
		out_degree = 0;
		nerb_custnum = 0;
		edge = g.vertices[i];
		while (edge != nullptr)
		{
			out_degree += 1;
			if (agency_map.find(edge->index) != agency_map.end())
				nerb_custnum += 1;
			edge = edge->next_edge;
		}
		evalu_sheet[i].index = i;
		evalu_sheet[i].out_degree = out_degree;
		evalu_sheet[i].nerb_agencynum = nerb_custnum;
		if (out_degree > max_outdeg)
			max_outdeg = out_degree;
		if (out_degree < min_outdeg)
			min_outdeg = out_degree;
		if (nerb_custnum > max_nerb_custnum)
			max_nerb_custnum = nerb_custnum;
		if (nerb_custnum < min_nerb_custnum)
			min_nerb_custnum = nerb_custnum;
	}
}

void ServerSelector::SortCustomersByAgency(vector<Customer> &customers)
{
	std::sort(customers.begin(), customers.end(), [](const Customer &a, const Customer &b) {
		return a.agency < b.agency;
	});
}

//根据evalu_sheet 选择服务器
void ServerSelector::Select(vector<structnode> &evalu_sheet)
{

	std::sort(evalu_sheet.begin(), evalu_sheet.end(), [](const structnode &a, const structnode &b) {
		return a.out_degree > b.out_degree;
	});

	servers.insert(servers.end(), evalu_sheet.begin(), evalu_sheet.begin() + server_num);
	backups.insert(backups.end(), evalu_sheet.begin() + server_num, evalu_sheet.end());
}

//分别给servers 和 backups 一个参考了节点出度比例的随机数，并进行替换
//server中节点出度分别为 [a,b,c,d] 替换a的概率p=1-(a-min(server)+1)/(sum(server)-(min(server-1))*4);
void ServerSelector::Mutate(void)
{

	// retrieve the server with minimal out_degree
	// and the sum of out_degrees of all the servers
	int server_min_pos = 0, server_sum = 0, out_degree;
	for (int i = 0; i != server_num; ++i)
	{
		out_degree = servers[i].out_degree;
		server_sum += out_degree;
		if (servers[server_min_pos].out_degree > out_degree)
		{
			server_min_pos = i;
		}
	}

	vector<double> server_probabilities;
	double probability;
	int out_degree_min = servers[server_min_pos].out_degree;
	for (int i = 0; i != server_num; ++i)
	{
		probability = (1 + rand() % 30) / 31.0;
		probability *= servers[i].out_degree - out_degree_min + 1;
		probability /= server_sum - server_num * (out_degree_min - 1);
		server_probabilities.push_back(probability);
	}

	auto iter_min = std::min_element(server_probabilities.begin(), server_probabilities.end());
	int server_substitute_pos = iter_min - server_probabilities.begin();
	//最可能被替换servers数组下标
	//float minpro_sev = rand() % 30 / (float) 31 * (servers[0].out_degree - (*minit).server_outdeg +1) / (servers_outdegsum-server_num*((*minit).server_outdeg-1))*1.0;
	//TODO: 如果severs 只有一个元素下面循环会不会越界

	int backup_min_pos = 0, backup_sum = 0;
	for (int i = 0; i != backup_num; ++i)
	{
		out_degree = backups[i].out_degree;
		backup_sum += out_degree;
		if (backups[backup_min_pos].out_degree > out_degree)
		{
			backup_min_pos = i;
		}
	}

	vector<double> backup_probabilities;
	out_degree_min = backups[backup_min_pos].out_degree;
	for (int i = 0; i != backup_num; ++i)
	{
		probability = (1 + rand() % 30) / 31.0;
		probability *= backups[i].out_degree - out_degree_min + 1;
		probability /= backup_sum - backup_num * (out_degree_min - 1);
		server_probabilities.push_back(probability);
	}

	auto iter_max = std::max_element(backup_probabilities.begin(), backup_probabilities.end());
	int backup_substitute_pos = iter_max - backup_probabilities.begin();

	std::swap(servers[server_substitute_pos], backups[backup_substitute_pos]);
	server_rollid = server_substitute_pos;
	backup_rollid = backup_substitute_pos;
}

void ServerSelector::Rollback(void)
{
	cout << "rollback:\t" << server_rollid << " " << backup_rollid << endl;
	std::swap(servers[server_rollid], backups[backup_rollid]);
}

//从servers 中提取servers的ID
//返回值： vector<int> serversid
vector<int> ServerSelector::get_servers(void)
{
	vector<int> indices;
	for (const auto &server : servers)
	{
		indices.push_back(server.index);
	}
	return indices;
}

void ServerSelector::PrintServers(void)
{
	//cout << servers.size() << endl;
	cout << "id\tout_degree" << endl;
	for (const auto &server : servers)
	{
		cout << server.index << '\t' << server.out_degree << endl;
	}
}
