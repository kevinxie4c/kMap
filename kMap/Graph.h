#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include "map.h"
#include "vector.h"
#include <set>
#include "tstring.h"

typedef struct
{
	std::tstring name, description, position;
	int x, y;
}SVertex;

typedef struct
{
	std::tstring  src, dst;
	int distance, time;
}SEdge;

class CGraph
{
public:
	void  Clear()
	{
		vertexs.clear();
		edges.clear();
	}
	bool VertexIsExist(const std::tstring &name) const
	{
		if (vertexs.find(name) == vertexs.end()) 
			return false;
		else return true;
	}
	bool EdgeIsExist(const std::tstring &v1, const std::tstring &v2) const
	{
		//if (edges[v1].find(v2) == edges[v1].end())
		map<std::tstring, map<std::tstring, SEdge> >::const_iterator tmp = edges.find(v1);
		if (tmp == edges.end()) return false;
		if (tmp->second.find(v2) == tmp->second.end()) return false;
		else return true;
	}
	void InsertVertex(const std::tstring &name, const std::tstring &description, const std::tstring &position, const int x, const int y)
	{
		vertexs[name].name = name;
		vertexs[name].description = description;
		vertexs[name].position = position;
		vertexs[name].x = x;
		vertexs[name].y = y;
	}
	void DeleteVertex(const std::tstring &name)
	{
		for (auto e : edges[name])
			edges[e.first].erase(name);
		edges.erase(name);
		vertexs.erase(name);
	}
	void InsertEdge(const std::tstring &v1, const std::tstring &v2, int distance, int time)
	{
		edges[v1][v2].src = edges[v2][v1].dst = v1;
		edges[v1][v2].dst = edges[v2][v1].src = v2;
		edges[v1][v2].distance = edges[v2][v1].distance = distance;
		edges[v1][v2].time = edges[v2][v1].time = time;
	}
	void DeleteEdge(const std::tstring &v1, const std::tstring &v2)
	{
		edges[v1].erase(v2);
		edges[v2].erase(v1);
	}
	SVertex GetVertex(const std::tstring &name) const
	{
		//return vertexs[name];
		return vertexs.find(name)->second;
	}
	SEdge GetEdge(const std::tstring &v1, const std::tstring &v2) const
	{
		//return edges[v1][v2];
		return edges.find(v1)->second.find(v2)->second;
	}
	void GetVertexNameVector(vector<std::tstring> &v) const
	{
		v.clear();
		for (auto t : vertexs)
			v.push_back(t.second.name);
	}
	void GetRealVertexsNameVector(vector<std::tstring> &v) const
	{
		v.clear();
		for (auto t : vertexs)
		if (t.second.description != TEXT("#null"))
			v.push_back(t.second.name);
	}
	void GetVertexsVector(vector<SVertex> &v) const
	{
		v.clear();
		for (auto t : vertexs)
			v.push_back(t.second);
	}
	void GetEdgesNameVector(const std::tstring &name, vector<std::tstring> &v) const
	{
		v.clear();
		if (edges.find(name) == edges.end()) return;
		//for (auto t : edges[name])
		for (auto t : edges.find(name)->second)
			v.push_back(t.first);
	}
	void GetEdgesVector(const std::tstring &name, vector<SEdge> &v) const
	{
		v.clear();
		if (edges.find(name) == edges.end()) return;
		//for (auto t : edges[name])
		for (auto t : edges.find(name)->second)
			v.push_back(t.second);
	}
	bool Dijkstra(const std::tstring &src, const std::tstring &dst, vector<std::tstring> &path)
	{
		const int infinity = 0x7f7f7f7f;
		map<std::tstring, int> dist;
		std::set<std::tstring> s;
		map<std::tstring, std::tstring> prev;
		std::tstring u;
		path.clear();
		for (auto t : vertexs)
			dist[t.first] = infinity;
		dist[src] = 0;
		for (auto t : vertexs)
			s.insert(t.first);
		while (!s.empty())
		{
			u = ExractMax(s, dist);
			if (u == dst)
			{
				path.push_back(dst);
				std::tstring tmp = dst;
				while ((tmp = prev[tmp]) != src) path.push_back(tmp);
				path.push_back(src);
				return true;
			}
			else if (u == TEXT("")) return false;
			for (auto v : edges[u])
			{
				if (dist[v.first] > dist[u] + v.second.distance)
				{
					dist[v.first] = dist[u] + v.second.distance;
					prev[v.first] = u;
				}
			}
		}
		return false;
	}
private:
	map<std::tstring, SVertex> vertexs;
	map<std::tstring, map<std::tstring, SEdge> > edges;
	std::tstring ExractMax(std::set<std::tstring> &s, map<std::tstring, int> dist)
	{
		const int infinity = 0x7f7f7f7f;
		int max = infinity;
		std::tstring res = TEXT("");
		for (auto c : s)
		if (dist[c] < max)
		{
			max = dist[c];
			res = c;
		}
		s.erase(res);
		return res;
	}
};

#endif