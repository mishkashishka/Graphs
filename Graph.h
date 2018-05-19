#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <queue>
#include <functional>
#include <stack>

using namespace std;

class AdjMatrixGraphRepresentation;
class AdjListGraphRepresentation;
class EdgeListGraphRepresentation;
class Graph;

typedef vector<vector<int>*>* AdjacencyMatrix;
typedef vector<map<int, int>*>* AdjacencyList;
typedef map<pair<int, int>, int>* EdgeList;
typedef map<int, int>* Vertexes;
typedef vector<map<int, bool>*>* MarkedEdges;


class StringParser
{
public:
	static vector<string> split(const string& str, char c)
	{
		vector<string> result;
		istringstream stream(str);
		string token;
		while (getline(stream, token, c))
			result.push_back(token);
		return result;
	}
};

class DSU {
private:
	vector<int>* parent;
	vector<int>* size;

public:
	DSU() {}
	DSU(int n) {
		parent = new vector<int>(n);
		size = new vector<int>(n);
	}
	~DSU() {
		delete parent;
		delete size;
	}

	void makeSet(int v) {
		(*parent)[v] = v;
		(*size)[v] = 1;
	}

	int find(int x) {
		if ((*parent)[x] == x)
			return x;
		int xRoot = find((*parent)[x]);
		(*parent)[x] = xRoot;
		return xRoot;
	}

	void unite(int x, int y) {
		int xRoot = find(x);
		int yRoot = find(y);
		if (xRoot != yRoot) {
			if ((*size)[xRoot] >= (*size)[yRoot])
			{
				(*size)[xRoot] += (*size)[yRoot];
				(*parent)[yRoot] = xRoot;
			}
			else
			{
				(*size)[yRoot] += (*size)[xRoot];
				(*parent)[xRoot] = yRoot;
			}
		}
	}

	map<int, int> getRoots() {
		map<int, int> roots = map<int, int>();
		for (int i = 0; i < parent->size(); ++i)
			if (i == (*parent)[i])
				roots[i] = (*size)[i];
		return roots;
	}
};


class GraphRepresentation
{
protected:
	bool isOriented, isWeighted;
	Vertexes vertexDegrees = nullptr;
	virtual DSU* buildDSU() = 0;

public:

	int edgesAmount = 0;
	GraphRepresentation() = default;
	virtual ~GraphRepresentation() = default;

	virtual void readGraph(istream& stream, vector<string> parameters) = 0;
	virtual void addEdge(int from, int to, int weight) = 0;
	virtual void removeEdge(int from, int to) = 0;
	virtual int changeEdge(int from, int to, int newWeight) = 0;
	virtual AdjMatrixGraphRepresentation* transformToAdjMatrix() = 0;
	virtual AdjListGraphRepresentation* transformToAdjList() = 0;
	virtual EdgeListGraphRepresentation* transformToListOfEdges() = 0;
	virtual void writeGraph(string fileName) = 0;
	virtual bool isAdjMatrixGraph() = 0;
	virtual bool isAdjListGraph() = 0;
	virtual bool isEdgeListGraph() = 0;
	int getBeginVertex(bool& circleExists) {
		int evenVertexesAmount = 0;
		int oddVertex = -1;
		for (auto it = vertexDegrees->begin(); it != vertexDegrees->end(); ++it) {
			if (it->second % 2 == 0)
				++evenVertexesAmount;
			else
				oddVertex = it->first;
		}
		circleExists = evenVertexesAmount == vertexDegrees->size();
		if (circleExists)
			return 0;
		if (vertexDegrees->size() - evenVertexesAmount <= 2)
			return oddVertex;
		return -1;
	}
	bool connectedComponentsCheck() {
		DSU* dsu = buildDSU();
		map<int, int> roots = dsu->getRoots();
		if (roots.size() <= 1)
			return true;
		int nonEmptyComponentsAmount = 0;
		for (auto it = roots.begin(); it != roots.end(); ++it)
			if (it->second > 1)
				nonEmptyComponentsAmount++;
		return nonEmptyComponentsAmount <= 1;
	}

	virtual int calc() = 0;
};

