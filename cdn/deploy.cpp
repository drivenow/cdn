#include "deploy.h"
#include "lib_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <limits>
#include <chrono>
#include <random>
#include <algorithm>
#include <iterator>
#include <fstream>
#include "server.h"
#include "util.h"
#include "MCMF.h"
using namespace std;

#define SETZR(a) memset(a, 0, sizeof(a))
#define INF std::numeric_limits<int>::max()

std::chrono::time_point<std::chrono::system_clock> start, tend;
std::chrono::duration<double> elapsed_seconds;

void PrintPath(const vector<int> &nodes_on_path)
{
	for (auto &index : nodes_on_path)
	{
		cout << index << " ";
	}
	cout << endl;
}

void PrintCustomer(const Customer &customer)
{
	cout << "index: " << customer.index << endl;
	cout << "demand: " << customer.demand << endl;
}

bool is_sln_found(int demand, const vector<vector<int>> &paths)
{
	int path_num = paths.size();
	for (int i = 0; i != path_num; i++)
	{
		demand -= paths[i][0];
	}
	return demand == 0 ? true : false;
}

int SA(vector<vector<int>> &best_paths_all, map<int, int> &best_servers_all,
       bool &found_all, int node_num, char **topo, int link_num,
       const map<int, int> &agency_map, int server_price, int line_num)
{
	int server_num = 0;
	int best_cost_all = INF;

	NetGraph g(node_num);
	g.CreateFromBuf(topo + 4, g, link_num); // generate graph info

	for (int i = 1; i != node_num; ++i)
	{
		server_num += 1;

		ServerSelector selector(server_num, node_num);

		// 服务器集合
		// vector<int> servers{ 37, 35, 14, 44, 38, 22};
		selector.GenEvalueSheet(g, selector.evalue_sheet, agency_map);
		selector.Select(selector.evalue_sheet);
		vector<int> servers = selector.get_servers();
		// selector.PrintServers();

		//******************************************************
		//寻路
		int m, n, s, t;
		int pointer[10000];
		// SETZR(pointer);
		// vector<vector<int>> paths;
		// bool find_so = true;

		// int customer_demand = dataLoad(topo, servers, line_num, m, n, s, t, pointer);

		// int cost = findCost(paths, pointer, n, s, t);

		// cout << customer_demand << endl;
		// for (auto i = 0; i < paths.size(); i++)
		// {
		// 	customer_demand -= paths[i][0];
		// 	cout << customer_demand << endl;
		// }
		// if (customer_demand != 0)
		// {
		// 	cout << customer_demand << endl;
		// 	cout << "no solution!!" << endl;
		// 	find_so = false;
		// }

		int cur_cost;
		int pre_cost = INF;
		int best_cost = INF;
		vector<vector<int>> best_paths;
		map<int, int> best_servers;
		int customer_demand;
		//TODO:
		//T与delta的选择应该 考虑 de/t之后的值，确保能有多少方案可以变异。
		//同时T的detla次方衰减到t_time 需要的次数与问题规模相匹配；
		//要考虑备选服务器数量。需要迭代次数与备选服务器数量近似。

		int de = -1;
		double T = 1000, delta = 0.99, T_time = 80;
		bool found = false;

		while (T > T_time)
		{
			map<int, int> valid_servers;
			SETZR(pointer);
			customer_demand = dataLoad(topo, servers, line_num, m, n, s, t, pointer);
			vector<vector<int>> paths;

			cur_cost = findCost(paths, valid_servers, pointer, n, s, t);
			cur_cost += server_price * valid_servers.size();

			if (is_sln_found(customer_demand, paths))
			{
				found = true;
				found_all = true;
			}
			else
			{
				cur_cost = INF;
			}

			de = pre_cost - cur_cost;

			if (de > 0 && is_sln_found(customer_demand, paths))
			{
				//接受该次改变，servers为上次mutate后的servers。
				best_cost = cur_cost;
				best_paths = paths;
				best_servers = valid_servers;
				//pre_servers = selector.servers;
			}
			else
			{
				if (exp(de / T) < (1 + rand() % 9999) / 10000.0)
				{
					//计算退火概率，以一定机率改变selector.servers为best_servers;
					selector.Rollback();
				}
			}
			pre_cost = cur_cost;
			selector.Mutate();
			servers = selector.get_servers();
			// selector.PrintServers();
			T *= delta;
		}

		if (found)
		{
			if (best_cost < best_cost_all)
			{
				best_cost_all = best_cost;
				best_paths_all = best_paths;
				best_servers_all = best_servers;
			}
			// else if ((best_cost - best_cost_all) > server_price)
			// {
			// 	break;
			// }
		}

		tend = std::chrono::system_clock::now();
		elapsed_seconds = tend - start;

		if (elapsed_seconds.count() > 40)
		{
			break;
		}
	}
	return best_cost_all;
}

