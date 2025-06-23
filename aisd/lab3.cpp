//Пусть дан некоторый связный граф, в котором узлы – это травмпункты.
//Предположим, в преддверии зимы вы хотите улучшить покрытие города.
//Для этого вам необходимо понять, какой травмпункт находится дальше 
//всего от своих прямых соседей(т.е.средняя длина связанных с ним ребер 
//максимальна).Напишите функцию, которая находит такой травмпункт.

#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <numeric>
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <queue>
#include <limits>
#include <stdexcept>

template<typename Vertex, typename Distance = double>
class Graph {
public:
    struct Edge {
        Vertex from;
        Vertex to;
        Distance distance;

        Edge(const Vertex& f, const Vertex& t, const Distance& d) : from(f), to(t), distance(d) {}

        bool operator==(const Edge& other) const {
            return from == other.from && to == other.to && distance == other.distance;
        }
    };

private:
    std::vector<Vertex> vertices;
    std::multimap<Vertex, Edge> edges;

public:
    // Проверка-добавление-удаление вершин
    bool has_vertex(const Vertex& v) const {
        return std::find(vertices.begin(), vertices.end(), v) != vertices.end();
    }

    bool add_vertex(const Vertex& v) {
        if (has_vertex(v)) {
            return false;
        }
        vertices.push_back(v);
        return true;
    }

    bool remove_vertex(const Vertex& v) {
        auto it = std::find(vertices.begin(), vertices.end(), v);
        if (it == vertices.end()) {
            return false;
        }
        vertices.erase(it);

        auto range = edges.equal_range(v);
        edges.erase(range.first, range.second);

        for (auto it = edges.begin(); it != edges.end(); ) {
            if (it->second.to == v) {
                it = edges.erase(it);
            }
            else {
                ++it;
            }
        }
        return true;
    }

    std::vector<Vertex> get_vertices() const {
        return vertices;
    }

    // Проверка-добавление-удаление ребер
    void add_edge(const Vertex& from, const Vertex& to, const Distance& d) {
        if (!has_vertex(from) || !has_vertex(to)) {
            throw std::invalid_argument("One or both vertices don't exist");
        }
        edges.insert({ from, Edge(from, to, d) });
    }

    bool remove_edge(const Vertex& from, const Vertex& to) {
        bool removed = false;

        for (auto it = edges.begin(); it != edges.end(); ) {
            if (it->first == from && it->second.to == to) {
                it = edges.erase(it);
                removed = true;
            }
            else {
                ++it;
            }
        }
        return removed;
    }

