#pragma once
#ifndef STRING_SPLIT_H
#define STRING_SPLIT_H

#include <string>
#include <vector>
#include <map>
using namespace std;

/* function: to split a string by separator
 * input:  @str: target string to split
 *         @sep: separator string
 * output: vector of string
 */
std::vector<std::string> str_split(const std::string &str, const std::string &sep);

string pathString(vector<vector<int>> paths, map<int, int> agency_map, bool find_so);

void write_cost(char * filename, char * content);

void optimizer1(vector<vector<int>> paths,map<int,int> &valid_server, map<int,vector<int>> &valid_node , int &cost, map<int,int> &cust_demand, int &transfer_cost, int customer_demand, vector<int> &servers, int n ,int s, int t,int before_cl);

void print_vector(vector<int> vec);
#endif