bool in(int i, const vector<int> &ivec)
{
	for (const auto &e : ivec)
	{
		if (i == e)
		{
			return true;
		}
	}

	return false;
}

bool is_subset_of(const vector<int> &a, const vector<int> &u)
{
	for (const auto &i : a)
	{
		if (!in(i, u))
		{
			return false;
		}
	}

	return true;
}

vector<float> operator*(float factor, const vector<int> &rhs)
{
	vector<float> fvec;
	for (const int &i : rhs)
	{
		fvec.push_back(factor * i);
	}

	return fvec;
}

vector<float> operator*(float factor, const vector<float> &rhs)
{
	vector<float> fvec;
	for (const int &f : rhs)
	{
		fvec.push_back(factor * f);
	}

	return fvec;
}

vector<float> operator+(const vector<float> &lhs, const vector<float> &rhs)
{
	vector<float> fvec;
	int num = lhs.size();

	for (auto i = 0; i != num; ++i)
	{
		fvec.push_back(lhs[i] + rhs[i]);
	}

	return fvec;
}

vector<float> operator+(const vector<int> &lhs, const vector<float> &rhs)
{
	vector<float> fvec;
	int num = lhs.size();

	for (auto i = 0; i != num; ++i)
	{
		fvec.push_back(static_cast<float>(lhs[i]) + rhs[i]);
	}

	return fvec;
}

vector<int> operator-(const vector<int> &lhs, const vector<int> &rhs)
{
	vector<int> ivec;
	int num = lhs.size();

	for (auto i = 0; i != num; ++i)
	{
		ivec.push_back(lhs[i] - rhs[i]);
	}

	return ivec;
}

float sig(float f)
{
	return 1.0 / (1 + std::exp(-f));
}

vector<int> round(const vector<float> &fvec)
{
	vector<int> ivec;
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.5, 1);
	for (const float &f : fvec)
	{
		if (dis(gen) < sig(f))
		{
			ivec.push_back(1);
		}
		else
		{
			ivec.push_back(0);
		}

		// if (abs(f) < abs(f - 1))
		// {
		// 	ivec.push_back(0);
		// }
		// else
		// {
		// 	ivec.push_back(1);
		// }
	}

	return ivec;
}

void mutate(vector<int> &position)
{
	int len = position.size();
	static default_random_engine e;
	static uniform_int_distribution<int> dis(0, len - 1);
	int k;
	for (int i = 0; i != len / 3; ++i)
	{
		k = dis(e);
		position[k] = position[k] ? 0 : 1;
	}
}

