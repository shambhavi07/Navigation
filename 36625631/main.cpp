/*main.cpp*/

//
// Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07: open street maps, graphs, and Dijkstra's alg
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:  
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"
using namespace std;
using namespace tinyxml2;


// function to find a node that has not yet been fixed and is minimum among such untravelled nodes
// minimum finding with a flag condition
long long minDistNode(vector<long long> vertices,map<long long,bool> flag,map<long long, double> distance){
  double INF = numeric_limits<double>::max();
  double min = INF;
  long long min_index = -1;

  for(int i=0;i<vertices.size();i++){
    if(flag[vertices[i]]==false && distance[vertices[i]]<=min){
      min = distance[vertices[i]];
      min_index = vertices[i];
    }
  }

  return min_index; 
}

// function to find the dijkstra path from start to destination
// returns the distance between the two nodes and modifies route to contain the route
double dijkstraNavigation(long long start,long long dest,const graph<long long, double> G,vector<long long>& route){
  double INF = numeric_limits<double>::max();

  map<long long, bool> flag;  // flag to store if a node is visited
  map<long long, double> distance; // map to store distance of a node from start
  map<long long,long long> discovery;  // the node from which a node was discovered, for predecessor
  vector<long long> vertices = G.getVertices();  // vector that contains all the vertices

  // initialise all the maps
  for(int i=0;i<vertices.size();i++){
    flag[vertices[i]]=false;
    distance[vertices[i]]=INF;
    discovery[vertices[i]]=vertices[i];
  }

  // the distance of start node from itself is 0 
  distance[start] = 0;

  // discover and fix path to one node in every iteration
  for (int count = 0; count < vertices.size(); count++) {
    //find the node to fix in present iteration
    long long u = minDistNode(vertices,flag,distance);
    // if the distance of this node is inf, break because no further path exist
    if(distance[u]==INF){
      break;
    }
    // set flag to true
    flag[u] = true; 
    //if this is the destination node, break
    if(u==dest){
      break;
    }

    // get all neighbours of this node and update their distance
    set<long long> adj = G.neighbors(u);
    set<long long>::iterator it;
    for(it=adj.begin();it!=adj.end();it++){
      long long node = (*it);
      if(!flag[node]){
        double weight;
        bool edgeExist = G.getWeight(u,node,weight);

        if(edgeExist && distance[u]+weight < distance[node]){
          distance[node] = distance[u]+weight;
          //set discovery for predecessor tracking
          discovery[node] = u;
        }
      }
    }
  } 

  // if distance of destination is not inf, route exist, modify the vector
  if(distance[dest]<INF){
    long long temp = dest;
    while(temp!=start){
      route.push_back(temp);
      temp = discovery[temp];
    }
    route.push_back(start);
  }

  // return the distance of destination from start
  return distance[dest];
}