class AdjMatrixGraphRepresentation : GraphRepresentation
{
	AdjacencyMatrix adjacencyMatrix = nullptr;
	AdjacencyMatrix readMatrix(istream& stream, int vertexAmount)
	{
		const AdjacencyMatrix matrix = new vector<vector<int>*>(vertexAmount);
		vertexDegrees = new map<int, int>();
		for (size_t i = 0; i < vertexAmount; ++i)
		{
			(*vertexDegrees)[i] = 0;
			(*matrix)[i] = new vector<int>(vertexAmount);
			string line;
			getline(stream, line);
			vector<string> tokens = StringParser::split(line, ' ');
			for (size_t j = 0; j < vertexAmount; ++j) {
				int weight = stoi(tokens[j]);
				(*(*matrix)[i])[j] = weight;
				if (weight > 0) {
					++(*vertexDegrees)[i];
					++edgesAmount;
					if (!isOriented)
						++(*vertexDegrees)[j];
				}
			}
		}
		return matrix;
	}
	static void writeMatrix(ostream& stream, AdjacencyMatrix matrix)
	{
		for (size_t i = 0; i < matrix->size(); ++i)
		{
			for (size_t j = 0; j < matrix->size(); ++j)
			{
				stream << (*(*matrix)[i])[j];
				if (j < matrix->size() - 1)
					stream << " ";
			}
			if (i < matrix->size() - 1)
				stream << endl;
		}
	}
	void copy(const AdjMatrixGraphRepresentation& graph) {
		this->isOriented = graph.isOriented;
		this->isWeighted = graph.isWeighted;
		this->edgesAmount = graph.edgesAmount;
		this->vertexDegrees = new map<int, int>(*graph.vertexDegrees);
		this->adjacencyMatrix = new vector<vector<int>*>(graph.adjacencyMatrix->size());
		for (size_t i = 0; i < adjacencyMatrix->size(); ++i)
			(*adjacencyMatrix)[i] = new vector<int>(*(*graph.adjacencyMatrix)[i]);
	}
	DSU* buildDSU() {
		DSU* dsu = new DSU(adjacencyMatrix->size());
		for (int i = 0; i < adjacencyMatrix->size(); ++i)
			dsu->makeSet(i);
		for (int i = 0; i < adjacencyMatrix->size(); ++i)
			for (int j = 0; j < adjacencyMatrix->size(); ++j)
				dsu->unite(i, j);
		return dsu;
	}

public:
	AdjMatrixGraphRepresentation() = default;
	AdjMatrixGraphRepresentation(int vertexAmount, int edgesAmount, bool isOriented, bool isWeighted)
	{
		this->vertexDegrees = new map<int, int>();
		this->adjacencyMatrix = new vector<vector<int>*>(vertexAmount);
		for (int i = 0; i < vertexAmount; ++i)
			(*adjacencyMatrix)[i] = new vector<int>(vertexAmount, 0);
		this->isOriented = isOriented;
		this->isWeighted = isWeighted;
		this->edgesAmount = edgesAmount;
	}
	AdjMatrixGraphRepresentation(const AdjMatrixGraphRepresentation& graph)
	{
		copy(graph);
	}
	~AdjMatrixGraphRepresentation()
	{
		delete vertexDegrees;
		for (int i = 0; i < adjacencyMatrix->size(); ++i)
			delete (*adjacencyMatrix)[i];
		delete adjacencyMatrix;
	};
	AdjMatrixGraphRepresentation& operator=(const AdjMatrixGraphRepresentation& graph) {
		copy(graph);
		return *this;
	}

	bool isAdjMatrixGraph() override { return true; }
	bool isAdjListGraph() override { return false; }
	bool isEdgeListGraph() override { return false; }

	void readGraph(istream& stream, vector<string> parameters) override
	{
		const int vertexAmount = stoi(parameters[1]);

		isOriented = parameters[2] == "1";
		isWeighted = parameters[3] == "1";

		adjacencyMatrix = readMatrix(stream, vertexAmount);
	}


	void addEdge(int from, int to, int weight) override
	{
		++edgesAmount;
		++(*vertexDegrees)[from];
		(*(*adjacencyMatrix)[from])[to] = weight;
		if (!isOriented) {
			++(*vertexDegrees)[to];
			(*(*adjacencyMatrix)[to])[from] = weight;
		}
	}


	void removeEdge(int from, int to)  override
	{
		--edgesAmount;
		--(*vertexDegrees)[from];
		(*(*adjacencyMatrix)[from])[to] = 0;
		if (!isOriented) {
			--(*vertexDegrees)[to];
			(*(*adjacencyMatrix)[to])[from] = 0;
		}
	}


	int changeEdge(int from, int to, int newWeight)  override
	{
		const int oldWeight = (*(*adjacencyMatrix)[from])[to];
		addEdge(from, to, newWeight);
		return oldWeight;
	}


	AdjMatrixGraphRepresentation* transformToAdjMatrix()  override
	{
		return this;
	}


	AdjListGraphRepresentation* transformToAdjList()  override;


	EdgeListGraphRepresentation* transformToListOfEdges()  override;


	void writeGraph(string fileName) override
	{
		ofstream file(fileName);
		file << "C " << adjacencyMatrix->size() << endl;
		file << (isOriented ? "1 " : "0 ") << (isWeighted ? "1" : "0") << endl;

		writeMatrix(file, adjacencyMatrix);

		file.close();
	}


	int calc() {
		return 0;
	}

	friend Graph;
};

