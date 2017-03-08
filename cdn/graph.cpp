#include <vector>
using std::vector;

#include <string>
using std::string;

#include <iostream>
using std::cout; using std::endl;

#include "strlib.h"

#include "graph.h"

#include <utility>

#include <algorithm>

void SortCustomers(vector<Customer> & customers)
{
	std::sort(customers.begin(), customers.end(), [](const Customer & a, const Customer & b){
		return a.demand > b.demand;
	});
}

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

static int FindNodeWithLeastDistance(const vector<int> & distances,
                                     const vector<bool> & is_visited,
                                     int node_num)
{
        int dist_min = INF, index = 0;
        for (int i = 0; i != node_num; ++i) {
                if (is_visited[i] == true) {
                        continue;
                }
                if (distances[i] < dist_min) {
                        index = i;
                        dist_min = distances[i];
                }
        }
        return index;
}

vector<int> Graph::DijkstraShortestPath(int src, int dst)
{
        int node_num = this->vertex_num;
        // unvisited vertices
        vector<bool> is_visited(node_num, false);
        vector<int> prior_nodes(node_num);
        // shortest distance from src to vertex
        vector<int> distances(node_num, INF);
        distances[src] = 0;
        int index;
        for (int i = 0; i != node_num; ++i) {
                index = FindNodeWithLeastDistance(distances, is_visited, node_num);
                if (index == dst) {
                        vector<int> nodes_on_path;
                        while (index != src) {
                                nodes_on_path.push_back(index);
                                index = prior_nodes[index];
                        }
                        nodes_on_path.push_back(index);
                        int num = nodes_on_path.size();
                        for (int j = 0; j != num / 2; ++j) {
                                std::swap(nodes_on_path[j], nodes_on_path[num - 1 - j]);
                        }
                        return nodes_on_path;
                }
                is_visited[index] = true;
                // update distances
                Edge * edge = this->vertices[index];
                int dist;
                while (edge != nullptr) {
                        dist = distances[index] + edge->unit_cost;
                        if (dist < distances[edge->index]) {
                                distances[edge->index] = dist;
                                prior_nodes[edge->index] = index;
                        }
                        edge = edge->next_edge;
                }
        }
}

Edge * Graph::GetEdgeWithIndex(int src, int dst)
{
        Edge *edge = this->vertices[src];
        while (edge != nullptr) {
                if (edge->index == dst) {
                        return edge;
                }
                edge = edge->next_edge;
        }
}


vector<int> Graph::RetrieveDistanceBound(const vector<int> & nodes_on_path)
{
        edge_num = nodes_on_path.size() - 1;
        Edge * edge;
        int begin, end;
        int dist_sum = 0, bound = INF;
        for (int i = 0; i != edge_num; ++i) {
                begin = nodes_on_path[i];
                end = nodes_on_path[i + 1];
                edge = GetEdgeWithIndex(begin, end);
                cout << edge->unit_cost << " + ";
                dist_sum += edge->unit_cost;
                if (edge->band_width < bound) {
                        bound = edge->band_width;
                }
        }
        cout << endl;
        return { dist_sum, bound };
}
