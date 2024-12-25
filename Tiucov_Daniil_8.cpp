#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <limits>
#include <queue>
#include <iomanip>   
#include <algorithm>
#include <chrono>

struct Node {
    double latitude, longitude;
    std::vector<std::pair<Node*, double>> neighbors;
};

class Graph {
public:
    std::vector<Node*> nodes;
    std::unordered_map<std::string, Node*> node_lookup;

    std::string create_node_key(double latitude, double longitude) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(7) << longitude << "," << latitude;
        return oss.str();
    }

    void load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto delimiter_pos = line.find(":");
            if (delimiter_pos == std::string::npos) continue;

            std::string parent_coords = line.substr(0, delimiter_pos);
            std::string neighbors_data = line.substr(delimiter_pos + 1);

            double parent_lat, parent_lon;
            sscanf(parent_coords.c_str(), "%lf,%lf", &parent_lon, &parent_lat);

            Node* parent_node;
            std::string parent_key = create_node_key(parent_lat, parent_lon);

            if (node_lookup.find(parent_key) == node_lookup.end()) {
                parent_node = new Node{parent_lat, parent_lon};
                nodes.push_back(parent_node);
                node_lookup[parent_key] = parent_node;
            } else {
                parent_node = node_lookup[parent_key];
            }

            std::stringstream ss(neighbors_data);
            std::string neighbor_info;
            while (std::getline(ss, neighbor_info, ';')) {
                double neighbor_lat, neighbor_lon, edge_weight;
                sscanf(neighbor_info.c_str(), "%lf,%lf,%lf", &neighbor_lon, &neighbor_lat, &edge_weight);

                Node* neighbor_node;
                std::string neighbor_key = create_node_key(neighbor_lat, neighbor_lon);

                if (node_lookup.find(neighbor_key) == node_lookup.end()) {
                    neighbor_node = new Node{neighbor_lat, neighbor_lon};
                    nodes.push_back(neighbor_node);
                    node_lookup[neighbor_key] = neighbor_node;
                } else {
                    neighbor_node = node_lookup[neighbor_key];
                }

                parent_node->neighbors.push_back({neighbor_node, edge_weight});
            }
        }

        file.close();
    }

    double calculate_euclid(Node* current, Node* target) {
        return std::sqrt(std::pow(current->latitude - target->latitude, 2) + std::pow(current->longitude - target->longitude, 2));
    }

    std::vector<Node*> find_shortest_path(Node* start, Node* goal, double& total_weight) {
        std::unordered_map<Node*, double> path_cost;
        std::unordered_map<Node*, Node*> previous_node;

        for (auto node : nodes) {
            path_cost[node] = std::numeric_limits<double>::max();
        }
        path_cost[start] = 0;

        auto compare = [](const std::pair<Node*, double>& a, const std::pair<Node*, double>& b) {
            return a.second > b.second;
        };
        std::priority_queue<std::pair<Node*, double>, std::vector<std::pair<Node*, double>>, decltype(compare)> open_set(compare);
        open_set.push({start, calculate_euclid(start, goal)});

        while (!open_set.empty()) {
            Node* current_node = open_set.top().first;
            open_set.pop();

            if (current_node == goal) break;

            for (auto neighbor : current_node->neighbors) {
                Node* neighbor_node = neighbor.first;
                double edge_weight = neighbor.second;
                double tentative_path_cost = path_cost[current_node] + edge_weight;

                if (tentative_path_cost < path_cost[neighbor_node]) {
                    previous_node[neighbor_node] = current_node;
                    path_cost[neighbor_node] = tentative_path_cost;
                    double estimated_total_cost = tentative_path_cost + calculate_euclid(neighbor_node, goal);
                    open_set.push({neighbor_node, estimated_total_cost});
                }
            }
        }

        total_weight = path_cost[goal];

        std::vector<Node*> path;
        Node* current_node = goal;
        while (current_node != nullptr) {
            path.push_back(current_node);
            if (previous_node.find(current_node) == previous_node.end()) break;
            current_node = previous_node[current_node];
        }

        std::reverse(path.begin(), path.end());
        return path;
    }
};

int main() {
    Graph graph;

    graph.load_from_file("C:/Users/Danielito/CLionProjects/untitled5/expanded_connected_graph.txt");

    double start_latitude = 59.0005412;
    double start_longitude = 30.000153;
    double goal_latitude = 259.3551939;
    double goal_longitude = 230.445527;

    std::string start_key = graph.create_node_key(start_latitude, start_longitude);
    std::string goal_key = graph.create_node_key(goal_latitude, goal_longitude);

    Node* start_node = graph.node_lookup[start_key];
    Node* goal_node = graph.node_lookup[goal_key];

    if (!start_node || !goal_node) {
        std::cerr << "Start or goal node not found!" << std::endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    double total_weight = 0.0;
    std::vector<Node*> path = graph.find_shortest_path(start_node, goal_node, total_weight);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;

    std::cout << "Path from start to goal:" << std::endl;
    for (auto node : path) {
        std::cout << "(" << node->latitude << ", " << node->longitude << ")" << std::endl;
    }

    std::cout << "Total weight of the path: " << total_weight << std::endl;
    std::cout << "Algorithm execution time: " << static_cast<int>(elapsed_time.count()) << " ms" << std::endl;

    return 0;
}