class AdjListGraphRepresentation : GraphRepresentation
{
	AdjacencyList adjacencyList = nullptr;
	AdjacencyList readList(istream& stream, int vertexAmount, bool isWeighted)
	{
		const AdjacencyList list = new vector<map<int, int>*>(vertexAmount);
		vertexDegrees = new map<int, int>();
		for (size_t i = 0; i < vertexAmount; ++i)
		{
			(*vertexDegrees)[i] = 0;
			(*list)[i] = new map<int, int>();
			string line;
			getline(stream, line);
			vector<string> tokens = StringParser::split(line, ' ');
			for (size_t j = 0; j < tokens.size(); ++j)
			{
				int vertexId, weight = 1;
				if (isWeighted)
				{
					vertexId = stoi(tokens[j]) - 1;
					weight = stoi(tokens[++j]);
				}
				else
					vertexId = stoi(tokens[j]) - 1;
				++edgesAmount;
				++(*vertexDegrees)[i];
				(*(*list)[i])[vertexId] = weight;
				if (!isOriented)
					++(*vertexDegrees)[vertexId];
			}
		}
		return list;
	}
	static void writeList(ostream& stream, AdjacencyList list, bool isWeighted)
	{
		for (size_t i = 0; i < list->size(); ++i)
		{
			map<int, int>* currentVertexMap = (*list)[i];
			for (auto edge = currentVertexMap->begin(); edge != currentVertexMap->end(); ++edge)
			{
				stream << edge->first + 1;
				if (isWeighted)
					stream << " " << edge->second;
				if (next(edge) != currentVertexMap->end())
					stream << " ";
			}
			if (i < list->size() - 1)
				stream << endl;
		}
	}
	void removeEdgeSimplex(AdjacencyList adjacencyList, int from, int to)
	{
		const auto it = (*(*adjacencyList)[from]).find(to);
		(*(*adjacencyList)[from]).erase(it);
		--(*vertexDegrees)[from];
	}
	void copy(const AdjListGraphRepresentation& graph) {
		this->isOriented = graph.isOriented;
		this->isWeighted = graph.isWeighted;
		this->edgesAmount = graph.edgesAmount;
		this->vertexDegrees = new map<int, int>(*graph.vertexDegrees);
		this->adjacencyList = new vector<map<int, int>*>(graph.adjacencyList->size());
		for (size_t i = 0; i < adjacencyList->size(); ++i)
			(*adjacencyList)[i] = new map<int, int>(*(*graph.adjacencyList)[i]);
	}
	DSU* buildDSU() {
		DSU* dsu = new DSU(adjacencyList->size());
		for (int i = 0; i < adjacencyList->size(); ++i)
			dsu->makeSet(i);
		for (int i = 0; i < adjacencyList->size(); ++i) {
			map<int, int>* edges = (*adjacencyList)[i];
			for (auto it = edges->begin(); it != edges->end(); ++it)
				dsu->unite(i, it->first);
		}
		return dsu;
	}

public:
	AdjListGraphRepresentation() = default;
	AdjListGraphRepresentation(int vertexAmount, int edgesAmount, bool isOriented, bool isWeighted)
	{
		this->vertexDegrees = new map<int, int>();
		this->edgesAmount = edgesAmount;
		this->adjacencyList = new vector<map<int, int>*>(vertexAmount);
		for (int i = 0; i < adjacencyList->size(); ++i)
			(*adjacencyList)[i] = new map<int, int>();
		this->isOriented = isOriented;
		this->isWeighted = isWeighted;
	}
	AdjListGraphRepresentation(const AdjListGraphRepresentation& graph) {
		copy(graph);
	}
	~AdjListGraphRepresentation()
	{
		delete vertexDegrees;
		for (size_t i = 0; i < adjacencyList->size(); ++i)
			delete (*adjacencyList)[i];
		delete adjacencyList;
	}
	AdjListGraphRepresentation& operator=(const AdjListGraphRepresentation& graph) {
		copy(graph);
		return *this;
	}

	bool isAdjMatrixGraph() override { return false; }
	bool isAdjListGraph() override { return true; }
	bool isEdgeListGraph() override { return false; }

	void readGraph(istream& stream, vector<string> parameters) override
	{
		const int vertexAmount = stoi(parameters[1]);

		isOriented = parameters[2] == "1";
		isWeighted = parameters[3] == "1";

		adjacencyList = readList(stream, vertexAmount, isWeighted);
	}


	void addEdge(int from, int to, int weight) override
	{
		++edgesAmount;
		++(*vertexDegrees)[from];
		(*(*adjacencyList)[from])[to] = weight;
		if (!isOriented) {
			++(*vertexDegrees)[to];
			(*(*adjacencyList)[to])[from] = weight;
		}
	}


	void removeEdge(int from, int to)  override
	{
		--edgesAmount;
		removeEdgeSimplex(adjacencyList, from, to);
		if (!isOriented)
			removeEdgeSimplex(adjacencyList, to, from);
	}


	int changeEdge(int from, int to, int newWeight)  override
	{
		const int oldWeight = (*(*adjacencyList)[from])[to];
		addEdge(from, to, newWeight);
		return oldWeight;
	}


	AdjMatrixGraphRepresentation* transformToAdjMatrix()  override;


	AdjListGraphRepresentation* transformToAdjList()  override { return this; }


	EdgeListGraphRepresentation* transformToListOfEdges()  override;


