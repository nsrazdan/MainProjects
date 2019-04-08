// written by nsrazdan

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include "index_min_pq.h"

class Vertex;
class Edge;
class Graph;
class ShortestPath;

// Public class to represent an edge from src vertex to dest. Directed, weighted
// graph, so used as unique member of src with a weight and a pointer to dest
// vertex
class Edge {
 public:
  Edge(std::shared_ptr<Vertex> dest, double weight);
  std::shared_ptr<Vertex> dest;
  double weight;
};

Edge::Edge(std::shared_ptr<Vertex> dest, double weight)
    : dest(std::move(dest)),
      weight(weight)
{}

// Public class to represent vertex in graph. Has a vector of edges for vertices
// it points to. Also contains pointer to previous vertex taken in path, and a
// distance from source vertex used for implementing Dijkstra's method
class Vertex {
 public:
  explicit Vertex(unsigned int id);
  // Add edge to vertex. This is a directed edge to another vertex in graph
  void AddEdge(std::unique_ptr<Edge> edge);
  std::vector<std::unique_ptr<Edge>> edges;
  std::shared_ptr<Vertex> previous_in_path;
  double dist;
  unsigned int id;
};

Vertex::Vertex(unsigned int id) : dist(-1), id(id) {}

void Vertex::AddEdge(std::unique_ptr<Edge> edge) {
  edges.emplace_back(std::move(edge));
}

// Class to represent the shortest path between two vertices in graph. Contains
// vector of vertices to represent path taken and a weight for that path
class ShortestPath {
 public:
  ShortestPath();
  // Print the shortest path
  void PrintShortestPath();
  std::shared_ptr<Vertex> src;
  std::shared_ptr<Vertex> dest;
  std::vector<std::shared_ptr<Vertex>> path;
  double path_weight;
};

ShortestPath::ShortestPath() :
  path_weight(0.00)
  {}

void ShortestPath::PrintShortestPath() {
  // If path is empty, print to user that no path was found
  if (path.empty()) {
    std::stringstream ss;
    ss << src->id << " to " << dest->id << ": no path";
    std::cout << ss.str() << std::endl;
    return;
  }
  // Print path, including every vertex taken
  std::cout << path.at(0)->id << " to " << path.at(path.size() - 1)->id << ": ";
  for (auto const& n : path) {
    std::cout << n->id;
    if (n != path.at(path.size() -1)) { std::cout << " => "; }
  }
  std::cout << " (" << path_weight << ")" << std::endl;
}

// Class to represent graph of vertices and edges
class Graph {
 public:
  explicit Graph(unsigned int cur_size);
  unsigned int Size();
  void AddEdge(const unsigned int& src, const unsigned int& dest,
    const double& weight);
  bool IsNodeIndexValid(int index);
  void Dijkstra(const std::shared_ptr<ShortestPath>& shortest_path,
    unsigned int src, unsigned int dest);
 private:
  std::vector<std::shared_ptr<Vertex>> vertices;
  unsigned int cur_size;
};

Graph::Graph(unsigned int cur_size) :
  cur_size(cur_size) {
  for (unsigned int i = 0; i < cur_size; i++) {
    vertices.emplace_back(std::make_shared<Vertex>(i));
  }
}

unsigned int Graph::Size() {
  return cur_size;
}
// Dijkstras algorithm function
void Graph::Dijkstra(const std::shared_ptr<ShortestPath>& shortest_path,
  unsigned int src, unsigned int dest) {
  // Initialize min priority queue
  IndexMinPQ<double> priority_vertices(cur_size);

  // Initialize shortest_path
  shortest_path->src = vertices[src];
  shortest_path->dest = vertices[dest];

  // Set source vertex distance to zero and push to queue
  vertices[src]->dist = 0;
  priority_vertices.Push(vertices[src]->dist, src);

  // While the queue is not empty
  while (priority_vertices.Size() != 0) {
    // Save and remove vertex
    unsigned int cur_vertex_index = priority_vertices.Top();
    priority_vertices.Pop();

    // If destination is reached, break
    if (vertices[cur_vertex_index]->id == dest) {
      break;
    }

    // For each adjacent vertex
    for (auto const &n : vertices[cur_vertex_index]->edges) {
      // Alt path weight = source->current node distance + possible path weight
      double alt_path_weight = vertices[cur_vertex_index]->dist + n->weight;

      // If alt path is better than current one
      if (alt_path_weight < n->dest->dist || n->dest->dist < 0) {
        // Change distance from source
        n->dest->dist = alt_path_weight;
        // Update previous node in path
        n->dest->previous_in_path = vertices[cur_vertex_index];

        // Update priority Queue
        if (priority_vertices.Contains(n->dest->id)) {
          priority_vertices.ChangeKey(alt_path_weight, n->dest->id);
        } else {
          priority_vertices.Push(alt_path_weight, n->dest->id);
        }
      }
    }
  }

  // Set current vertex to destination. Used to check if path is found.
  // If it is, to recreate it
  std::shared_ptr<Vertex> current_vertex(vertices[dest]);

  // If no path was found, return and do not create path
  if (current_vertex->dist < 0) {
    return;
  }

  // Backtracking to set shortest path
  if (current_vertex->dist > 0) {
    shortest_path->path_weight = current_vertex->dist;
    do {
      shortest_path->path.insert(shortest_path->path.begin(), current_vertex);
      current_vertex = current_vertex->previous_in_path;
    } while (current_vertex);
  }
}

