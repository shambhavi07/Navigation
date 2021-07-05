/*graph.h*/

//
// Shambhavi Danayak
//
// Basic graph class using List Representation
// original author: Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>

using namespace std;


template<typename VertexT, typename WeightT>
class graph
{
private:
  struct ListData
  {
    VertexT  vertex;
    WeightT  Weight;

    ListData(VertexT v, WeightT w)
    {
      vertex = v;
      Weight = w;
    }
  };

  map<VertexT, vector<ListData> > AdjList;
  vector<VertexT> vertices;

  //
  // _hasVertex
  //
  // returns true if the vertex is present in the graph
  // map find has a logN complexity and hence meets the requirement
  bool _hasVertex(VertexT v) const
  {
    return AdjList.find(v) != AdjList.end();
  }


public:
  //
  // constructor:
  // The map is cleared just for safety
  graph()
  {
    AdjList.clear();
  }

  graph(const graph &g)
  {
  	AdjList.clear();
  	vertices = g.getVertices();
  	for(VertexT v: vertices)
  	{
  		AdjList[v];
  		set<VertexT> s = g.neighbors(v);
  		for(VertexT n: s)
  		{
  			WeightT weight;
  			if(g.getWeight(v, n, weight))
  			{
  				ListData ld(n, weight);
  				AdjList[v].push_back(ld);
  			}
  		}
  	}
  }

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const
  {
    return static_cast<int>(this->AdjList.size());
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const
  {
    int count = 0;

    //
    // loop through the adjacency list and add the number of edges 
    // for each vertex
    //
    for (auto it = AdjList.begin(); it != AdjList.end(); ++it)
    {
      count += static_cast<int>(it->second.size());
    }

    return count;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v)
  {
    //
    // is the vertex already in the graph?  If so, we do not 
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (_hasVertex(v))
      return false;

    // we create a new vector for vertex v by calling the [] operator
    this->AdjList[v];
    vertices.push_back(v);
    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    // We check if the vertices exist already using _hasVertex
    // If not we return false
    if (!_hasVertex(from) || !_hasVertex(to))  // not found:
      return false;

    //
    // the vertices exist and create the list data for this edge
    // and push it to the vector
  	int size = AdjList.at(from).size();
    for(int i = 0; i < size; ++i)
    {
      // If the to vertex is found then weight is changed and
      // true is returned
      if(AdjList.at(from)[i].vertex == to)
      {
        AdjList.at(from)[i].Weight = weight;
        return true;
      }
    }
    ListData ld(to, weight);
    this->AdjList[from].push_back(ld);

    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If 
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not 
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const
  {
    // We check if the vertices exist already using _hasVertex
    // If not we return false
    if (!_hasVertex(from) || !_hasVertex(to))  // not found:
      return false;

    int size = this->AdjList.at(from).size();
    for(int i = 0; i < size; ++i)
    {
      // If the to vertex is found then weight is changed and
      // true is returned
      if(AdjList.at(from)[i].vertex == to)
      {
        weight = AdjList.at(from)[i].Weight;
        return true;
      }
    }

    // Vertices exist but the edge doesn't so false
    return false;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const
  {
    set<VertexT>  S;

    // We check if the vertiex exists already using _hasVertex
    // If not we return false
    if (!_hasVertex(v))
      return S;

    //
    // we loop along the list of neighbours of vertex v
    // and add the neighbours to the set
    int size = AdjList.at(v).size();
    for(int i = 0; i < size; ++i)
      S.insert(AdjList.at(v)[i].vertex);

    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const
  {
    return vertices;
  }

  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

    output << endl;
    output << "**Vertices:" << endl;
    
    // Looping through the adjacency list 
    for (auto it = AdjList.begin(); it != AdjList.end(); ++it)
    {
      output << " " << it->first << endl;
    }

    output << endl;
    output << "**Edges:" << endl;
    // Looping through the adjacency list 
    for (auto it = AdjList.begin(); it != AdjList.end(); ++it)
    {
      output << it->first << ": ";
      int size = it->second.size();
      for (int i = 0; i < size; ++i)
      {
          output << "("
            << it->first << ","
            << it->second[i].vertex << ","
            << it->second[i].Weight << ") ";
      }

      output << endl;
    }

    output << "**************************************************" << endl;
  }

};