	void writeGraph(string fileName) override
	{
		ofstream file(fileName);
		file << "L " << adjacencyList->size() << endl;
		file << (isOriented ? "1 " : "0 ") << (isWeighted ? "1" : "0") << endl;

		writeList(file, adjacencyList, isWeighted);

		file.close();
	}


	int calc() {
		return 0;
	}


	AdjacencyList copyListWithDuplicates() {
		AdjacencyList adjList = new vector<map<int, int>*>(adjacencyList->size());
		for (size_t i = 0; i < adjacencyList->size(); ++i)
			(*adjList)[i] = new map<int, int>();

		for (size_t i = 0; i < adjacencyList->size(); ++i)
			for (auto it = adjacencyList->at(i)->begin(); it != adjacencyList->at(i)->end(); ++it) {
				(*(*adjList)[i])[it->first] = it->second;
				if (!isOriented)
					(*(*adjList)[it->first])[i] = it->second;
			}

		return adjList;
	}

	friend Graph;
};

class EdgeListGraphRepresentation : GraphRepresentation
{
	int vertexAmount;
	EdgeList edgeList = nullptr;
	EdgeList readList(istream& stream, int edgeAmount)
	{
		const EdgeList edgeList = new map<pair<int, int>, int>();
		vertexDegrees = new map<int, int>();
		for (size_t i = 0; i < vertexAmount; ++i)
			(*vertexDegrees)[i] = 0;
		for (size_t i = 0; i < edgeAmount; ++i)
		{
			string line;
			getline(stream, line);
			vector<string> edgeParameters = StringParser::split(line, ' ');
			const int weight = edgeParameters.size() > 2 ? stoi(edgeParameters[2]) : 1;
			const int from = stoi(edgeParameters[0]) - 1;
			const int to = stoi(edgeParameters[1]) - 1;
			(*edgeList)[make_pair(from, to)] = weight;
			++(*vertexDegrees)[from];
			if (!isOriented)
				++(*vertexDegrees)[to];
		}
		edgesAmount = edgeList->size();
		return edgeList;
	}
	static void writeList(ostream& stream, EdgeList edgeList, bool isWeighted)
	{
		for (auto it = edgeList->begin(); it != edgeList->end(); ++it)
		{
			stream << it->first.first + 1 << " " << it->first.second + 1;
			if (isWeighted)
				stream << " " << it->second;
			if (next(it) != edgeList->end())
				stream << endl;
		}
	}
	void copy(const EdgeListGraphRepresentation& graph) {
		this->isOriented = graph.isOriented;
		this->isWeighted = graph.isWeighted;
		this->vertexAmount = graph.vertexAmount;
		this->edgesAmount = graph.edgesAmount;
		this->vertexDegrees = new map<int, int>(*graph.vertexDegrees);
		this->edgeList = new map<pair<int, int>, int>(*(graph.edgeList));
	}
	int getEdgesAmount() {
		return edgeList->size();
	}
	DSU* buildDSU() {
		DSU* dsu = new DSU(vertexAmount);
		for (int i = 0; i < vertexAmount; ++i)
			dsu->makeSet(i);
		for (auto it = edgeList->begin(); it != edgeList->end(); ++it)
			dsu->unite(it->first.first, it->first.second);
		return dsu;
	}

public:
	EdgeListGraphRepresentation() = default;
	EdgeListGraphRepresentation(bool isOriented, bool isWeighted, int vertexAmount, int edgesAmount)
	{
		this->vertexDegrees = new map<int, int>();
		this->edgeList = new map<pair<int, int>, int>();
		this->isOriented = isOriented;
		this->isWeighted = isWeighted;
		this->vertexAmount = vertexAmount;
		this->edgesAmount = edgesAmount;
	}
	EdgeListGraphRepresentation(const EdgeListGraphRepresentation& graph) {
		copy(graph);
	}
	~EdgeListGraphRepresentation()
	{
		delete vertexDegrees;
		delete edgeList;
	}
	EdgeListGraphRepresentation& operator=(const EdgeListGraphRepresentation& graph) {
		copy(graph);
		return *this;
	}

	bool isAdjMatrixGraph() override { return false; }
	bool isAdjListGraph() override { return false; }
	bool isEdgeListGraph() override { return true; }

	void readGraph(istream& stream, vector<string> parameters) override
	{
		vertexAmount = stoi(parameters[1]);
		int edgeAmount = stoi(parameters[2]);

		isOriented = parameters[3] == "1";
		isWeighted = parameters[4] == "1";

		edgeList = readList(stream, edgeAmount);
	}


	void addEdge(int from, int to, int weight) override
	{
		++edgesAmount;
		(*edgeList)[make_pair(from, to)] = weight;
		++(*vertexDegrees)[from];
		++(*vertexDegrees)[to];
	}


	void removeEdge(int from, int to)  override
	{
		--edgesAmount;
		const pair<int, int> edge = make_pair(from, to);
		const auto it = edgeList->find(edge);
		edgeList->erase(it);
		--(*vertexDegrees)[from];
		--(*vertexDegrees)[to];
	}


