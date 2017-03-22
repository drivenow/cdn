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
#endif
