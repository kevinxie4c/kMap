#include "stdafx.h"
#include "FileIO.h"
#include "Graph.h"
#include <cstdio>
#include "vector.h"
#include "tstring.h"

#define FILE_NAME "kMap.dat"

void ReadString(FILE *f, std::tstring &s)
{
	size_t length;
	fread(&length, sizeof length, 1, f);
	TCHAR *buffer = new TCHAR[length + 1];
	fread(buffer, sizeof TCHAR, length + 1, f);
	s = buffer;
}

void WriteString(FILE *f, const std::tstring &s)
{
	size_t length = lstrlen(s.c_str());
	fwrite(&length, sizeof length, 1, f);
	fwrite(s.c_str(), sizeof TCHAR, length + 1, f);
}

bool OpenFile(CGraph &graph)
{
	FILE *f;
	fopen_s(&f, FILE_NAME, "rb");
	if (f == NULL) return false;
	size_t length;
	fread(&length, sizeof length, 1, f);
	for (size_t i = 0; i < length; ++i)
	{
		int x, y;
		std::tstring name, description, position;
		fread(&x, sizeof x, 1, f);
		fread(&y, sizeof y, 1, f);
		ReadString(f, name);
		ReadString(f, description);
		ReadString(f, position);
		graph.InsertVertex(name, description, position, x, y);
		size_t t_length;
		fread(&t_length, sizeof t_length, 1, f);
		for (size_t j = 0; j < t_length; ++j)
		{
			int distance, time;
			std::tstring src, dst;
			fread(&distance, sizeof distance, 1, f);
			fread(&time, sizeof time, 1, f);
			ReadString(f, src);
			ReadString(f, dst);
			graph.InsertEdge(src, dst, distance, time);
		}

	}
	fclose(f);
	return true;
}

bool SaveFile(const CGraph &graph)
{
	using namespace std;
	FILE *f;
	vector<SVertex> v;
	vector<SEdge> e;
	fopen_s(&f, FILE_NAME, "wb");
	if (f == NULL) return false;
	graph.GetVertexsVector(v);
	size_t length = v.size();
	fwrite(&length, sizeof length, 1, f);
	for (auto &c : v)
	{
		fwrite(&(c.x), sizeof c.x, 1, f);
		fwrite(&(c.y), sizeof c.y, 1, f);
		WriteString(f, c.name);
		WriteString(f, c.description);
		WriteString(f, c.position);
		graph.GetEdgesVector(c.name, e);
		length = e.size();
		fwrite(&length, sizeof length, 1, f);
		for (auto &d : e)
		{
			fwrite(&(d.distance), sizeof d.distance, 1, f);
			fwrite(&(d.time), sizeof d.time, 1, f);
			WriteString(f, d.src);
			WriteString(f, d.dst);
		}
	}
	fclose(f);
	return true;
}

