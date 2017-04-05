#include "util.h"
#include "MCMF.h"
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
using namespace std;

std::vector<std::string> str_split(const std::string &str, const std::string &sep)
{
        std::vector<std::string::size_type> posVec;
        auto pos = str.find(sep);
        while (pos != std::string::npos)
        {
                posVec.push_back(pos);
                pos = str.find(sep, pos + sep.length());
        }
        auto sepNum = posVec.size();
        if (sepNum == 0)
        {
                return {str};
        }
        else
        {
                std::vector<std::string> strVec;
                if (posVec[0] != 0)
                {
                        strVec.push_back(str.substr(0, posVec[0]));
                }
                if (sepNum == 1 && posVec[0] + sep.length() != str.size())
                {
                        strVec.push_back(str.substr(posVec[0] + sep.length()));
                }
                else
                {
                        for (unsigned i = 0; i != sepNum - 1; ++i)
                        {
                                if (posVec[i + 1] - posVec[i] != 1)
                                {
                                        strVec.push_back(str.substr(posVec[i] + sep.length(), posVec[i + 1] - posVec[i] - 1));
                                }
                        }
                        if (posVec[0] + sep.length() != str.size())
                        {
                                strVec.push_back(str.substr(posVec[sepNum - 1] + sep.length()));
                        }
                }
                return strVec;
        }
}

string pathString(vector<vector<int>> paths, map<int, int> agency_map, bool find_so)
{
        string output = "";
        if (find_so == false)
        {
                output = "NA";
                return output;
        }
        output += to_string(paths.size()) + "\n\n";
        for (unsigned i = 0; i < paths.size(); i++)
        {
                auto len = paths[i].size();
                for (unsigned j = 1; j < len-1; j++)
                {
                        if (j == len - 2)
                        {
                                output += to_string(agency_map[paths[i][j-1]]) + " " + to_string(paths[i][len-1]);
                        }
                        else
                        {
                                output += to_string(paths[i][j]) + " ";
                        }
                }
                if (i != paths.size() - 1)
                {
                        output += "\n";
                }
        }
        return output;
}

void write_cost(char * filename, char * content){
	FILE *pf;//定义一个文件指针   大写通常定义的变量都是指针  封装好的_t才是整数类型

	pf = fopen(filename,"a+");//以读写的方式打开my文件
	if(pf == NULL)//如果文件空打开失败
	{
	perror("open");
	exit(1);
	}
	fputs(content,pf);//将 一行  字符串写入pf
	fputs("\n",pf);


	fclose(pf);
}

void print_vector(vector<int> vec){
	for(int i = 0; i<vec.size(); i++){
		cout<<vec[i]<<" ";
	}
	cout<<endl;
}
void optimizer1(vector<vector<int>> &paths,map<int,int> &valid_server, map<int,vector<int>> &valid_node , int &cost, map<int,int> &cust_demand, int &transfer_cost, int customer_demand, vector<int> &servers, int n ,int s, int t,int before_cl)
{
	//去除不往外传输流量的代理店
	int alone_agency_cost = 0;//删除后流量的大小
	map<int,int> valid_server_alone = valid_server;
	int alone_demand = 0,alone_cost = cost;

	for(map<int,int>::iterator it = valid_server_alone.begin(); it!=valid_server_alone.end(); it++){
		//不删除非直连点的服务器
		if(cust_demand.find(it->first) == cust_demand.end()){
			cout<<"find nearby"<<endl;
			continue;
		}
		if(it->second <= 2*cust_demand.find(it->first)->second){
			//代理店只给自己输送流量
			for(int i = 0;i<servers.size();i++){
				if(servers[i]==it->first){
					//删除该服务器
//					cout<<"erase:"<<it->first<<endl;
					servers.erase(servers.begin()+i);
//					print_vector(servers);
//					cout<<"server size"<<servers.size()<<endl;
				}
			}
			memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
			memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
			serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
			alone_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
			if(alone_demand!=0){
//				cout<<"important node!"<<it->first<<endl;
				servers.push_back(it->first);
				continue;
			}
//			cout<<"server size"<<servers.size()<<endl;
			alone_agency_cost = transfer_cost+server_price*valid_server.size();
			if(alone_agency_cost > alone_cost){
				servers.push_back(it->first);
			}
			else{
				cout<<"remove: "<<it->first<<", reduce cost:"<<cost-alone_agency_cost<<endl;
				alone_cost = alone_agency_cost;
			}
		}
	}



	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
	alone_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
	GetPath(paths, s,t , iter_flow, iter_pointer);
}

