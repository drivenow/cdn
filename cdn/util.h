#pragma once
#ifndef STRING_SPLIT_H
#define STRING_SPLIT_H

#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include <stdio.h>
#include <stdlib.h>
#include "MCMF.h"
using std::map;
using namespace std;



/* function: to split a string by separator
 * input:  @str: target string to split
 *         @sep: separator string
 * output: vector of string
 */

struct Customer {
	int index;   //消费者
	int agency;  //消费者相连的代理
	int demand;  //消费者需求

};

std::vector<std::string> str_split(const std::string &str, const std::string &sep);

string pathString(vector<vector<int>> paths, map<int, int> agency_map,bool find_so);
void customerNodes(record *edge, int *pointer, int cl, int t, int before_top, int top, map<int,int> &nb_nodes, int cnodes_dis[], int pc, int search_layer);

vector<vector<int>> allCustomerNodes(record *edge, int *pointer, int cl, int t, map<int, int> agency_map, int search_layer);

#endif
