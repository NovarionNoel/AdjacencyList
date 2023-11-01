#include <iostream>
#include <fstream>
#include <cstring>
#include <chrono>
#include <thread>
#include <sstream>
#include <ctime>
#include <unordered_map>
#include <iomanip>
#include <unordered_set>
#include <set>
#include <queue>

#define TXT 11
#define MTX 12
#define EDGES 13
#define NODES 14

/*
Adding edges
attempt elimination order
any nodes in elimination order can be skipped
calculate clustering coefficient
(n*(n-1))/2 - e
fill-in
attempt elimination order
finding minimum fill in is NP complete (go cry about it)
*/

using namespace std;

unordered_map<int, unordered_set<int>> createChordalSets(unordered_map<int, unordered_map<int, int>> &adjacencyList)
{
   unordered_map<int, unordered_set<int>> chordalSets;
   for (const auto &pair : adjacencyList)
   {
      chordalSets.emplace(pair.first, std::unordered_set<int>());
   }
   return chordalSets;
}

priority_queue<pair<int, int>> createMaxHeap(unordered_map<int, unordered_set<int>> &chordalSets, int nodes[])
{
   priority_queue<pair<int, int>> maxChordalSet;
   for (const auto &pair : chordalSets)
   {
      if (nodes[pair.first] == 0)
      {
         maxChordalSet.emplace(pair.first, pair.second.size());
      }
   }
   return maxChordalSet;
}

int selectStart(unordered_map<int, unordered_map<int, int>> &adjacencyList)
{
   int k = adjacencyList.size();
   int start = (rand() % (adjacencyList.size() + 1)) + 1;
   return start;
}

void fillWithZeroes(int nodes[])
{
   for (int i = 0; i < (sizeof(nodes) / sizeof(nodes[0])); i++)
   {
      nodes[i] = 0;
   }
}

void markVisited(int nodes[], int vertex, int step)
{
   nodes[vertex] = step;
}

int selectLargest(unordered_map<int, unordered_set<int>> &chordalSets, int nodes[])
{
   priority_queue<pair<int, int>> heap;
   heap = createMaxHeap(chordalSets, nodes);
   int newVertex = -1;
   if (!heap.empty())
   {
      newVertex = heap.top().first;
   }
   return newVertex;
}

void checkNeighbors(unordered_map<int, unordered_map<int, int>> &adjacencyList, unordered_map<int, unordered_set<int>> &chordalSets, int nodes[], int vertex, int step)
{
   // check each neighbor of the vertex in adjacency list
   for (auto const &[vertex, edgeSet] : adjacencyList)
   {
      for (auto const &[edge, weight] : edgeSet)
      {
         if (nodes[edge] == 0)
         {
            if (chordalSets[edge].size() == 0)
            {
               chordalSets[edge].insert(vertex);
               // adding to the chordal set final in adjacencyList
            }
            else
            {
               set<int> cSetCopyV;
               set<int> cSetCopyE;
               bool subset = true;
               for (auto it = chordalSets[vertex].begin(); it != chordalSets[vertex].end(); it++)
               {
                  cSetCopyV.emplace(*it);
               }
               for (auto it2 = chordalSets[edge].begin(); it2 != chordalSets[edge].end(); it2++)
               {
                  cSetCopyE.emplace(*it2);
               }
               // check to see if subset of V
               for (auto it = cSetCopyE.begin(); it != cSetCopyE.end(); it++)
               {
                  if (cSetCopyV.count(*it) == 0)
                  {
                     subset = false;
                     break;
                  }
               }
               if (subset)
               {
                  chordalSets[edge].emplace(vertex);
               }
            }
         }
      }
   }
   markVisited(nodes, vertex, step);
}