void optimizer2(vector<int> &nearby_server, vector<vector<int>> &paths,map<int,int> &valid_server, map<int,vector<int>> &valid_node , int &cost, map<int,int> &cust_demand, int &transfer_cost, int customer_demand, vector<int> &servers, int n ,int s, int t,int before_cl)
{
	//去除不往外传输流量的代理店
	int alone_agency_cost = 0;//删除后流量的大小
	map<int,int> valid_server_alone = valid_server;
	int alone_demand = 0,alone_cost = cost;

	for(map<int,int>::iterator it = valid_server_alone.begin(); it!=valid_server_alone.end(); it++){
		//不删除非直连点的服务器
		if(cust_demand.find(it->first) == cust_demand.end()){
			cout<<"find nearby"<<endl;
			continue;
		}
		if(it->second <= 2*cust_demand.find(it->first)->second){
			//代理点往外传输的流量较小
			for(int i = 0;i < servers.size();i++){
				if(servers[i]==it->first){
					//删除该服务器
//					cout<<"erase:"<<it->first<<endl;
					servers.erase(servers.begin()+i);
//					print_vector(servers);
//					cout<<"server size"<<servers.size()<<endl;
				}
			}
			memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
			memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
			serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
			alone_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
			if(alone_demand!=0){
//				cout<<"important node!"<<it->first<<endl;
				servers.push_back(it->first);
				continue;
			}
//			cout<<"server size"<<servers.size()<<endl;
			alone_agency_cost = transfer_cost+server_price*valid_server.size();
			if(alone_agency_cost > alone_cost){
				servers.push_back(it->first);
			}
			else{
				cout<<"remove: "<<it->first<<", reduce cost:"<<cost-alone_agency_cost<<endl;
				alone_cost = alone_agency_cost;
			}
		}
	}



	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
	alone_demand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);

}


//memset(vis, 0, sizeof (vis));
void customerNodes(record *edge, int *pointer, int cl, int t, int before_top, int top, map<int,int> &nb_nodes, int cnodes_dis[], int pc, int search_layer){
	int p, pv;
	//
	if(cnodes_dis[before_top] + pc < cnodes_dis[top]){
		cnodes_dis[top] = cnodes_dis[before_top] + pc;
	}
	nb_nodes[top] = cnodes_dis[top];


	//
	if(search_layer ==0 || top == t){
		return;
	}
	search_layer = search_layer-1;

	for(p=pointer[top]; p ; p = edge[edge[p].next].next){
		if(p > cl) {
			p = edge[p].next;
		}
		pv = edge[p].v;
		pc = abs(edge[p].c);
		customerNodes(edge, pointer, cl, t, top, pv, nb_nodes, cnodes_dis, pc, search_layer);
	}
}

map<int,vector<int>> allCustomerNodes(record *edge, int *pointer, int cl, int t, map<int, int> agency_map, int search_layer){
	map<int,vector<int>> custsNodes; //存放每个代理的服务器集合
	int cnodes_dis[MAXNODE];
	bool cnodes_vis[MAXNODE];
	map<int,int> nb_nodes;
	int p;
	cl = cl-2*agency_map.size();
	cout<<"cl:"<<cl<<endl;


	for (map<int, int>::iterator i=agency_map.begin(); i!=agency_map.end(); i++){
		 vector<int> custNodes; //存放单个代理的服务器集合
		 for (int i=0;i<MAXNODE;i++){
			 cnodes_dis[i] = INF;
			 cnodes_vis[i] = 0;
		 }
		 p = i->first;
		 cnodes_dis[p] = 0;

		 customerNodes(edge, pointer, cl, t, p, p, nb_nodes, cnodes_dis, 0, search_layer);

		 //
		 for (auto iter = nb_nodes.begin(); iter != nb_nodes.end(); iter++){
			 custNodes.push_back(iter->first);
			 custNodes.push_back(iter->second);
			 nb_nodes.erase(iter);
		 }
		 custsNodes[p] = custNodes;
	 }
	return custsNodes;

}