    bool remove_edge(const Edge& e) {
        auto range = edges.equal_range(e.from);

        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == e) {
                edges.erase(it);
                return true;
            }
        }
        return false;
    }

    bool has_edge(const Vertex& from, const Vertex& to) const {
        auto range = edges.equal_range(from);

        for (auto it = range.first; it != range.second; ++it) {
            if (it->second.to == to) {
                return true;
            }
        }
        return false;
    }

    bool has_edge(const Edge& e) const {
        auto range = edges.equal_range(e.from);

        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == e) {
                return true;
            }
        }
        return false;
    }

    // Получение всех ребер, выходящих из вершины
    std::vector<Edge> get_edges(const Vertex& vertex) {
        std::vector<Edge> result;
        auto range = edges.equal_range(vertex);

        for (auto i = range.first; i != range.second; ++i) {
            result.push_back(i->second);
        }
        return result;
    }

    size_t order() const {
        return vertices.size();
    }

    size_t degree(const Vertex& v) const {
        return edges.count(v);
    }

    // Проверка сильной связности графа (в глубину)
    bool is_connected() const {
        if (vertices.empty()) {
            return true;
        }

        for (const auto& start : vertices) {
            std::set<Vertex> visited;
            std::stack<Vertex> stack;
            stack.push(start);
            visited.insert(start);

            while (!stack.empty()) {
                Vertex cur = stack.top();
                stack.pop();

                auto range = edges.equal_range(cur);

                for (auto i = range.first; i != range.second; ++i) {
                    Vertex neighbor = i->second.to;
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        stack.push(neighbor);
                    }
                }
            }
            if (visited.size() != vertices.size()) {
                return false;
            }
        }
        return true;
    }

    // Поиск кратчайшего пути (Беллмана-Форда)
    std::vector<Edge> shortest_path(const Vertex& from, const Vertex& to) const {
        std::map<Vertex, Distance> distances;
        std::map<Vertex, Edge> predecessors;
        const Distance infinity = std::numeric_limits<Distance>::max();

        for (const auto& v : vertices) {
            distances[v] = infinity;
        }
        distances[from] = Distance{};

        // Релаксация ребер
        for (size_t i = 1; i < vertices.size(); ++i) {
            for (const auto& [_, edge] : edges) {
                if (distances[edge.from] != infinity &&
                    distances[edge.to] > distances[edge.from] + edge.distance) {
                    distances[edge.to] = distances[edge.from] + edge.distance;
                    predecessors[edge.to] = edge;
                }
            }
        }

        // Проверка на отрицательные циклы
        for (const auto& [_, edge] : edges) {
            if (distances[edge.from] != infinity &&
                distances[edge.to] > distances[edge.from] + edge.distance) {
                throw std::runtime_error("Graph contains a negative weight cycle");
            }
        }

        // Восстановление пути
        if (distances[to] == infinity) {
            return {};
        }

        std::vector<Edge> path;
        Vertex cur = to;

        while (cur != from) {
            auto i = predecessors.find(cur);

            if (i == predecessors.end()) {
                return {};
            }
            path.push_back(i->second);
            cur = i->second.from;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    // Обход в ширину
    std::vector<Vertex> walk(const Vertex& start_vertex) const {
        std::vector<Vertex> visited;

        if (!has_vertex(start_vertex)) {
            return visited;
        }

        std::queue<Vertex> queue;
        std::set<Vertex> marked;

        queue.push(start_vertex);
        marked.insert(start_vertex);

        while (!queue.empty()) {
            Vertex cur = queue.front();
            queue.pop();
            visited.push_back(cur);

            auto range = edges.equal_range(cur);

            for (auto i = range.first; i != range.second; ++i) {
                Vertex neighbor = i->second.to;
                if (marked.find(neighbor) == marked.end()) {
                    marked.insert(neighbor);
                    queue.push(neighbor);
                }
            }
        }
        return visited;
    }

    // Вычисление средней длины ребер вершины
    Distance average_edge_length(const Vertex& v) const {
        auto edges = const_cast<Graph*>(this)->get_edges(v);

        if (edges.empty()) {
            return Distance{};
        }

        Distance sum = std::accumulate(edges.begin(), edges.end(), Distance{},
            [](Distance acc, const Edge& e) { return acc + e.distance; });

        return sum / static_cast<Distance>(edges.size());
    }
};

template<typename Vertex, typename Distance = double>
Vertex find_vertex_with_max_avg_edge_length(const Graph<Vertex, Distance>& graph) {
    if (graph.order() == 0) {
        throw std::runtime_error("Graph is empty");
    }

    Vertex result;
    Distance max_avg = std::numeric_limits<Distance>::lowest();

    for (const auto& vertex : graph.get_vertices()) {
        Distance avg = graph.average_edge_length(vertex);
        if (avg > max_avg) {
            max_avg = avg;
            result = vertex;
        }
    }
    return result;
}

int main() {
    Graph<std::string, double> city_graph;

    city_graph.add_vertex("Hospital A");
    city_graph.add_vertex("Hospital B");
    city_graph.add_vertex("Hospital C");
    city_graph.add_vertex("Hospital D");

    city_graph.add_edge("Hospital A", "Hospital B", 5.0);
    city_graph.add_edge("Hospital A", "Hospital C", 3.0);
    city_graph.add_edge("Hospital B", "Hospital C", 2.0);
    city_graph.add_edge("Hospital B", "Hospital D", 7.0);
    city_graph.add_edge("Hospital C", "Hospital D", 1.0);
    city_graph.add_edge("Hospital D", "Hospital A", 4.0);

    std::string furthest = find_vertex_with_max_avg_edge_length(city_graph);
    std::cout << "The trauma center furthest from its neighbors is " << furthest << std::endl;

    double avg = city_graph.average_edge_length(furthest);
    std::cout << "Average distance to neighbors: " << avg << std::endl;

    return 0;
}