unordered_map<int, unordered_map<int, int>> createAdjacencyList(fstream *file, int type, int skip)
{
   unordered_map<int, unordered_map<int, int>> AdjacencyList;
   string eatLines;
   int x = 0, y = 0, w = 0;

   switch (type)
   {
   case TXT:
      for (int i = 0; i < skip; i++)
      {
         getline(*file, eatLines);
      }
      // read edge pairs
      // use first coord as first vertex
      // use second coord as second vertex
      // third coord is weight

      while (*file >> x >> y)
      {

         AdjacencyList[x].insert(make_pair(y, w));
      }
      break;
   case MTX:
      // do stuff
      for (int i = 0; i < skip; i++)
      {
         getline(*file, eatLines);
      }
      // unweighted mtx
      while (*file >> x >> y)
      {
         AdjacencyList[x].insert(make_pair(y, w));
      }
      break;
   case EDGES:
      for (int i = 0; i < skip; i++)
      {
         getline(*file, eatLines);
      }

      while (*file >> x >> y)
      {
         AdjacencyList[x].insert(make_pair(y, w));
      }
      break;
   default:
      // give me a supported file type.
      cout << "File type not supported. Try again." << endl;
      exit(2);
      break;
   }
   return AdjacencyList;
}

string getCurrentDateTime(const string &format)
{
   auto now = chrono::system_clock::now();
   auto in_time_t = chrono::system_clock::to_time_t(now);

   stringstream ss;
   ss << put_time(localtime(&in_time_t), format.c_str());
   return ss.str();
}

void logError(const string &message)
{
   cerr << message << endl;
   fstream logFile("error_log_" + getCurrentDateTime("%Y%m%d_%H%M%S") + ".log", ios::out);
   if (logFile.is_open())
   {
      logFile << getCurrentDateTime("%Y-%m-%d %H:%M:%S") << ": " << message << endl;
   }
}

fstream singleOpen(const string &filename)
{
   const int maxAttempts = 3;
   int attempts = 0;
   fstream file;

   while (attempts < maxAttempts && !file.is_open())
   {
      file.open(filename, ios::in);
      if (!file.is_open())
      {
         cout << "Failed to open file (attempt " << (attempts + 1) << "). Retrying..." << endl;
         this_thread::sleep_for(chrono::seconds(1));
         attempts++;
      }
      else
      {
         cout << filename << " opened successfully \n";
      }
   }

   if (!file.is_open())
   {
      string errorMessage = "Error: Failed to open file after " + to_string(maxAttempts) + " attempts.";
      logError(errorMessage);
      exit(1); // or throw an exception
   }

   return file;
}

void printAdjList(unordered_map<int, unordered_map<int, int>> adjList)
{
   for (auto const &[node, edgeSet] : adjList)
   {
      cout << "NODE: " << node << "\n";
      cout << "EDGES: ";
      for (auto const &[edge, weight] : edgeSet)
      {
         cout << edge << " w: " << weight << ", ";
      }
      cout << "\n";
   }
}

void printChordalSet(unordered_map<int, unordered_set<int>> chordalSets)
{
   for (auto const &[node, edgeSet] : chordalSets)
   {
      cout << "NODE: " << node << "\n";
      cout << "EDGES: ";
      for (auto const &edge : edgeSet)
      {
         cout << edge << ", ";
      }
      cout << "\n";
   }
}

int main(int argc, char *argv[])
{
   int skip = 0;
   if (argc >= 3)
   {
      unordered_map<int, unordered_map<int, int>> AdjacencyList;
      if (strcmp(argv[1], "-f") == 0)
      {
         // open file
         fstream combinedGraph = singleOpen(argv[2]);
         int type = 0;

         string extension(argv[2]);

         extension = (extension.substr(extension.find(".") + 1));

         if (extension.compare("txt") == 0)
         {
            type = TXT;
         }

         else if (extension.compare("edges") == 0)
         {
            type = EDGES;
         }

         else if (extension.compare("mtx") == 0)
         {
            type = MTX;
         }

         if (argc == 5 && strcmp(argv[3], "-s") == 0)
         {
            skip = stoi(argv[4]);
         }

         AdjacencyList = createAdjacencyList(&combinedGraph, type, skip);
         int *nodes = new int[AdjacencyList.size()];
         fillWithZeroes(nodes);
         unordered_map<int, unordered_set<int>> chordalSets = createChordalSets(AdjacencyList);
         int vertex = selectStart(AdjacencyList);

         for (int step = AdjacencyList.size() - 1; step >= 0; step--)
         {

            checkNeighbors(AdjacencyList, chordalSets, nodes, vertex, step);
            vertex = selectLargest(chordalSets, nodes);
         }

         cout << "Chordal Sets \n";
         printChordalSet(chordalSets);
      }
   }
   else
   {
      cout << "Invalid arguments. Please supply either a file with both nodes and edges." << endl;
      exit(1);
   }

   return 0;
}