void findFlow(Graph &g, vector<vector<int>> &paths, map<int,int> &inTrans, map<int,int> &outFlow){
	int flow = 0;
	int cost = 0;
	int transfer = 0;
	int path_len = 0;
	int server,agency;
	vector<int>::iterator end_iter;
	//path (流量，终点。。起点)

	for(int i = 0;i<paths.size();i++){

		end_iter = paths[i].end();
		path_len = paths[i].size();
		agency = paths[i][1];
		server = *(end_iter-1);
		flow = paths[i][0];
		if(outFlow.find(server)==outFlow.end()){
			outFlow[server] = flow;
		}
		else{
			outFlow[server]+=flow;
		}
		//代理店就是服务器
		if(path_len == 2){
			if(inTrans.find(server)==inTrans.end()){
				inTrans[server] = 0;
			}
			else{
				inTrans[server] += 0;
			}
		}
		else{
			for(int j = path_len-1; j > 1; j--){
				cost = g.GetEdgeWithIndex(paths[i][j],paths[i][j-1])->unit_cost;

				transfer = cost*flow;
//				cout<<"transfer:"<<transfer<<"flow"<<flow<<endl;
				if(inTrans.find(paths[i][j-1])==inTrans.end()){
					inTrans[paths[i][j-1]] = transfer;
//					cout<<"init inTrans"<<inTrans[paths[i][j-1]]<<endl;
				}
				else{
					inTrans[paths[i][j-1]]+=transfer;
//					cout<<"plus inTrans"<<inTrans[paths[i][j-1]]<<endl;
				}
			}
		}
	}
}

void changeServers(vector<int> &servers, vector<int> in, vector<int> out){
	for(int i = 0; i <in.size();i++)
		servers.push_back(in[i]);
	for(int j =0;j<out.size();j++){
		for(int i = 0; i<servers.size(); i++){
			if(servers[i] ==out[j]){
				cout<<"remove"<<out[j]<<endl;
				servers.erase(servers.begin()+i);
				break;
			}
		}
	}
}

//增加一个服务器，找最优解
void repalce_find_cost(vector<int> &in, vector<int> &out, vector<vector<int>> &paths, map<int,int> &valid_server, vector<int> &servers, int &cost, int &transfer_cost, map<int,vector<int>> &valid_node, int customer_demand, int n, int s, int t, int before_cl)
{
	int trans_undemand = 0;
	changeServers(servers, in ,out);
	//重新找路
	memcpy(iter_pointer, pointer, MAXNODE*sizeof(int));
	memcpy(iter_flow, flow, MAXEDGE*sizeof(int));
	serverLoad(edge ,iter_flow, servers, s, iter_pointer, before_cl);
	trans_undemand = findCost(edge, iter_flow, paths, valid_server, valid_node, iter_pointer, customer_demand, n, s,t,transfer_cost);
	if(trans_undemand!=0){
		cout<<"find path error!"<<endl;
	}
	cost = transfer_cost+ server_price * valid_server.size();
//	cout<<"add "<<in[0]<<" transfer"<<transfer_cost<<" final cost:"<<cost<<" valid_server"<<valid_server.size()<<endl;

	//清空数组
	in.clear();
	out.clear();
}

