#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <stdio.h>

struct Edge {
        int  index;
        Edge *next_edge;
        int  band_width;
        int  unit_cost;
};

class Graph {
public:
	int vertex_num;
	int edge_num;
	std::vector<Edge*> vertices;

	Graph(int node_num) : vertex_num(node_num), edge_num(0)
	{
		for (int i = 0; i != node_num; ++i) {
			vertices.push_back(nullptr);
		}
	}
	inline int get_vertex_num(void) { return this->vertex_num; }
	inline int get_edge_num(void)   { return this->edge_num; }
	/*由边的起始和终止位置，在邻接表中查找边信息*/
	inline Edge* get_edge(int start_point,int end_point){
		Edge* next = vertices[start_point];
		while(next!=nullptr){
			if(next->index==end_point){
				break;
			}
			next = next->next_edge;
		}
		if (next==nullptr){
			printf("targed edge not found!!");
			return nullptr;
		}
		return next;
	}
	void CreateFromBuf(char ** buf, int line_num);
	void Print(void);
	Edge * GetEdgeWithIndex(int src, int dst);
	std::vector<int> DijkstraShortestPath(int src, int dst);
	int DijkstraLeastDistance(int src, int dst);
	~Graph();
};

struct Customer{
	int start;//消费者
	int agency;//消费者相连的代理
	int demand;//消费者需求
};

#endif
