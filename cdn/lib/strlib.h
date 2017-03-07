#pragma once
#ifndef STRING_SPLIT_H
#define STRING_SPLIT_H

#include <string>
#include <vector>

/* function: to split a string by separator
 * input:  @str: target string to split
 *         @sep: separator string
 * output: vector of string
 */
std::vector<std::string> str_split(const std::string &str, const std::string &sep);
#endif