//针对不同的替换组合，计算最优
//paths:当前最优路径
void deep_search(map<int,int> &delete_map, int &best_in, int search_layer, Graph &g, vector<vector<int>> &paths,  map<int,int> &valid_server, vector<int> &servers, int &cost, int &transfer_cost, map<int,vector<int>> &valid_node, int customer_demand, int n, int s, int t, int before_cl, map<int,int> cust_demand)
{

	if(node_num<=700){
		if(search_layer==0){
			return;
		}
		map<int,int> inTrans;
		map<int,int> outFlow;
		int trans_best_cost = 0;
		int trans_trans_cost = 0;
		int trans_undemand = 0;
		vector<vector<int>> trans_path;
		vector<int> init_servers = servers, iter_servers;
		map<int,int> trans_valid;
		vector<int> in,out;
		map<int,int> all_result;

	//	int replace_iter1 = 10;//当前路径下找多少点加入

		inTrans.clear();
		outFlow.clear();
		findFlow(g, paths, inTrans, outFlow);
//		for(map<int,int>::iterator it =  inTrans.begin();it !=  inTrans.end();it++){
//				cout<<it->first<<" "<<it->second<<endl;
//			}
//		cout<<"********************************************************"<<endl;
		map<int,int>::iterator in_iter;
		float rate = 0.1;
		for(map<int,int>::iterator it = inTrans.begin(); it!=inTrans.end();it++){
			//判断有无相邻边
			Edge * edge = g.vertices[it->first];
			while(edge!=nullptr){
				in_iter = inTrans.find(edge->index);
				if(in_iter!=inTrans.end()){
					//合并权重
					inTrans[it->first] = rate*in_iter->second +it->second;
//					cout<<"合并权重"<<it->second<<" "<<rate*in_iter->second<<endl;
				}
				edge = edge->next_edge;
			}
		}

		vector<pair<int,int>> inTrans_vec(inTrans.begin(), inTrans.end());
		vector<pair<int,int>> outFlow_vec(outFlow.begin(), outFlow.end());
		sort(inTrans_vec.begin(), inTrans_vec.end(), CmpByValue());
		sort(outFlow_vec.begin(), outFlow_vec.end(), CmpByValue());
//		for(int i = 0;i<inTrans_vec.size();i++){
//			cout<<inTrans_vec[i].first<<" "<<inTrans_vec[i].second<<endl;
//		}

//		if(best_in == -1){
//			wirte_inTrans(inTrans_vec);
//			cout<<"inTrans"<<endl;
//		}

		//找传输流量大的有无相连的
		vector<int> important_node;

		bool viz[node_num];
		for(int i = 0; i<int(inTrans_vec.size()); i++){
			if(inTrans_vec[i].second > 0.4*server_price){
				important_node.push_back(inTrans_vec[i].first);
				viz[inTrans_vec[i].first] = 1;
			}
		}
	//	print_vector(important_node);
		for(int j = 0; j<important_node.size();j++){
			if(delete_map.find(important_node[j])!=delete_map.end()){
				//差结果，删除
				continue;
			}
			in.push_back(important_node[j]);
			iter_servers = init_servers;
			repalce_find_cost(in, out, trans_path, trans_valid, iter_servers, trans_best_cost, trans_trans_cost, valid_node,customer_demand,n,s,t,before_cl);
	//		optimizer1(trans_path, trans_valid, valid_node , trans_best_cost, cust_demand, trans_trans_cost, customer_demand, iter_servers, n , s, t, before_cl);
			trans_best_cost = trans_trans_cost+ server_price * trans_valid.size();
			all_result[important_node[j]] = trans_best_cost;//每个节点的cost保留
			//替换最优解
			if(j==0){
				servers = iter_servers;
				cost = trans_best_cost;
				transfer_cost = trans_trans_cost ;
				paths = trans_path;
				valid_server = trans_valid;
				best_in = important_node[j];
				cout<<"*********change best**********"<<trans_best_cost<<endl;
			}
			if(trans_best_cost < cost){
				servers = iter_servers;
				cost = trans_best_cost;
				transfer_cost = trans_trans_cost ;
				paths = trans_path;
				valid_server = trans_valid;
				best_in = important_node[j];
				cout<<"*********change best**********"<<trans_best_cost<<endl;
			}

	//		cout<<"22222222"<<search_layer<<" search transfer"<<trans_trans_cost<<" search final1 cost:"<<trans_best_cost<<" search valid_server"<<trans_valid.size()<<endl;
		}
		vector<pair<int,int>> all_result_vec(all_result.begin(), all_result.end());
		sort(all_result_vec.begin(), all_result_vec.end(), CmpByValue());
		//只保留最优的十个作为下次搜索

		for(int i= 0; i<all_result_vec.size()*0.1;i++){
			delete_map[all_result_vec[i].first] = 0;
		}
	}
	else{
		int count = 0;
		if (search_layer == 0){
			return;
		}
		map<int, int> inTrans;
		map<int, int> outFlow;
		int trans_best_cost = 0;
		int trans_trans_cost = 0;
		int trans_undemand = 0;
		vector<vector<int>> trans_path;
		vector<int> init_servers = servers, iter_servers;
		map<int, int> trans_valid;
		vector<int> in, out;
		map<int, int> all_result;

		//	int replace_iter1 = 10;//当前路径下找多少点加入

		inTrans.clear();
		outFlow.clear();
		findFlow(g, paths, inTrans, outFlow);
		map<int, int>::iterator in_iter;
		float rate = 0.3;
		for (map<int, int>::iterator it = inTrans.begin(); it != inTrans.end(); it++){
			//判断有无相邻边
			Edge * edge = g.vertices[it->first];
			while (edge != nullptr){
				in_iter = inTrans.find(edge->index);
				if (in_iter != inTrans.end()){
					//合并权重
					inTrans[it->first] = rate*in_iter->second + it->second;
				}
				edge = edge->next_edge;
			}
		}

		vector<pair<int, int>> inTrans_vec(inTrans.begin(), inTrans.end());
		vector<pair<int, int>> outFlow_vec(outFlow.begin(), outFlow.end());
		sort(inTrans_vec.begin(), inTrans_vec.end(), CmpByValue());
		sort(outFlow_vec.begin(), outFlow_vec.end(), CmpByValue());
		//	for(int i = 0;i<inTrans_vec.size();i++){
		//		cout<<inTrans_vec[i].first<<" "<<inTrans_vec[i].second<<endl;
		//	}

		if (best_in == -1){
			//		wirte_inTrans(inTrans_vec);
			cout << "inTrans" << endl;
		}

		//找传输流量大的有无相连的
		vector<int> important_node;

		bool viz[node_num];
		for (int i = 0; i<int(inTrans_vec.size()); i++){
			if (inTrans_vec[i].second > 0.7*server_price){
				important_node.push_back(inTrans_vec[i].first);
				viz[inTrans_vec[i].first] = 1;
			}
		}
		//	print_vector(important_node);
		for (int j = 0; j<important_node.size()*0.9; j++){
			if(count>=3) break;
			if (delete_map.find(important_node[j]) != delete_map.end()){
				//差结果，删除
				continue;
			}
			in.push_back(important_node[j]);
			iter_servers = init_servers;
			repalce_find_cost(in, out, trans_path, trans_valid, iter_servers, trans_best_cost, trans_trans_cost, valid_node, customer_demand, n, s, t, before_cl);
			//		optimizer1(trans_path, trans_valid, valid_node , trans_best_cost, cust_demand, trans_trans_cost, customer_demand, iter_servers, n , s, t, before_cl);
			trans_best_cost = trans_trans_cost + server_price * trans_valid.size();
			all_result[important_node[j]] = trans_best_cost;//每个节点的cost保留
			//替换最优解, cost is &
			// if (j == 0){
			// 	servers = iter_servers;
			// 	cost = trans_best_cost;
			// 	transfer_cost = trans_trans_cost;
			// 	paths = trans_path;
			// 	valid_server = trans_valid;
			// 	best_in = important_node[j];
			// 	cout << "*********change best**********" << trans_best_cost << endl;
			// 	count++;
			// }
			if (trans_best_cost < cost){
				servers = iter_servers;
				cost = trans_best_cost;
				transfer_cost = trans_trans_cost;
				paths = trans_path;
				valid_server = trans_valid;
				best_in = important_node[j];
				cout << "*********change best**********" << trans_best_cost << endl;
				count++;
			}

			//		cout<<"22222222"<<search_layer<<" search transfer"<<trans_trans_cost<<" search final1 cost:"<<trans_best_cost<<" search valid_server"<<trans_valid.size()<<endl;
		}
		vector<pair<int, int>> all_result_vec(all_result.begin(), all_result.end());
		sort(all_result_vec.begin(), all_result_vec.end(), CmpByValue());
		//只保留最优的十个作为下次搜索

		for (int i = 0; i<all_result_vec.size()*0.2; i++){
			delete_map[all_result_vec[i].first] = 0;
		}

	}
//	cout<<"******************search transfer"<<transfer_cost<<" search final1 cost:"<<cost<<" search valid_server"<<valid_server.size()<<endl;
}

void wirte_inTrans(vector<pair<int,int>> inTrans_vec){
	char *filePath1 = "/home/full/Desktop/inTrans.txt";
	FILE *pf;//定义一个文件指针   大写通常定义的变量都是指针  封装好的_t才是整数类型

	pf = fopen(filePath1,"a+");//以读写的方式打开my文件
	if(pf == NULL)//如果文件空打开失败
	{
	perror("open");
	exit(1);
	}
	for(int i = 0; i<inTrans_vec.size(); i++){
		char cost_char[20];
		sprintf(cost_char,"%d",inTrans_vec[i].first);
		fputs(cost_char,pf);//将 一行  字符串写入pf
		fputs(" ",pf);

		sprintf(cost_char,"%d",inTrans_vec[i].second);
		fputs(cost_char,pf);//将 一行  字符串写入pf
		fputs("\n",pf);
	}
	fclose(pf);
}


