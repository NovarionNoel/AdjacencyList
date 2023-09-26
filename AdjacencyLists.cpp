#include <iostream>
#include <fstream>
#include <cstring>
#include <chrono>
#include <thread>
#include <sstream>
#include <ctime>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

#define TXT 11
#define MTX 12
#define EDGES 13
#define NODES 14

using namespace std;

unordered_map<int, unordered_set<int>> createAdjacencyList(fstream *file, int type)
{
   unordered_map<int, unordered_set<int>> AdjacencyList;

   switch (type)
   {
   case TXT:

      // read edge pairs
      // use first coord as first vertex
      // use second coord as second vertex
      // third coord is weight (this graph is unweighted)
      int x, y, w;
      while (*file >> x >> y >> w)
      {

         AdjacencyList[x].insert(y);
      }
      break;
   case MTX:
      // do stuff
      break;
   case EDGES:
      // do stuff
      break;
   default:
      // give me a supported file type.
      cout << "File type not supported. Try again." << endl;
      exit(2);
      break;
   }
   return AdjacencyList;
}

// unordered_map<int, unordered_set<int>> createAdjacencyList(fstream *nodes, fstream *edges)
// {
// }

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
   }

   if (!file.is_open())
   {
      string errorMessage = "Error: Failed to open file after " + to_string(maxAttempts) + " attempts.";
      logError(errorMessage);
      exit(1); // or throw an exception
   }

   return file;
}

int main(int argc, char *argv[])
{
   if (argc >= 3)
   {
      if (strcmp(argv[1], "-f") == 0)
      {
         fstream combinedGraph = singleOpen(argv[2]);
         int type = 0;
         string extension(argv[2]);

         extension = (extension.substr(extension.find(".") + 1));
         if (extension.compare("txt") == 0)
         {
            type = TXT;
         }
         else if (extension.compare("edges"))
         {
            type = EDGES;
         }
         else if (extension.compare("mtx"))
         {
            type = MTX;
         }
         unordered_map<int, unordered_set<int>> AdjacencyList;
         AdjacencyList = createAdjacencyList(&combinedGraph, type);
         for (auto it = AdjacencyList.cbegin(); it != AdjacencyList.cend(); it++)
         {
            cout << "NODE: " << it->first << "\n";
            cout << "EDGES: ";
            for (auto setIT = it->second.cbegin(); setIT != it->second.cend(); setIT++)
            {
               cout << *setIT << ',';
            }
            cout << "\n";
         }
      }
      else if (strcmp(argv[1], "-n") == 0 && argc > 4 && strcmp(argv[3], "-e") == 0)
      {
         fstream nodeFile = singleOpen(argv[2]);
         fstream edgeFile = singleOpen(argv[4]);
      }
      else
      {
         cout << "Invalid arguments. Please supply either a file with both nodes and edges, or two separate files." << endl;
         exit(1);
      }
   }
   return 0;
}