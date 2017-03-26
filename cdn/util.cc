#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <cmath>
#include "util.h"
#include "MCMF.h"
using std::vector;
using std::map;
using namespace std;



std::vector<std::string> str_split(const std::string &str, const std::string &sep)
{
        std::vector<std::string::size_type> posVec;
        auto pos = str.find(sep);
        while (pos != std::string::npos) {
                posVec.push_back(pos);
                pos = str.find(sep, pos + sep.length());
        }
        auto sepNum = posVec.size();
        if (sepNum == 0)
        {
                return{ str };
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
                else {
                        for (unsigned i = 0; i != sepNum - 1; ++i) {
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

string pathString(vector<vector<int>> paths, map<int, int> agency_map,bool find_so){
	string output = "";
	if (find_so==false){
		output="NA";
		return output;
	}
	output += to_string(paths.size())+"\n\n";
	for (auto i=0;i<paths.size();i++){
		auto len = paths[i].size();
		for (auto j=0;j<len;j++){
			if(j==len-1){
				output+= to_string(agency_map[paths[i][1]])+" "+to_string(paths[i][0]);
			}
			else{
				output+=to_string(paths[i][len-1-j])+" ";
			}
		}
		if(i!=paths.size()-1){
			output+="\n";
		}
	}
	return output;
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

vector<vector<int>> allCustomerNodes(record *edge, int *pointer, int cl, int t, map<int, int> agency_map, int search_layer){
	vector<vector<int>> custsNodes; //存放每个代理的服务器集合
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
		 custsNodes.push_back(custNodes);
	 }
	return custsNodes;

}
