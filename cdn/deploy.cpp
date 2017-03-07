#include "deploy.h"
#include "strlib.h"
#include <stdio.h>
#include <string>
using std::string;

#include <vector>
using std::vector;

#include <iostream>
using std::cout; using std::endl;

#include "graph.h"

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{

	int node_num, link_num, customer_num, server_unit_cost;
	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");
	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	cout << node_num << endl;
	cout << link_num << endl;
	cout << customer_num << endl;
	server_unit_cost = std::atoi(topo[2]);
	cout << server_unit_cost << endl;

	Graph g(node_num);
	g.CreateFromBuf(topo + 4, link_num);
	g.Print();
	// 需要输出的内容
	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
