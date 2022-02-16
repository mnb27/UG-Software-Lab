// AMAN BILAIYA 2018CSB1069 //
// Grid partitioning of network graph //

#include<bits/stdc++.h>
using namespace std;
#define nodeCoor pair<double, double>
#define ppi pair<int, int>

// file modes
#define writeData std::ios::out
#define readData std::ios::in
#define appendData std::ios::out | std::ios::app

// Graph Data markers
string edgeWithinCell = "##";
string boundaryNodes = "**";
string overflowMarker = "??";
string boundaryEgdes = "%%";

int cellSizeK = 15000; // Each cell is of fixed size K*K
int entries = 15000; // B - number of entries (distinct nodes or edges) allowed in each disk block

// a map from node id → x and y coordinate
map<int, nodeCoor> nodeCoorMap;

// a map between the cell ids and the file names corresponding to their disk blocks
map<ppi, set<string>> CellIDFilenamesMap; // cell(x,y) --> disk blocks.txt

vector<vector<int>> adj; // adjacency list for storing graph network
map<ppi, vector<double>> edgeWt; // mutiple egdes {a->b} maps to edge weight
double X_MIN = DBL_MAX, X_MAX = DBL_MIN, Y_MIN = DBL_MAX, Y_MAX = DBL_MIN;

// Determine the appropriate cell id given an x and y coordinates
ppi getCell_id_fromXY(double x, double y) {
  int i = (x - X_MIN) / cellSizeK;
  int j = (y - Y_MIN) / cellSizeK;
  return make_pair(i, j);
}

ppi getCell_id_fromNodeID(int nodeID) {
  // Returns id of cell corresponding to a node (nodeID)
  pair<double, double> coor = nodeCoorMap[nodeID];
  return getCell_id_fromXY(coor.first, coor.second);
}

string getFileName(int x, int y, int num) {
  string filename =  "Cell(";
  filename += to_string(x);
  filename.push_back(',');
  filename += to_string(y);
  filename.push_back(')');
  filename.push_back('_');
  if (num == 0) filename += "Block";
  else filename += "OverflowBlock";
  if (num > 0) filename += to_string(num);
  filename += ".txt";
  return filename;
}

// Utility function to handle file write for conditions of overflow
void overflowHandler(int &entriesCount, int &fileIndex, std::ofstream &file, int cellX, int cellY) {
  if (entriesCount < entries) return; 
  entriesCount = 0;
  string overflowFileName = getFileName(cellX, cellY, fileIndex + 1);
  file << overflowMarker << " " << overflowFileName << endl;
  file.close();

  file.open(overflowFileName, appendData);
  file << overflowMarker << " " << getFileName(cellX, cellY, fileIndex) << endl;
  fileIndex++;
  CellIDFilenamesMap[make_pair(cellX, cellY)].insert(overflowFileName);
  return;
}

// write portion of the graph inside each cell C with id(x,y) in single disk block (along with overflow blocks as needed)
void makeDiskBlocks(int x, int y, vector<int> nodes) {
  string filename = getFileName(x, y, 0);
  std::cout << "Filename - " << filename << endl;
  ofstream fileHandle(filename, appendData);
  if ((int)nodes.size() == 0) return;

  int entriesCount = 0;
  int fileIndex = 0;
  sort(nodes.begin(), nodes.end());

  // NODES INSIDE CELL X entriesCount
  for (int id : nodes) {
    fileHandle << id << " " << nodeCoorMap[id].first << " " << nodeCoorMap[id].second << endl;
    entriesCount++;
    overflowHandler(entriesCount, fileIndex, fileHandle, x, y);
  }

  // EDGES INSIDE CELL X Y
  fileHandle << edgeWithinCell << endl;

  unordered_set<int> nodesSet;
  for (int id : nodes) nodesSet.insert(id);

  for (int id : nodes) {
    for (int child : adj[id]) {
      if (nodesSet.count(child)) {
        for(double weight : edgeWt[{id, child}]) {
            fileHandle << id << " " << child << " " << weight << endl;
            entriesCount++;
            overflowHandler(entriesCount, fileIndex, fileHandle, x, y);
        }
      }
    }
  }

  // NODES ON BOUNDARY
  fileHandle << boundaryNodes << endl;
  set<int> out_of_bound;
  for (int id : nodes) {
    for (int child : adj[id]) {
      if (!nodesSet.count(child)) {
        out_of_bound.insert(child);
        fileHandle << child << " " << nodeCoorMap[child].first << " " << nodeCoorMap[child].second << endl;
        entriesCount++;
        overflowHandler(entriesCount, fileIndex, fileHandle, x, y);
      }
    }
  }

  // BOUNDARY EGDES
  fileHandle << boundaryEgdes << endl;
  // In to out directed edges
  for (int id : nodes) {
    for (int child : adj[id]) {
      if (out_of_bound.count(child)) {
        for(double weight : edgeWt[{id, child}]) {
            fileHandle << id << " " << child << " " << weight << endl;
            entriesCount++;
            overflowHandler(entriesCount, fileIndex, fileHandle, x, y);
        }
      }
    }
  }
  // out to in directed edges
  for (int A : out_of_bound) {
    for (int B : adj[A]) {
      for(double weight : edgeWt[{A,B}]) {
        fileHandle << A << " " << B << " " << weight << endl;
        entriesCount++;
        overflowHandler(entriesCount, fileIndex, fileHandle, x, y);
      }
    }
  }
  fileHandle.close();
}

