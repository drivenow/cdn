#include <vector>
using std::vector;

#include <string>
using std::string;

#include <iostream>
using std::cout; using std::endl;

#include "strlib.h"

#include "graph.h"
using namespace std;

void Graph::CreateFromBuf(char ** buf, int line_num)
{
        //vector<string> lines;
        //int comp = 0;
        for (int i = 0; i != line_num; ++i) {
                string line(buf[i]);
                //lines.push_back(line);
                vector<string> numbers = str_split(line, " ");
                int src = std::stoi(numbers[0]);
                int dst = std::stoi(numbers[1]);
                int band_width = std::stoi(numbers[2]);
                int unit_cost  = std::stoi(numbers[3]);

                Edge *edge_src = new Edge, *edge_dst = new Edge;
                edge_src->index = dst;
                edge_dst->index = src;
                edge_src->band_width = edge_dst->band_width = band_width;
                edge_src->unit_cost  = edge_dst->unit_cost  = unit_cost;
                edge_src->next_edge = edge_dst->next_edge = nullptr;
                if (vertices[src] == nullptr) {
                        vertices[src] = edge_src;
                } else {
                        edge_src->next_edge = vertices[src];
                        vertices[src] = edge_src;
                }
                if (vertices[dst] == nullptr) {
                        vertices[dst] = edge_dst;
                } else {
                        edge_dst->next_edge = vertices[dst];
                        vertices[dst] = edge_dst;
                }
                this->edge_num++;
        }
}

void Graph::Print(void)
{
        int node_num = this->vertex_num;
        for (unsigned i = 0; i != node_num; ++i) {
                Edge *edge = vertices[i];
                int index, band_width, unit_cost;
                while (edge != nullptr) {
                        index = edge->index;
                        band_width = edge->band_width;
                        unit_cost  = edge->unit_cost;
                        edge = edge->next_edge;
                        cout << i << " to " << index << " " << band_width << " " << unit_cost << endl;
                }
        }
}

static Edge* GetLastEdge(Edge * edge)
{
        if (edge->next_edge == nullptr) {
                return edge;
        }
        Edge *last = edge, *prior = edge->next_edge;
        while (prior != nullptr) {
                last  = last->next_edge;
                prior = prior->next_edge;
        }
        return last;
}

static Edge* GetPriorEdge(Edge * edge_current, Edge * head)
{
        if (head == edge_current) {
                return nullptr;
        }
        Edge *next = head;
        while (next->next_edge != edge_current) {
                next = next->next_edge;
        }
        return next;
}

Graph::~Graph()
{
        int node_num = this->vertex_num;
        for (unsigned i = 0; i != node_num; ++i) {
                Edge *edge  = GetLastEdge(vertices[i]);
                while (edge != vertices[i]) {
                        Edge *prior = GetPriorEdge(edge, vertices[i]);
                        delete edge;
                        edge = prior;
                }
        }
}