	int changeEdge(int from, int to, int newWeight)  override
	{
		const pair<int, int> edge = make_pair(from, to);
		const int oldWeight = (*edgeList)[edge];
		(*edgeList)[edge] = newWeight;
		return oldWeight;
	}


	AdjMatrixGraphRepresentation* transformToAdjMatrix()  override;


	AdjListGraphRepresentation* transformToAdjList()  override;


	EdgeListGraphRepresentation* transformToListOfEdges()  override { return this; }


	void writeGraph(string fileName) override
	{
		ofstream file(fileName);
		file << "E " << vertexAmount << " " << edgeList->size() << endl;
		file << (isOriented ? "1 " : "0 ") << (isWeighted ? "1" : "0") << endl;

		writeList(file, edgeList, isWeighted);

		file.close();
	}

	friend Graph;

	int calc() {
		int result = 0;
		for (auto it = edgeList->begin(); it != edgeList->end(); ++it) {
			result += it->second;
		}
		return result;
	}
};

AdjListGraphRepresentation* AdjMatrixGraphRepresentation::transformToAdjList()
{
	AdjListGraphRepresentation* adjListGraph = new AdjListGraphRepresentation(adjacencyMatrix->size(), 0, isOriented, isWeighted);

	for (size_t i = 0; i < adjacencyMatrix->size(); ++i)
	{
		for (size_t j = 0; j < adjacencyMatrix->size(); ++j)
		{
			int weight = (*(*adjacencyMatrix)[i])[j];
			if (weight > 0)
				adjListGraph->addEdge(i, j, weight);
		}
	}

	return adjListGraph;
}

EdgeListGraphRepresentation* AdjMatrixGraphRepresentation::transformToListOfEdges()
{
	EdgeListGraphRepresentation* edgeListGraph = new EdgeListGraphRepresentation(isOriented, isWeighted, adjacencyMatrix->size(), 0);

	for (size_t i = 0; i < adjacencyMatrix->size(); ++i)
		for (size_t j = 0; j < adjacencyMatrix->size(); ++j)
		{
			int weight = (*(*adjacencyMatrix)[i])[j];
			if (weight > 0)
				edgeListGraph->addEdge(i, j, weight);
		}

	return edgeListGraph;
}

AdjMatrixGraphRepresentation* AdjListGraphRepresentation::transformToAdjMatrix()
{
	AdjMatrixGraphRepresentation* adjMatrixGraph = new AdjMatrixGraphRepresentation(adjacencyList->size(), 0, isOriented, isWeighted);

	for (size_t i = 0; i < adjacencyList->size(); ++i)
	{
		map<int, int>* currentVertexMap = (*adjacencyList)[i];
		for (auto edge = currentVertexMap->begin(); edge != currentVertexMap->end(); ++edge)
			adjMatrixGraph->addEdge(i, edge->first, edge->second);
	}

	return adjMatrixGraph;
}

EdgeListGraphRepresentation* AdjListGraphRepresentation::transformToListOfEdges()
{
	EdgeListGraphRepresentation* edgeListGraph = new EdgeListGraphRepresentation(isOriented, isWeighted, adjacencyList->size(), 0);

	for (size_t i = 0; i < adjacencyList->size(); ++i)
	{
		map<int, int>* currentVertexMap = (*adjacencyList)[i];
		for (auto edge = currentVertexMap->begin(); edge != currentVertexMap->end(); ++edge)
			edgeListGraph->addEdge(i, edge->first, edge->second);
	}

	return edgeListGraph;
}

AdjMatrixGraphRepresentation* EdgeListGraphRepresentation::transformToAdjMatrix()
{
	AdjMatrixGraphRepresentation* adjMatrixGraph = new AdjMatrixGraphRepresentation(vertexAmount, 0, isOriented, isWeighted);

	for (auto it = edgeList->begin(); it != edgeList->end(); ++it)
		adjMatrixGraph->addEdge(it->first.first, it->first.second, it->second);

	return adjMatrixGraph;
}

AdjListGraphRepresentation* EdgeListGraphRepresentation::transformToAdjList()
{
	AdjListGraphRepresentation* adjListGraph = new AdjListGraphRepresentation(vertexAmount, 0, isOriented, isWeighted);

	for (auto it = edgeList->begin(); it != edgeList->end(); ++it)
		adjListGraph->addEdge(it->first.first, it->first.second, it->second);

	return adjListGraph;
}