void getDiskFileNames() {
  // A function to print all generated files for quick inspection
  for (auto mp : CellIDFilenamesMap) {
    std::cout << "In Cell (" << mp.first.first << "," << mp.first.second << ") --> ";
    for (string names : mp.second) {
      std::cout << names << "   ";
    }
    std::cout << endl;
  }
}

// function which would print the disk block (and its associated overflow block) contents of any given node id in "visualizer.txt"
void visualizer(int nodeID) {
  ppi cell_id = getCell_id_fromNodeID(nodeID);
  int x = cell_id.first, y = cell_id.second;
  std::cout << "Visualizing - node " << nodeID << " in cell (" << x << "," << y << ")" << endl;
  vector<string> fileNames;
  for (string name : CellIDFilenamesMap[ {x, y}]) {
    fileNames.push_back(name);
  }
  sort(fileNames.begin(), fileNames.end());
  cout<<"Related disk block files for given node are - ";
  for (auto x : fileNames) std::cout << x << " ";

  ofstream visualizerFile("Visualizer.txt", appendData);
  for (string file : fileNames) {
    ifstream fileHandle(file);
    string line;
    while (getline(fileHandle, line)) {
      visualizerFile << line << endl;
      line = "";
    }

  }
}

vector<string> stringTokenizer(string &str) {
  stringstream lineStream(str);
  vector<string> res;
  string part;
  while (getline(lineStream, part, ' '))
    res.push_back(part);
  return res;
}

int main() {

  char option;
  cout<<"Want to give custom input(y) or continue with default values(n) ? --> ";
  cin>>option;
  cout<<endl;
  int cellSize, entriesLimit;
  if(option=='y') {
     cout<< "Input cell size (k) : ";     cin>>cellSize;      cout<<endl;
     cout << "Input entries limit for disk blocks (B) : "; cin>>entriesLimit; cout<<endl;
     cellSizeK = cellSize;
     entries = entriesLimit;
  }

  ifstream edgeInfo("Dataset/edges.txt");
  ifstream nodesInfo("Dataset/nodes.txt");

  string line;
  vector<string> split;

  // Computing X_MIN, X_MAX, Y_MIN, Y_MAX from the dataset
  while (getline(nodesInfo, line)) {
    split = stringTokenizer(line);
    int nodeID = stoi(split[0]);
    double nodeX = stod(split[1]);
    double nodeY = stod(split[2]);
    if (X_MIN > nodeX) X_MIN = nodeX;
    if (X_MAX < nodeX) X_MAX = nodeX;
    if (Y_MIN > nodeY) Y_MIN = nodeY;
    if (Y_MAX < nodeY) Y_MAX = nodeY;
    // creating node coordinates map
    nodeCoorMap[nodeID] = make_pair(nodeX, nodeY);
    split.clear();
    line = "";
  }

  // Adusting X_MAX and Y_MAX boundaries so width,height become multiple of cellSizeK(k)
  int width = (X_MAX - X_MIN);
  int height = (Y_MAX - Y_MIN);
  X_MAX += (cellSizeK - (width % cellSizeK)) % cellSizeK;
  Y_MAX += (cellSizeK - (height % cellSizeK)) % cellSizeK;


  std::cout << "GRID : ";
  std::cout.precision(20);
  std::cout << "X_MIN: " << X_MIN << " " << "X_MAX: " << X_MAX << " " << "Y_MIN :" << Y_MIN << " " << "Y_MAX" << Y_MAX << endl;

  int nodesCount = nodeCoorMap.size();
  adj.resize(nodesCount);

  // Creating edge map
  while (getline(edgeInfo, line)) {
    split = stringTokenizer(line);
    int nodeA = stoi(split[0]);
    int nodeB = stoi(split[1]);
    double weight = stod(split[2]);
    adj[nodeA].push_back(nodeB);
    edgeWt[ {nodeA, nodeB}].push_back(weight);
  }

  int x = 0, y = 0;
  map<ppi, vector<int>> node_data;

  for (auto idd : nodeCoorMap) {
    int ID = idd.first;
    ppi cell_id = getCell_id_fromNodeID(ID);
    node_data[ {cell_id.first, cell_id.second}].push_back(ID);
  }

  int count = 0;
  while (true) {
    if (y * cellSizeK >= Y_MAX) break;
    if (x * cellSizeK >= X_MAX) {
      x = 0;
      y++;
      continue;
    }
    if (node_data.count({x, y})) {
      string filename = getFileName(x, y, 0);
      CellIDFilenamesMap[ {x, y}].clear();
      CellIDFilenamesMap[ {x, y}].insert(filename);
      // cout<< "block - " <<x <<" "<<y <<" " << node_data[{x,y}].size() <<endl;
      makeDiskBlocks(x, y, node_data[ {x, y}]);
    }
    x++;
    count++;
  }
  std::cout<<count<<endl;
  // getDiskFileNames();
  visualizer(700);

  edgeInfo.close();
  nodesInfo.close();
}