//////////////////////////////////////////////////////////////////
//
// main
//
int main()
{
  map<long long, Coordinates>  Nodes;     // maps a Node ID to it's coordinates (lat, lon)
  vector<FootwayInfo>          Footways;  // info about each footway, in no particular order
  vector<BuildingInfo>         Buildings; // info about each building, in no particular order
  XMLDocument                  xmldoc;
  
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "")
  {
    filename = def_filename;
  }

  //
  // Load XML-based map file 
  //
  if (!LoadOpenStreetMap(filename, xmldoc))
  {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  
  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == Nodes.size());
  assert(footwayCount == Footways.size());
  assert(buildingCount == Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


  //
  // Add vertices
  //
  graph<long long, double> G;
  map<long long, Coordinates>::iterator nit;
  for(nit=Nodes.begin();nit!=Nodes.end();nit++){
    G.addVertex(nit->first);
  }

  // Add edges
  vector<FootwayInfo>::iterator fit;
  vector<long long>::iterator it;
  for(fit=Footways.begin();fit!=Footways.end();fit++){
    vector<long long> temp = fit->Nodes;
    for(int i=0;i<temp.size()-1;i++){
      long long first = temp[i];
      long long second = temp[i+1];
      double distance = distBetween2Points(Nodes[first].Lat,Nodes[first].Lon,Nodes[second].Lat,Nodes[second].Lon);
      G.addEdge(first,second,distance);
      G.addEdge(second,first,distance);
    }
  }

  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#")
  {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);


    //
    // lookup buildings, find nearest start and dest nodes,
    // run Dijkstra's alg, output distance and path to destination:
    //
    bool startFlag = false;
    bool destFlag = false;
    int startIndex=-1,destIndex=-1;
    for(int i=0;i<Buildings.size();i++){
      if(Buildings[i].Abbrev==startBuilding){
        startIndex = i;
      }else if(Buildings[i].Abbrev==destBuilding){
        destIndex = i;
      }
    }
    if(startIndex<0){
      for(int i=0;i<Buildings.size();i++){
        if(Buildings[i].Fullname.find(startBuilding) != string::npos){
          startIndex = i;
        }
      }
    }
    
    if(destIndex<0){
      for(int i=0;i<Buildings.size();i++){
        if(Buildings[i].Fullname.find(destBuilding) != string::npos){
          destIndex = i;
        }
      }
    }

    //print error if start or destination doesn't exist
    //else print the building details and coordinates
    if(startIndex<0){
      cout << "Start building not found" << endl;
    }else if(destIndex<0){
      cout << "Destination building not found" << endl;
    }else{
      cout << "Starting point:\n";
      cout << " " << Buildings[startIndex].Fullname << "\n";
      cout << " (" << Buildings[startIndex].Coords.Lat << ", " << Buildings[startIndex].Coords.Lon << ")\n";
      cout << "Destination point:\n";
      cout << " " << Buildings[destIndex].Fullname << "\n";
      cout << " (" << Buildings[destIndex].Coords.Lat << ", " << Buildings[destIndex].Coords.Lon << ")\n";
    }

    //linearly search within all nodes, the one nearest to 
    //the start and destination building
    // min finding algorithm
    if(startIndex>=0 && destIndex>=0){
      cout << endl;
      long long startNode=-1;
      long long destNode=-1;
      double startNodeDistance=10000000,destNodeDistance=10000000;
      for(fit=Footways.begin();fit!=Footways.end();fit++){
        vector<long long> temp = fit->Nodes;
        for(int i=0;i<temp.size();i++){
          if(i==0||i==temp.size()-1){
            long long node =temp[i];
            double tempStartDistance = distBetween2Points(Buildings[startIndex].Coords.Lat,Buildings[startIndex].Coords.Lon,Nodes[node].Lat,Nodes[node].Lon); 
            double tempDestDistance = distBetween2Points(Buildings[destIndex].Coords.Lat,Buildings[destIndex].Coords.Lon,Nodes[node].Lat,Nodes[node].Lon);
            if(tempStartDistance < startNodeDistance){
              startNodeDistance = tempStartDistance;
              startNode = node;
            }
            if(tempDestDistance < destNodeDistance){
              destNodeDistance = tempDestDistance;
              destNode = node;
            }
          }
        }
      }

      // print the nearest nodes
      cout << "Nearest start node:\n";
      printf(" %lld\n",startNode);
      cout << " (" << Nodes[startNode].Lat << ", " << Nodes[startNode].Lon << ")\n";
      cout << "Nearest destination node:\n";
      printf(" %lld\n",destNode);
      cout << " (" << Nodes[destNode].Lat << ", " << Nodes[destNode].Lon << ")\n";

      cout << endl;
      cout << "Navigating with Dijkstra...\n";

      // get route and if route exist print it.
      vector<long long> route;
      route.clear();
      double distance = dijkstraNavigation(startNode,destNode,G,route);
      if(route.size()<1){
        cout << "Sorry, destination unreachable\n";
      }else{
        cout << "Distance to dest: " << distance << " miles\n";
        cout << "Path: " << route[route.size()-1];
        for(int i=route.size()-2;i>-1;i--){
          cout << "->" << route[i];
        }
        cout << endl;
      }
    }


    //
    // another navigation?
    //
    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  //
  // done:
  //
  cout << "** Done **" << endl;

  return 0;
}