class Graph
{
	GraphRepresentation* graphRepresentation = nullptr;
	static vector<string> getGraphParameters(istream &stream)
	{
		vector<string> parameters;
		string line;
		for (size_t i = 0; i < 2; i++)
		{
			getline(stream, line);
			for (const string token : StringParser::split(line, ' '))
				parameters.push_back(token);
		}
		return parameters;
	}
	static GraphRepresentation* decideRepresentationType(string code)
	{
		if (code == "C")
			return (GraphRepresentation*) new AdjMatrixGraphRepresentation();
		if (code == "L")
			return (GraphRepresentation*) new AdjListGraphRepresentation();
		if (code == "E")
			return (GraphRepresentation*) new EdgeListGraphRepresentation();
		return nullptr;
	}
	void copy(const Graph& graph) {
		GraphRepresentation* newGraph = nullptr;
		if (graph.graphRepresentation->isAdjListGraph())
			newGraph = (AdjListGraphRepresentation*)graph.graphRepresentation;
		else if (graph.graphRepresentation->isAdjMatrixGraph())
			newGraph = (AdjMatrixGraphRepresentation*)graph.graphRepresentation;
		else if (graph.graphRepresentation->isEdgeListGraph())
			newGraph = (EdgeListGraphRepresentation*)graph.graphRepresentation;
		delete graphRepresentation;
		graphRepresentation = newGraph;
	}
	MarkedEdges initMarkedEdges(AdjacencyList adjList) {
		MarkedEdges bridges = new vector<map<int, bool>*>(adjList->size());
		for (int i = 0; i < bridges->size(); ++i) {
			(*bridges)[i] = new map<int, bool>();
			for (auto it = adjList->at(i)->begin(); it != adjList->at(i)->end(); ++it)
				(*(*bridges)[i])[it->first] = false;
		}
		return bridges;
	}
	MarkedEdges findBridges() {
		AdjListGraphRepresentation* graph = (AdjListGraphRepresentation*)graphRepresentation;

		MarkedEdges bridges = initMarkedEdges(graph->adjacencyList);
		vector<bool>* isVisited = new vector<bool>(graph->adjacencyList->size(), false);
		vector<int>* discoveryTime = new vector<int>(graph->adjacencyList->size());
		vector<int>* minTime = new vector<int>(graph->adjacencyList->size());
		vector<int>* parent = new vector<int>(graph->adjacencyList->size(), -1);

		int time = 0;
		for (int i = 0; i < graph->adjacencyList->size(); ++i)
			if (!isVisited->at(i))
				bridgeRecursiveStep(i, time, graph->adjacencyList, isVisited, discoveryTime, minTime, parent, bridges);

		return bridges;
	}
	void bridgeRecursiveStep(int u, int& time, AdjacencyList adjList, vector<bool>* isVisited, vector<int>* discoveryTime,
		vector<int>* minTime, vector<int>* parent, MarkedEdges bridges) {
		(*isVisited)[u] = true;
		(*discoveryTime)[u] = (*minTime)[u] = ++time;

		for (auto it = adjList->at(u)->begin(); it != adjList->at(u)->end(); ++it)
		{
			int v = it->first;
			if (!isVisited->at(v))
			{
				(*parent)[v] = u;
				bridgeRecursiveStep(v, time, adjList, isVisited, discoveryTime, minTime, parent, bridges);
				(*minTime)[u] = min((*minTime)[u], (*minTime)[v]);
				if ((*minTime)[v] > (*discoveryTime)[u]) {
					(*(*bridges)[u])[v] = true;
					//(*(*bridges)[v])[u] = true;
				}
			}
			else if (v != parent->at(u))
				(*minTime)[u] = min((*minTime)[u], (*discoveryTime)[v]);
		}
	}
	bool tryKuhn(int v, vector<bool> & used, AdjacencyList adjList, vector<int> & part) {
		if (used[v])  return false;
		used[v] = true;
		for (auto it = adjList->at(v)->begin(); it != adjList->at(v)->end(); ++it) {
			int to = it->first;
			if (part[to] == -1 || tryKuhn(part[to], used, adjList, part)) {
				part[to] = v;
				return true;
			}
		}
		return false;
	}

public:
	Graph() {}
	Graph(int n) {
		graphRepresentation = (GraphRepresentation*) new EdgeListGraphRepresentation(false, true, n, 0);
	}
	Graph(const Graph& graph) {
		copy(graph);
	}
	~Graph()
	{
		delete graphRepresentation;
	};
	Graph& operator=(const Graph& graph) {
		copy(graph);
		return *this;
	}

	void readGraph(string fileName)
	{
		ifstream file(fileName);
		const vector<string> parameters = getGraphParameters(file);
		graphRepresentation = decideRepresentationType(parameters[0]);
		graphRepresentation->readGraph(file, parameters);
		file.close();
	}
	void addEdge(int from, int to, int weight) const
	{
		graphRepresentation->addEdge(from, to, weight);
	}
	void removeEdge(int from, int to) const
	{
		graphRepresentation->removeEdge(from, to);
	}
	int changeEdge(int from, int to, int newWeight) const
	{
		return graphRepresentation->changeEdge(from, to, newWeight);
	}
	void transformToAdjList()
	{
		if (!graphRepresentation->isAdjListGraph()) {
			GraphRepresentation* adjListGraph = (GraphRepresentation*)graphRepresentation->transformToAdjList();
			delete graphRepresentation;
			graphRepresentation = adjListGraph;
		}
	}
	void transformToAdjMatrix()
	{
		if (!graphRepresentation->isAdjMatrixGraph()) {
			GraphRepresentation* adjMatrixGraph = (GraphRepresentation*)graphRepresentation->transformToAdjMatrix();
			delete graphRepresentation;
			graphRepresentation = adjMatrixGraph;
		}
	}
	void transformToListOfEdges()
	{
		if (!graphRepresentation->isEdgeListGraph()) {
			GraphRepresentation* edgeListGraph = (GraphRepresentation*)graphRepresentation->transformToListOfEdges();
			delete graphRepresentation;
			graphRepresentation = edgeListGraph;
		}
	}
	void writeGraph(string fileName) const
	{
		graphRepresentation->writeGraph(fileName);
	}