void Graph::AddEdge(const unsigned int& src, const unsigned int& dest,
  const double& weight) {
  std::unique_ptr<Edge> edge(new Edge(vertices[dest], weight));
  vertices[src]->AddEdge(std::move(edge));
}

bool Graph::IsNodeIndexValid(int index) {
  return (index >= 0 && index < static_cast<int>(cur_size));
}

void CheckArgsValid(int argc, char* argv[]) {
  // Init stringstream to print errors
  std::stringstream ss;
  // If an invalid amount of args are entered, print usage to user
  if (argc != 4) {
    ss << "Usage: " << argv[0] << " <graph.dat> src dst";
    throw std::runtime_error(ss.str());
  }
}

void CheckFileValid(char* argv[]) {
  // Init stringstream and input file
  std::ifstream task_file(argv[1]);
  std::stringstream ss;
  // If input file cannot be read, print out error to user
  if (!task_file.good()) {
    ss << "Error: cannot open file " << argv[1];
    throw std::runtime_error(ss.str());
  }
}

void ReadInputFile(char* argv[], std::shared_ptr<Graph>& graph) {
  // Init input file and string stream to print error
  std::ifstream task_file(argv[1]);
  std::stringstream ss;

  int num_vertices;
  task_file >> num_vertices;

  // Check if valid number of vertices
  if (num_vertices <= 0) {
    ss << "Error: invalid graph size";
    throw std::runtime_error(ss.str());
  }

  graph.reset(new Graph(static_cast<unsigned int>(num_vertices)));

  // Check if valid source and destination values were entered earlier
  if (!graph->IsNodeIndexValid(std::stoi(argv[2]))) {
    ss << "Error: invalid source vertex number " << std::stoi(argv[2]);
    throw std::runtime_error(ss.str());
  } else if (!graph->IsNodeIndexValid(std::stoi(argv[3]))) {
    ss << "Error: invalid dest vertex number " << std::stoi(argv[3]);
    throw std::runtime_error(ss.str());
  }

  int src, dest;
  double weight;

  // Read every line in file. Use that data to create and push edges into graph.
  while (task_file >> src >> dest >> weight) {
    if (!graph->IsNodeIndexValid(src)) {
      ss << "Invalid source vertex number " << src;
      throw std::runtime_error(ss.str());
    } else if (!graph->IsNodeIndexValid(dest)) {
      ss << "Invalid dest vertex number " << dest;
      throw std::runtime_error(ss.str());
    } else if (weight < 0) {
      ss << "Invalid weight " << weight;
      throw std::runtime_error(ss.str());
    }
    graph->AddEdge(static_cast<unsigned int>(src),
      static_cast<unsigned int>(dest), weight);
  }
}

int main(int argc, char* argv[]) {
  std::shared_ptr<Graph> graph;
  std::shared_ptr<ShortestPath> shortest_path(new ShortestPath());
  try {
    CheckArgsValid(argc, argv);
    CheckFileValid(argv);
    ReadInputFile(argv, graph);
  } catch(std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
  graph->Dijkstra(shortest_path,
      static_cast<unsigned int>(std::stoul(argv[2])),
      static_cast<unsigned int>(std::stoul(argv[3])));
  shortest_path->PrintShortestPath();
  return 0;
}
