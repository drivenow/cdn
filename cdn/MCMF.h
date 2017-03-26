/*
 * MCMF1.h
 *
 *  Created on: Mar 18, 2017
 *      Author: full
 */

#ifndef MCMF1_H_
#define MCMF1_H_

#include <queue>
#include <cstring>
#include <cstdio>
#include <string>
#include <limits>
#include <random>
#include <map>
using namespace std;
#define INF numeric_limits<int>::max()


struct record {
    int v, next, c;
};

extern const int MAXEDGE;
extern const int MAXNODE;
extern record edge[];
extern int flow[];
extern int pointer[];
extern int iter_flow[];
extern int iter_pointer[];
extern int cl;


void connect(record* edge, int flow[],int a, int b, int f, int c,int &cl,int pointer[]);
int dataLoad(record* edge,int flow[], char ** topo, int line_num, int &n ,int &s,int &t, int pointer[], int &cl, int &customer_demand);
void serverLoad(record* edge, int flow[], vector<int> servers, int s, int  pointer[], int cl);
bool spfa(record* edge, int flow[], vector<int> & path, int &ans, int pointer[], int n, int s,int t,map<int,int> &valid_server, map<int, vector<int>> &valid_node);
int findCost(record* edge, int flow[], vector<vector<int>> &paths, map<int,int> &valid_server, map<int, vector<int>> &valid_node, int pointer[], int customer_demand, int n, int s,int t,int &transfer_cost);

#endif /* MCMF1_H_ */
