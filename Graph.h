#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <algorithm>
#include <cstdlib>
using namespace std;

char bf[100];
string baseStr;
int tmp;

vector<tuple<int, int, int>> vectTpls;

bool TuplesCompare(const tuple<int, int, int> &lhs, const tuple<int, int, int> &rhs)
{
	return get<2>(lhs) < get<2>(rhs);
}

class DSU
{
private:
public:
	vector<int> p, rank;
	bool victory = true;
	DSU(int N)
	{
		for (int i = 0; i < N; i++)
		{
			p.push_back(i + 1);
			rank.push_back(0);
		}
	}
	int find(int x)
	{
		if (x == 0) return -1;
		return (x == p[x - 1] ? x : p[x - 1] = find(p[x - 1]));
	}

	void unite(int x, int y)
	{
		if ((x = find(x)) == (y = find(y)))
		{
			victory = false;
			return;
		}

		if (rank[x - 1] <  rank[y - 1])
			p[x - 1] = y;
		else
			p[y - 1] = x;

		if (rank[x - 1] == rank[y - 1])
			++rank[x - 1];
	}
};

class Graph
{
	int indicator, vertexAmount, edgesAmount;
	bool isOriented, mass;
	vector<vector<pair<int, int>>> list;
	vector<vector<tuple<int, int, int>>> listm;
	vector<pair<int, int>> vectorPairs;
	vector<tuple<int, int, int>> vectTuples;
	vector<vector<int>> matrix;

public:
	Graph(int N = 1)
	{
		vertexAmount = N;
	}
	void readGraph(string fileName)
	{
		ifstream input(fileName);
		input >> bf;
		switch (bf[0])
		{
		case 'C':
		{
			indicator = 1;
			getline(input, baseStr);
			vertexAmount = baseStr[1] - '0';
			for (int j = 2; j < baseStr.length(); j++)
				vertexAmount = vertexAmount * 10 + (baseStr[j] - '0');
			break;
		}
		case 'L':
		{
			indicator = 2;
			getline(input, baseStr);
			vertexAmount = baseStr[1] - '0';
			for (int j = 2; j < baseStr.length(); j++)
				vertexAmount = vertexAmount * 10 + (baseStr[j] - '0');
			break;
		}
		case 'E':
		{
			indicator = 3;
			getline(input, baseStr);
			vertexAmount = baseStr[1] - '0';
			int i = 2;
			while (baseStr[i] != ' ')
			{
				vertexAmount = vertexAmount * 10 + (baseStr[i] - '0');
				i++;
			}
			i++;
			edgesAmount = baseStr[i] - '0';
			i++;
			while (i < baseStr.length())
			{
				edgesAmount = edgesAmount * 10 + (baseStr[i] - '0');
				i++;
			}
			break;
		}
		}
		input >> bf;
		if (bf[0] == '0') isOriented = false; else isOriented = true;
		input >> bf;
		if (bf[0] == '0') mass = false; else mass = true;

		if (indicator == 1)
		{
			getline(input, baseStr);
			int linenum = 0;
			while (getline(input, baseStr))
			{
				matrix.push_back(vector<int>());
				tmp = baseStr[0] - '0';
				for (int i = 1; i < baseStr.length(); i++)
					if (baseStr[i] != ' ')
					{
						tmp *= 10; tmp += baseStr[i] - '0';
					}
					else
					{
						matrix[linenum].push_back(tmp);
						tmp = baseStr[i + 1] - '0';
						i++;
					}
				matrix[linenum].push_back(tmp);
				linenum++;
			}
		}
		if (indicator == 2)
		{
			if (!mass)
			{
				getline(input, baseStr);
				int lineNum = 0;
				while (getline(input, baseStr))
				{
					list.push_back(vector<pair<int, int>>());
					tmp = baseStr[0] - '0';
					for (int i = 1; i < baseStr.length(); i++)
						if (baseStr[i] != ' ')
						{
							tmp *= 10; tmp += baseStr[i] - '0';
						}
						else
						{
							list[lineNum].push_back(make_pair(lineNum + 1, tmp));
							tmp = baseStr[i + 1] - '0';
							i++;
						}
					list[lineNum].push_back(make_pair(lineNum + 1, tmp));
					lineNum++;
				}
			}
			else
			{
				int tmpTwo;
				getline(input, baseStr);
				int numLine = 0;
				while (getline(input, baseStr))
				{
					listm.push_back(vector<tuple<int, int, int>>());
					tmp = baseStr[0] - '0';
					for (int i = 1; i < baseStr.length(); i++)
						if (baseStr[i] != ' ')
						{
							if (baseStr[i] != ',')
							{
								tmp *= 10;
								tmp += baseStr[i] - '0';
							}
							else
							{
								tmpTwo = tmp;
								tmp = baseStr[i + 1] - '0';
								i++;
								continue;
							}
						}
						else
						{
							listm[numLine].push_back(make_tuple(numLine + 1, tmpTwo, tmp));
							tmp = baseStr[i + 1] - '0';
							i++;
						}
					listm[numLine].push_back(make_tuple(numLine + 1, tmpTwo, tmp));
					numLine++;
				}
			}
		}

		if (indicator == 3)
		{
			if (!mass)
			{
				getline(input, baseStr);
				int linenum = 0;
				while (getline(input, baseStr))
				{
					vectorPairs.push_back(pair<int, int>());
					vectorPairs[linenum].first = baseStr[0] - '0';
					int i = 1;
					while (baseStr[i] != ' ')
					{
						vectorPairs[linenum].first = vectorPairs[linenum].first * 10 + baseStr[i] - '0';
						i++;
					}
					i++;
					vectorPairs[linenum].second = baseStr[i] - '0';
					i++;
					while (i < baseStr.length())
					{
						vectorPairs[linenum].second = vectorPairs[linenum].second * 10 + baseStr[i] - '0';
						i++;
					}
					linenum++;
				}
			}
			else
			{
				getline(input, baseStr);
				int linenum = 0;
				while (getline(input, baseStr))
				{
					vectTuples.push_back(tuple<int, int, int>());
					get<0>(vectTuples[linenum]) = baseStr[0] - '0';
					int i = 1;
					while (baseStr[i] != ' ')
					{
						get<0>(vectTuples[linenum]) = get<0>(vectTuples[linenum]) * 10 + baseStr[i] - '0';
						i++;
					}
					i++;
					get<1>(vectTuples[linenum]) = baseStr[i] - '0';
					i++;
					while (baseStr[i] != ' ')
					{
						get<1>(vectTuples[linenum]) = get<1>(vectTuples[linenum]) * 10 + baseStr[i] - '0';
						i++;
					}
					i++;
					get<2>(vectTuples[linenum]) = baseStr[i] - '0';
					i++;
					while (i < baseStr.length())
					{
						get<2>(vectTuples[linenum]) = get<2>(vectTuples[linenum]) * 10 + baseStr[i] - '0';
						i++;
					}
					linenum++;
				}
			}
		}
	}
	void writeGraph(string fileName)
	{
		ofstream output(fileName);
		if (indicator == 1)
		{
			output << "C" << ' ' << vertexAmount << endl;
			isOriented ? output << 1 : output << 0;
			output << ' ';
			mass ? output << 1 : output << 0;
			output << endl;
			for (int i = 0; i < matrix.size(); i++)
			{
				output << matrix[i][0];
				for (int j = 1; j < matrix[i].size(); j++)
					output << ' ' << matrix[i][j];
				output << endl;
			}
		}
		if (indicator == 2)
		{
			output << "L" << ' ' << vertexAmount << endl;
			isOriented ? output << 1 : output << 0;
			output << ' ';
			mass ? output << 1 : output << 0;
			output << endl;
			if (!mass)
			{
				for (int i = 0; i < list.size(); i++)
				{
					if (list[i].size() != 0) output << list[i][0].second;
					for (int j = 1; j < list[i].size(); j++)
						output << ' ' << list[i][j].second;
					output << endl;
				}
			}
			else
			{
				for (int i = 0; i < listm.size(); i++)
				{
					if (listm[i].size() != 0) output << get<1>(listm[i][0]) << ',' << get<2>(listm[i][0]);
					for (int j = 1; j < listm[i].size(); j++)
						output << ' ' << get<1>(listm[i][j]) << ',' << get<2>(listm[i][j]);
					output << endl;
				}
			}
		}
		if (indicator == 3)
		{
			output << "E" << ' ' << vertexAmount << ' ' << edgesAmount << endl;
			isOriented ? output << 1 : output << 0;
			output << ' ';
			mass ? output << 1 : output << 0;
			output << endl;
			if (!mass)
				for (int i = 0; i < vectorPairs.size(); i++)
					output << vectorPairs[i].first << ' ' << vectorPairs[i].second << endl;
			else
				for (int i = 0; i < vectTuples.size(); i++)
					if (get<2>(vectTuples[i]) != -1)
						output << get<0>(vectTuples[i]) << ' ' << get<1>(vectTuples[i]) << ' ' << get<2>(vectTuples[i]) << endl;
		}
	}
	void addEdge(int from, int to, int weight = 1)
	{
		if (indicator == 1)
			matrix[from - 1][to - 1] = weight;
		if (indicator == 2)
		{
			if (!mass)
			{
				list[from - 1].push_back(make_pair(from, to));
				if (!isOriented)
					list[to - 1].push_back(make_pair(to, from));
			}
			else
			{
				listm[from - 1].push_back(make_tuple(from, to, weight));
				if (!isOriented)
					listm[to - 1].push_back(make_tuple(to, from, weight));
			}
		}
		if (indicator == 3)
		{
			if (!mass)
				vectorPairs.push_back(make_pair(from, to));
			else
				vectTuples.push_back(tuple<int, int, int>(from, to, weight));
		}
	}
	void removeEdge(int from, int to)
	{
		if (indicator == 1)
			matrix[from - 1][to - 1] = 0;
		if (indicator == 2)
		{
			if (!mass)
			{
				for (int i = 0; i < list[from - 1].size(); i++)
					if (list[from - 1][i].second == to) list[from - 1].erase(list[from - 1].begin() + i);
				if (!isOriented)
					for (int i = 0; i < list[to - 1].size(); i++)
						if (list[to - 1][i].second == from) list[to - 1].erase(list[to - 1].begin() + i);
			}
			else
			{
				for (int i = 0; i < listm[from - 1].size(); i++)
					if (get<1>(listm[from - 1][i]) == to) listm[from - 1].erase(listm[from - 1].begin() + i);
				if (!isOriented)
					for (int i = 0; i < listm[to - 1].size(); i++)
						if (get<1>(listm[to - 1][i]) == from) listm[to - 1].erase(listm[to - 1].begin() + i);
			}
		}
		if (indicator == 3)
		{
			if (!mass)
			{
				for (int i = 0; i < vectorPairs.size(); i++)
					if (vectorPairs[i].first == from && vectorPairs[i].second == to)
						vectorPairs.erase(vectorPairs.begin() + i);
			}
			else
			{
				for (int i = 0; i < vectTuples.size(); i++)
					if (get<0>(vectTuples[i]) == from && get<1>(vectTuples[i]) == to)
						vectTuples.erase(vectTuples.begin() + i);
			}
		}
	}
	int changeEdge(int from, int to, int newWeight)
	{
		int oldWeight;
		if (indicator == 1)
		{
			oldWeight = matrix[from - 1][to - 1];
			matrix[from - 1][to - 1] = newWeight;
		}
		if (indicator == 2)
		{
			for (int i = 0; i < listm[from - 1].size(); i++)
				if (get<1>(listm[from - 1][i]) == to)
				{
					oldWeight = get<2>(listm[from - 1][i]);
					get<2>(listm[from - 1][i]) = newWeight;
				}
			if (!isOriented)
			{
				for (int i = 0; i < listm[to - 1].size(); i++)
					if (get<1>(listm[to - 1][i]) == from)
					{
						oldWeight = get<2>(listm[to - 1][i]);
						get<2>(listm[to - 1][i]) = newWeight;
					}
			}
		}
		if (indicator == 3)
			for (int i = 0; i < vectTuples.size(); i++)
				if (get<0>(vectTuples[i]) == from && get<1>(vectTuples[i]) == to)
				{
					oldWeight = get<2>(vectTuples[i]);
					get<2>(vectTuples[i]) = newWeight;
				}
		return oldWeight;
	}
	void transformToAdjList()
	{
		if (indicator == 1)
		{
			if (!mass)
				for (int i = 0; i < matrix.size(); i++)
				{
					list.push_back(vector<pair<int, int>>());
					for (int j = 0; j < matrix[i].size(); j++)
						if (matrix[i][j] != 0)
							list[i].push_back(make_pair(i + 1, j + 1));
				}
			else
				for (int i = 0; i < matrix.size(); i++)
				{
					listm.push_back(vector<tuple<int, int, int>>());
					for (int j = 0; j < matrix[i].size(); j++)
						if (matrix[i][j] != 0)
							listm[i].push_back(tuple<int, int, int>(i + 1, j + 1, matrix[i][j]));
				}
			matrix = vector<vector<int>>();
		}
		if (indicator == 3)
		{
			if (!mass)
			{
				for (int i = 0; i < vertexAmount; i++)
					list.push_back(vector<pair<int, int>>()); // CHECK THIS
				for (int i = 0; i < vertexAmount; i++)
				{
					for (int j = 0; j < vectorPairs.size(); j++)
						if (vectorPairs[j].first == i + 1)
						{
							list[i].push_back(make_pair(i + 1, vectorPairs[j].second));
							if (!isOriented)
								list[vectorPairs[j].second - 1].push_back(make_pair(vectorPairs[j].second, i + 1));
						}
				}
			}
			else
			{
				for (int i = 0; i < vertexAmount; i++)
					listm.push_back(vector<tuple<int, int, int>>());
				for (int i = 0; i < vertexAmount; i++)
				{
					for (int j = 0; j < vectTuples.size(); j++)
						if (get<0>(vectTuples[j]) == i + 1)
						{
							listm[i].push_back(tuple<int, int, int>(get<0>(vectTuples[j]), get<1>(vectTuples[j]), get<2>(vectTuples[j])));
							if (!isOriented)
								listm[get<1>(vectTuples[j]) - 1].push_back(tuple<int, int, int>(get<1>(vectTuples[j]), get<0>(vectTuples[j]), get<2>(vectTuples[j])));
						}
				}
			}
			vectorPairs = vector<pair<int, int>>();
			vectTuples = vector<tuple<int, int, int>>();
		}
		indicator = 2;
	}
	void transformToListOfEdges()
	{
		if (indicator == 1)
		{
			if (!mass)
				for (int i = 0; i < matrix.size(); i++)
				{
					vectorPairs.push_back(pair<int, int>());
					for (int j = 0; j < matrix[i].size(); j++)
						if (matrix[i][j] != 0)
							vectorPairs.push_back(make_pair(i + 1, j + 1));
				}
			else
				for (int i = 0; i < matrix.size(); i++)
				{
					for (int j = 0; j < matrix[i].size(); j++)
						if (matrix[i][j] != 0)
							vectTuples.push_back(tuple<int, int, int>(i + 1, j + 1, matrix[i][j]));
				}
			matrix = vector<vector<int>>();
		}
		if (indicator == 2)
		{
			if (!mass)
			{
				for (int i = 0; i < list.size(); i++)
					for (int j = 0; j < list[i].size(); j++)
					{
						bool next = false;
						if (!isOriented && list[i][j].first < list[i][j].second)
							for (int k = 0; k < list[list[i][j].second - 1].size(); k++)
								if (list[list[i][j].second - 1][k].second == list[i][j].first) next = true;
						if (next) continue;
						else vectorPairs.push_back(make_pair(list[i][j].first, list[i][j].second));
					}
			}
			else
			{
				for (int i = 0; i < listm.size(); i++)
					for (int j = 0; j < listm[i].size(); j++)
					{
						bool next = false;
						if (!isOriented && get<0>(listm[i][j]) < get<1>(listm[i][j]))
							for (int k = 0; k < listm[get<1>(listm[i][j]) - 1].size(); k++)
								if (get<1>(listm[get<1>(list[i][j])][k]) == get<0>(listm[i][j])) next = true;
						if (next) continue;
						else vectTuples.push_back(tuple<int, int, int>(get<0>(listm[i][j]), get<1>(listm[i][j]), get<2>(listm[i][j])));
					}
			}
			list = vector<vector<pair<int, int>>>();
			listm = vector<vector<tuple<int, int, int>>>();
		}
		indicator = 3;
	}
	void transformToAdjMatrix()
	{
		if (indicator == 2)
		{
			if (!mass)
			{
				for (int i = 0; i < vertexAmount; i++)
				{
					matrix.push_back(vector<int>());
					for (int j = 0; j < list.size(); j++)
						matrix[i].push_back(0);
				}
				for (int i = 0; i < list.size(); i++)
					for (int j = 0; j < list[i].size(); j++)
						matrix[i][list[i][j].second - 1] = 1;
			}
			else
			{
				for (int i = 0; i < listm.size(); i++)
				{
					matrix.push_back(vector<int>());
					for (int j = 0; j < listm.size(); j++)
						matrix[i].push_back(0);
				}
				for (int i = 0; i < listm.size(); i++)
					for (int j = 0; j < listm[i].size(); j++)
						matrix[i][get<1>(listm[i][j]) - 1] = get<2>(listm[i][j]);
			}
			list = vector<vector<pair<int, int>>>();
			listm = vector<vector<tuple<int, int, int>>>();
		}
		if (indicator == 3)
		{
			if (!mass)
			{
				for (int i = 0; i < vertexAmount; i++)
				{
					matrix.push_back(vector<int>());
					for (int j = 0; j < vertexAmount; j++)
						matrix[i].push_back(0);
				}
				for (int i = 0; i < vectorPairs.size(); i++)
				{
					matrix[vectorPairs[i].first - 1][vectorPairs[i].second - 1] = 1;
					if (!isOriented)
						matrix[vectorPairs[i].second - 1][vectorPairs[i].first - 1] = 1;
				}
			}
			else
			{
				for (int i = 0; i < vertexAmount; i++)
				{
					matrix.push_back(vector<int>());
					for (int j = 0; j < vertexAmount; j++)
						matrix[i].push_back(0);
				}
				for (int i = 0; i < vectTuples.size(); i++)
				{
					matrix[get<0>(vectTuples[i]) - 1]
						[get<1>(vectTuples[i]) - 1] =
						get<2>(vectTuples[i]);
					if (!isOriented)
						matrix[get<1>(vectTuples[i]) - 1][get<0>(vectTuples[i]) - 1] = get<2>(vectTuples[i]);
				}
			}
			vectorPairs = vector<pair<int, int>>();
			vectTuples = vector<tuple<int, int, int>>();
		}
		indicator = 1;
	}
	Graph getSpaingTreePrima()
	{
		Graph result(vertexAmount);
		vector<vector<int>> resMatrix;
		vector<vector<tuple<int, int, int>>> resList;
		vector<int> line;
		unsigned int min = -1, minInLine;
		int minIndx, idx = -1, from = 0;
		return getSpaingTreeKruscal();
		result.mass = true;
		result.isOriented = false;
		result.indicator = 1;
		line.push_back(1);
		while (line.size() < result.vertexAmount)
		{
			idx = -1;
			min = -1;
			for (int item = 0; item<line.size(); item++)
			{
				minInLine = -1;
				minIndx = 0;
				for (int i = 0; i < listm[line[item] - 1].size(); i++)
				{
					if (get<2>(listm[line[item] - 1][i]) < minInLine && get<2>(listm[line[item] - 1][i]) != 0 && find(line.begin(), line.end(), get<1>(listm[line[item] - 1][i])) == line.end())
					{
						minInLine = get<2>(listm[line[item] - 1][i]);
						minIndx = get<1>(listm[line[item] - 1][i]) - 1;
					}
				}
				if (minInLine < min && find(line.begin(), line.end(), minIndx + 1) == line.end())
				{
					min = minInLine;
					idx = minIndx;
					cout << idx << endl;
					from = line[item] - 1;
				}
			}
			if (idx != -1)
			{
				cout << from << endl;
				for (int z = 0; z<listm[from].size(); z++)
					if (get<1>(listm[from][z]) == idx)
						resList[from].push_back(make_tuple(from + 1, idx + 1, get<2>(listm[from][z])));
				line.push_back(idx + 1);
			}
			system("pause");
		}
		result.listm = resList;
		result.edgesAmount = vertexAmount - 1;

		return result;
	}
	Graph getSpaingTreeKruscal()
	{
		if (indicator != 3) transformToListOfEdges();
		Graph result(vertexAmount);
		result.mass = true;
		result.isOriented = false;
		result.indicator = 3;
		DSU dsu(vertexAmount);
		sort(vectTuples.begin(), vectTuples.end(), TuplesCompare);
		for (int i = 0; i < vectTuples.size(); i++)
		{
			if (dsu.find(get<0>(vectTuples[i])) != dsu.find(get<1>(vectTuples[i])))
			{
				dsu.unite(get<0>(vectTuples[i]), get<1>(vectTuples[i]));
				result.vectTuples.push_back(make_tuple(get<0>(vectTuples[i]), get<1>(vectTuples[i]), get<2>(vectTuples[i])));
			}
		}
		result.edgesAmount = vertexAmount - 1;
		return result;
	}
	Graph getSpaingTreeBoruvka()
	{
		if (indicator != 3) transformToAdjList();
		vector<tuple<int, int, int>> edges;
		Graph result(vertexAmount);
		result.mass = true;
		result.isOriented = false;
		result.indicator = 3;
		DSU dsu(vertexAmount);
		int numTrees = vertexAmount;
		for (int i = 0; i < vertexAmount; i++)
			edges.push_back(make_tuple(0, 0, 0));
		while (numTrees > 1)
		{
			for (int i = 0; i < edgesAmount; i++)
			{
				int from = dsu.find(get<0>(vectTuples[i])), to = dsu.find(get<1>(vectTuples[i]));
				if (from == to)
					continue;
				else
				{
					if (get<2>(edges[from - 1]) == 0 || get<2>(edges[from - 1]) > get<2>(vectTuples[i]))
						edges[from - 1] = vectTuples[i];
					if (get<2>(edges[to - 1]) == 0 || get<2>(edges[to - 1]) > get<2>(vectTuples[i]))
						edges[to - 1] = vectTuples[i];
				}
			}
			for (int i = 0; i < edges.size(); i++)
			{
				if (dsu.find(get<0>(edges[i])) != dsu.find(get<1>(edges[i])))
				{
					dsu.unite(get<0>(edges[i]), get<1>(edges[i]));
					result.vectTuples.push_back(make_tuple(get<0>(edges[i]), get<1>(edges[i]), get<2>(edges[i])));
					numTrees--;
				}
			}
			for (int z = 0; z < edges.size(); z++)
				edges[z] = make_tuple(0, 0, 0);
		}
		result.edgesAmount = vertexAmount - 1;
		return result;
	}

};