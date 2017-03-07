#ifndef GRAPH_H
#define GRAPH_H

struct Edge {
        int  index;
        Edge *next_edge;
        int  band_width;
        int  unit_cost;
};

class Graph {
private:
        int vertex_num;
        int edge_num;
        vector<Edge*> vertices;
public:
	Graph(int node_num) : vertex_num(node_num), edge_num(0)
	{
		for (unsigned i = 0; i != node_num; ++i) {
			vertices.push_back(nullptr);
		}
	}
	inline int get_vertex_num(void) { return this->vertex_num; }
	inline int get_edge_num(void)   { return this->edge_num; }
	void CreateFromBuf(char ** buf, int line_num);
	void Print(void);
	~Graph();
};

#endif