int PSO(int iteration_cnt_max, int particle_num, int node_num,
	const vector<int> &agencies, char **topo, int line_num,
	int server_price, vector<vector<int>> &best_paths_all, bool &found_all)
{
	// genrate particles randomly
	vector<vector<int>> particles;
	int server_num, index;
	int b = static_cast<int>(sqrt(node_num));
	static std::default_random_engine e;
	static std::uniform_int_distribution<int> d(0, node_num - 1);
	for (int i = 0; i != particle_num;)
	{
		server_num = d(e);
		vector<int> servers;
		for (int j = 0; j != server_num; ++j)
		{
			do
			{
				index = d(e);
			} while (in(index, servers));
			servers.push_back(index);
		}

		if (is_subset_of(agencies, servers))
		{
			continue;
		}
		else
		{
			particles.push_back(servers);
			i += 1;
		}
	}

	vector<vector<int>> positions;
	for (const vector<int> &particle : particles)
	{
		vector<int> position(node_num, 0);
		for (const int &i : particle)
		{
			position[i] = 1;
		}
		positions.push_back(std::move(position));
	}

	float c = 1.4961;
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static uniform_real_distribution<float> dis(0, 1);
	float r1 = dis(gen);
	float r2 = dis(gen);
	float inertia_weight = 0.99;
	float gap = 0.7 / iteration_cnt_max;

	int customer_demand;
	vector<vector<float>> velocities;
	for (int i = 0; i != particle_num; ++i)
	{
		vector<float> tmp(node_num, 0.0);
		velocities.push_back(std::move(tmp));
	}
	vector<vector<int>> pbests;
	for (int i = 0; i != particle_num; ++i)
	{
		vector<int> tmp(node_num, 0);
		pbests.push_back(std::move(tmp));
	}
	vector<int> gbest;
	vector<int> pbests_costs(particle_num, INF);
	int gbest_cost = INF;
	vector<vector<vector<int>>> paths_all(particle_num);
	// std::ofstream file("cdn.log");

	for (int i = 0; i != iteration_cnt_max; ++i)
	{
		// compute fitness
		int cost;
		vector<int> costs;
		for (int j = 0; j != particle_num; ++j)
		{
			map<int, int> valid_servers;
			int pointer[10000], m, n, s, t;
			SETZR(pointer);
			customer_demand = dataLoad(topo, particles[j], line_num, m, n, s, t, pointer);
			vector<vector<int>> paths;

			cost = findCost(paths, valid_servers, pointer, n, s, t);
			cost += server_price * valid_servers.size();

			if (!is_sln_found(customer_demand, paths))
			{
				cost = INF;
			}
			else
			{
				found_all = true;
			}

			costs.push_back(cost);

			if (cost < pbests_costs[j]) // update pbest
			{
				pbests_costs[j] = cost;
				pbests[j] = positions[j];
				paths_all[j] = paths;
			}
		}

		auto iter = std::min_element(pbests_costs.begin(), pbests_costs.end());

		if (*iter < gbest_cost) // update gbest
		{
			gbest_cost = *iter;
			gbest = positions[std::distance(pbests_costs.begin(), iter)];
			best_paths_all = paths_all[std::distance(pbests_costs.begin(), iter)];
		}

		cout << gbest_cost << endl;

		for (int j = 0; j != particle_num; ++j) // update velocity
		{
			velocities[j] = inertia_weight * velocities[j] + c * r1 * (pbests[j] - positions[j]) + c * r2 * (gbest - positions[j]);
		}

		for (int j = 0; j != particle_num; ++j) // update particles
		{
			vector<float> tmp = positions[j] + velocities[j];
			// cout << "after +" << endl;
			positions[j] = round(tmp);
			// if (dis(gen) < 0.5)
			// {
			// 	mutate(positions[j]);
			// }
			particles[j].clear();
			for (int k = 0; k != node_num; ++k)
			{
				if (positions[j][k] == 1)
				{
					particles[j].push_back(k);
				}
			}
		}

		// file << "particles:" << endl;
		// for (int i = 0; i != particle_num; ++i)
		// {
		// 	for (const int &j : particles[i])
		// 	{
		// 		file << j << ' ';
		// 	}
		// 	file << endl;
		// }

		inertia_weight -= gap;

		r1 = dis(gen);
		r2 = dis(gen);

		tend = std::chrono::system_clock::now();
		elapsed_seconds = tend - start;

		if (elapsed_seconds.count() > 10)
		{
			break;
		}
	}

	// for (int i = 0; i != node_num; ++i)
	// {
	// 	file << gbest[i] << ' ';
	// }
	// file << endl;

	// file.close();

	return gbest_cost;
}

//你要完成的功能总入口
void deploy_server(char *topo[MAX_EDGE_NUM], int line_num, char *filename)
{

	start = std::chrono::system_clock::now();

	int node_num, link_num, customer_num, server_price;

	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");

	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	server_price = std::atoi(topo[2]);

	cout << "number of nodes:\t" << node_num << endl;
	cout << "number of links:\t" << link_num << endl;
	cout << "number of customers:\t" << customer_num << endl;
	cout << "cost of a server:\t" << server_price << endl;

	// customers
	vector<Customer> customers(customer_num);
	map<int, int> agency_map;
	vector<int> agencies;
	char **base_pos = topo + 4 + link_num + 1;
	vector<string> numbers;
	for (int i = 0; i < customer_num; i++)
	{
		string line(base_pos[i]);
		numbers = str_split(line, " ");
		customers[i].index = std::stoi(numbers[0]);
		customers[i].agency = std::stoi(numbers[1]);
		customers[i].demand = std::stoi(numbers[2]);
		agency_map[customers[i].agency] = customers[i].index;
		agencies.push_back(customers[i].agency);
	}

	int base_price = customer_num * server_price;

	cout << "base_price:\t" << base_price << endl;

	// srand(time(NULL));
	bool found_all = false;
	vector<vector<int>> best_paths_all;
	int min_price = PSO(INF, 40, node_num, agencies, topo, line_num,
			    server_price, best_paths_all, found_all);

	cout << "min_price:\t" << min_price << endl;
	// map<int, int> best_servers_all;
	// int best_cost_all;

	// best_cost_all = SA(best_paths_all, best_servers_all, found_all,
	// 		   node_num, topo, link_num, agency_map,
	// 		   server_price, line_num);

	// if (found_all)
	// {
	// 	cout << "server\tflow" << endl;
	// 	for (const auto &s : best_servers_all)
	// 	{
	// 		cout << s.first << '\t' << s.second << endl;
	// 	}
	// 	cout << "cost min:\t" << best_cost_all << endl;
	// }
	string output = pathString(best_paths_all, agency_map, found_all);
	char *topo_file = new char[output.size() + 1];
	std::copy(output.begin(), output.end(), topo_file);
	topo_file[output.size()] = '\0'; // don't forget the terminating 0

	//直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
	delete[] topo_file;
}
