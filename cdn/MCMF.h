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
using namespace std;


void connect(int a, int b, int f, int c,int &cl,int * pointer);
int dataLoad(char ** topo, vector<int> servers, int line_num, int &n,int &m,int &s,int &t, int * pointer);
bool spfa(vector<int> & path, int &ans, int * pointer, int n, int s,int t);
int findCost(vector<vector<int>> &paths, map<int,int> &valid_server, int * pointer, int n, int s,int t);



#endif /* MCMF1_H_ */
