#include "util.h"
#include "MCMF.h"
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
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
                for (unsigned j = 0; j < len; j++)
                {
                        if (j == len - 1)
                        {
                                output += to_string(agency_map[paths[i][1]]) + " " + to_string(paths[i][0]);
                        }
                        else
                        {
                                output += to_string(paths[i][len - 1 - j]) + " ";
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
void optimizer1(vector<vector<int>> paths,map<int,int> &valid_server, map<int,vector<int>> &valid_node , int &cost, map<int,int> &cust_demand, int &transfer_cost, int customer_demand, vector<int> &servers, int n ,int s, int t,int before_cl){
	//去除不往外传输流量的代理店
	int alone_agency_cost = 0;//删除后流量的大小
	map<int,int> valid_server_alone = valid_server;
	int alone_demand = 0,alone_cost = cost;

	for(map<int,int>::iterator it = valid_server_alone.begin(); it!=valid_server_alone.end(); it++){
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
}
