#include "util.h"
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
	fputs("\n",pf);//将 一行  字符串写入pf
	fputs(content,pf);//将 一行  字符串写入pf


	fclose(pf);
}
