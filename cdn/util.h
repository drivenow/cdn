#pragma once
#ifndef STRING_SPLIT_H
#define STRING_SPLIT_H

#include <string>
#include <vector>
#include "MCMF.h"
#include "graph.h"
#include <map>
using namespace std;

/* function: to split a string by separator
 * input:  @str: target string to split
 *         @sep: separator string
 * output: vector of string
 */
extern Graph g;
std::vector<std::string> str_split(const std::string &str, const std::string &sep);

string pathString(vector<vector<int>> paths, map<int, int> agency_map, bool find_so);

void write_cost(char * filename, char * content);

void optimizer1(vector<vector<int>> &paths,map<int,int> &valid_server, map<int,vector<int>> &valid_node , int &cost, map<int,int> &cust_demand, int &transfer_cost, int customer_demand, vector<int> &servers, int n ,int s, int t,int before_cl);

void print_vector(vector<int> vec);

void customerNodes(record *edge, int *pointer, int cl, int t, int before_top, int top, map<int,int> &nb_nodes, int cnodes_dis[], int pc, int search_layer);

map<int,vector<int>> allCustomerNodes(record *edge, int *pointer, int cl, int t, map<int, int> agency_map, int search_layer);

void findFlow(Graph &g, vector<vector<int>> &paths, map<int,int> &inTrans, map<int,int> &outFlow);

void changeServers(vector<int> &servers, vector<int> in, vector<int> out);

void repalce_find_cost(vector<int> &in, vector<int> &out, vector<vector<int>> &paths, map<int,int> &valid_server, vector<int> &servers, int &cost, int &transfer_cost, map<int,vector<int>> &valid_node, int customer_demand, int n, int s, int t, int before_cl);

void deep_search(map<int,int> &delete_map,int &best_in, int search_layer, Graph &g, vector<vector<int>> &paths,  map<int,int> &valid_server, vector<int> &servers, int &cost, int &transfer_cost, map<int,vector<int>> &valid_node, int customer_demand, int n, int s, int t, int before_cl, map<int,int> cust_demand);

void wirte_inTrans(vector<pair<int,int>> inTrans_vec);

#endif