	Graph getSpaingTreePrima() {
		transformToAdjList();
		AdjacencyList adjList = ((AdjListGraphRepresentation*)graphRepresentation)->adjacencyList;

		priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
		int src = 0;
		vector<int> distances(adjList->size(), INT32_MAX);
		vector<int> parent(adjList->size(), -1);
		vector<bool> inMST(adjList->size(), false);

		pq.push(make_pair(0, src));
		distances[src] = 0;

		while (!pq.empty()) {
			int u = pq.top().second;
			pq.pop();
			inMST[u] = true;
			for (auto it = adjList->at(u)->begin(); it != adjList->at(u)->end(); ++it) {
				int v = it->first;
				int weight = it->second;
				if (inMST[v] == false && distances[v] > weight) {
					distances[v] = weight;
					pq.push(make_pair(distances[v], v));
					parent[v] = u;
				}
			}
		}

		EdgeListGraphRepresentation* newEdgeListGraph = new EdgeListGraphRepresentation(false, true, adjList->size(), 0);
		for (size_t i = 1; i < parent.size(); ++i)
			newEdgeListGraph->addEdge(parent[i], i, distances[i]);
		Graph* graph = new Graph();
		graph->graphRepresentation = newEdgeListGraph;
		return *graph;
	}

	Graph getSpaingTreeKruscal() {
		transformToListOfEdges();
		EdgeListGraphRepresentation* edgeListGraph = (EdgeListGraphRepresentation*)graphRepresentation;

		auto cmp = [](pair<pair<int, int>, int> a, pair<pair<int, int>, int> b) {
			return a.second < b.second;
		};
		auto edgeVector = vector<pair<pair<int, int>, int>>(edgeListGraph->edgeList->begin(), edgeListGraph->edgeList->end());
		sort(edgeVector.begin(), edgeVector.end(), cmp);

		DSU dsu = DSU(edgeListGraph->vertexAmount);
		for (size_t i = 0; i < edgeListGraph->vertexAmount; ++i)
			dsu.makeSet(i);

		EdgeListGraphRepresentation* newEdgeListGraph = new EdgeListGraphRepresentation(false, true, edgeListGraph->vertexAmount, 0);
		for (size_t i = 0; i < edgeVector.size(); i++)
		{
			int from = edgeVector[i].first.first;
			int to = edgeVector[i].first.second;
			if (dsu.find(from) != dsu.find(to)) {
				newEdgeListGraph->addEdge(from, to, edgeVector[i].second);
				dsu.unite(from, to);
			}
		}
		Graph* graph = new Graph();
		graph->graphRepresentation = newEdgeListGraph;
		return *graph;
	}

	Graph getSpaingTreeBoruvka() {
		transformToListOfEdges();
		EdgeListGraphRepresentation* edgeListGraph = (EdgeListGraphRepresentation*)graphRepresentation;
		EdgeList edgeList = edgeListGraph->edgeList;
		int vertexAmount = edgeListGraph->vertexAmount;
		auto edgeVector = vector<pair<pair<int, int>, int>>(edgeListGraph->edgeList->begin(), edgeListGraph->edgeList->end());

		DSU dsu = DSU(vertexAmount);
		for (size_t i = 0; i < edgeListGraph->vertexAmount; ++i)
			dsu.makeSet(i);

		EdgeListGraphRepresentation* newEdgeListGraph = new EdgeListGraphRepresentation(false, true, vertexAmount, 0);
		while (newEdgeListGraph->edgesAmount < vertexAmount - 1) {
			auto minEdges = map<int, int>();
			for (int i = 0; i < vertexAmount; ++i)
				minEdges[i] = -1;
			for (int i = 0; i < edgeVector.size(); ++i)
			{
				auto edge = edgeVector[i];
				int from = edge.first.first;
				int to = edge.first.second;
				int weight = edge.second;
				int fromComponent = dsu.find(from);
				int toComponent = dsu.find(to);
				if (fromComponent != toComponent) {
					if (minEdges[fromComponent] == -1 || edgeVector[minEdges[fromComponent]].second > weight)
						minEdges[fromComponent] = i;
					if (minEdges[toComponent] == -1 || edgeVector[minEdges[toComponent]].second > weight)
						minEdges[toComponent] = i;
				}
			}
			for (int i = 0; i < minEdges.size(); i++) {
				if (minEdges[i] != -1) {
					pair<pair<int, int>, int> edge = edgeVector[minEdges[i]];
					dsu.unite(edge.first.first, edge.first.second);
					newEdgeListGraph->addEdge(edge.first.first, edge.first.second, edge.second);
				}
			}
		}

		Graph* graph = new Graph();
		graph->graphRepresentation = newEdgeListGraph;
		return *graph;
	}

	int checkEuler(bool &circleExist) {
		bool connectedComponentsCheck = graphRepresentation->connectedComponentsCheck();
		if (!connectedComponentsCheck) {
			circleExist = false;
			return 0;
		}
		return graphRepresentation->getBeginVertex(circleExist) + 1;
	}

	vector<int> getEuleranTourFleri() {
		bool circleExists;
		int currentVertex = checkEuler(circleExists) - 1;
		if (!circleExists && currentVertex == -1)
			return vector<int>();

		transformToAdjList();
		AdjacencyList adjList = ((AdjListGraphRepresentation*)graphRepresentation)->copyListWithDuplicates();
		MarkedEdges bridges = findBridges();
		MarkedEdges isTraversed = initMarkedEdges(adjList);
		vector<int> path = vector<int>();

		path.push_back(currentVertex);
		for (int i = 0; i < graphRepresentation->edgesAmount; ++i) {
			int nextVertex;
			bool onlyBridges = true;
			for (auto it = adjList->at(currentVertex)->begin(); it != adjList->at(currentVertex)->end(); ++it) {
				if (!(*(*isTraversed)[currentVertex])[it->first]) {
					if (!(*(*bridges)[currentVertex])[it->first]) {
						onlyBridges = false;
						nextVertex = it->first;
					}
					else if (onlyBridges)
						nextVertex = it->first;
				}
			}
			path.push_back(nextVertex);
			(*(*isTraversed)[currentVertex])[nextVertex] = true;
			(*(*isTraversed)[nextVertex])[currentVertex] = true;
			currentVertex = nextVertex;
		}

		return path;
	}

	vector<int> getEuleranTourEffective() {
		bool circleExists;
		int currentVertex = checkEuler(circleExists) - 1;
		if (!circleExists && currentVertex == -1)
			return vector<int>();

		transformToAdjList();
		AdjacencyList adjList = ((AdjListGraphRepresentation*)graphRepresentation)->copyListWithDuplicates();
		MarkedEdges isTraversed = initMarkedEdges(adjList);
		vector<int> path = vector<int>();

		stack<int> s = stack<int>();
		s.push(currentVertex);
		while (!s.empty())
		{
			int w = s.top();
			for (auto it = adjList->at(w)->begin(); it != adjList->at(w)->end(); ++it) {
				if (!isTraversed->at(w)->at(it->first)) {
					s.push(it->first);
					(*isTraversed->at(w))[it->first] = true;
					(*isTraversed->at(it->first))[w] = true;
					break;
				}
			}
			if (w == s.top()) {
				s.pop();
				path.push_back(w);
			}
		}

		return path;
	}

	int checkBipart(vector<char> &marks) {
		transformToAdjList();
		AdjacencyList adjList = ((AdjListGraphRepresentation*)graphRepresentation)->copyListWithDuplicates();

		marks = vector<char>(adjList->size(), -1);

		for (int i = 0; i < adjList->size(); ++i) {
			if (marks[i] == -1) {
				queue<int> q = queue<int>();
				q.push(i);
				marks[i] = 'A';
				while (!q.empty()) {
					int v = q.front();
					q.pop();
					for (auto it = adjList->at(v)->begin(); it != adjList->at(v)->end(); ++it) {
						int to = it->first;
						if (marks[to] == -1) {
							if (marks[v] == 'A')
								marks[to] = 'B';
							else
								marks[to] = 'A';
							q.push(to);
						}
						else if (marks[to] == marks[v]) {
							return 0;
						}
					}
				}
			}
		}

		return 1;
	}

	vector<pair<int, int>> getMaximumMatchingBipart() {
		vector<char> marks = vector<char>();
		int isBipart = checkBipart(marks);
		if (isBipart) {
			vector<pair<int, int>> result = vector<pair<int, int>>();
			AdjacencyList adjList = ((AdjListGraphRepresentation*)graphRepresentation)->copyListWithDuplicates();
			vector<int> part = vector<int>(adjList->size(), -1);

			for (size_t i = 0; i < adjList->size(); i++)
				if (marks[i] == 'A') {
					vector<bool> used = vector<bool>(adjList->size(), false);
					tryKuhn(i, used, adjList, part);
				}
			for (size_t i = 0; i < adjList->size(); i++)
				if (marks[i] == 'B' && part[i] != -1)
					result.push_back(make_pair(part[i] + 1, i + 1));
			return result;
		}
		return vector<pair<int, int>>();
	}

	int calc() {
		return graphRepresentation->calc();
	}
};