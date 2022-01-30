// AMAN BILAIYA 2018CSB1069 //
// Implementation of External sorting with the N-way merge algorithm (refer readme.txt for additional details) //

#include<iostream>
#include<vector>
#include<string>
#include <sstream>
#include<fstream>
using namespace std;

// CHANGE THE BELOW 4 PARAMETERS ACCORDINGLY
#define records 1000 // total records of department stores
#define B 45 // each disk block can store only B records
#define M 4 // you have space for storing only M “simulated disk blocks” in the main memory
#define N 3 // Order of merge, set order in range [2,3,..M-1]

int runs_counter = 0;

// Below function generates synthetic table containing sales records of department stores
string generateSyntheticTable(int saleAmountRange,int categoryRange, int nameLength) {
    int transactionID = 1; // Unique transaction ID
    int transactionSaleAmount; // Transaction sale amount
    char customerName[nameLength+1]; // Customer Name
    int Itemcategory = -1; // category of item

    char c = 'A';
    char nameCharacters[26];
    for(int i=0;i<26;i++)
        nameCharacters[i] = c + i;

    ofstream fileHandle;
    string tableName = "synTable.txt";
    fileHandle.open(tableName);
    srand(0);
    
    while(transactionID != records+1) {
        transactionSaleAmount = rand()%saleAmountRange + 1;
        for(int j=0;j<nameLength;j++) {
            int idx = rand()%26;
            customerName[j] = nameCharacters[idx];
        }
        customerName[nameLength] = '\0';
        Itemcategory = rand()%categoryRange + 1;
        fileHandle <<transactionID <<" "<<transactionSaleAmount<<" "<<customerName<<" "<<Itemcategory<< endl;
        transactionID++;
    }
    // fileHandle << "First Disk Block - 1.txt" <<endl;
    fileHandle.close();
    return tableName;
}

void generateDiskBlocks(string synDataTable) {
    int totalDiskBlocks = records/B + ((records%B)!=0);
    int eachFilerecordCnt = B;

    ifstream inFile(synDataTable);
    string line;
    int fileNo = 1;
    int counter = 0;

    int endofLine = records;
    int lineIdx = 0; 

    ofstream outFile[totalDiskBlocks];
    for (int i=0;i<totalDiskBlocks ;i++) {
        outFile[i].open("blk" + to_string(i+1) + ".txt", std::ios::out|std::ios::app);
    }

    while(lineIdx < endofLine and getline(inFile, line)) {
        outFile[fileNo-1] << line << endl;
        counter++;
        if(counter == eachFilerecordCnt) {
            // if(fileNo!=totalDiskBlocks) outFile << "Next Disk Block - " + to_string(fileNo+1) + ".txt" << endl;
            fileNo++;
            counter=0;
        }
        lineIdx++;
    }
    for (int i=0;i<totalDiskBlocks ;i++) {
        outFile[i].close();
    }
    inFile.close();
}

vector<string> stringTokenizer(string &str) {
    stringstream strStream(str);
    vector<string> res;
    string inter;
    while(getline(strStream, inter, ' '))
        res.push_back(inter);
    return res;
}

vector<int> getNodeInfo(int i) {
    int leftChild = 1 + 2*i;
    int rightChild = 2 + 2*i;
    int parent = (i-1)/2;
    return {parent, leftChild, rightChild};
}

struct pairMap {
    int keyA, valA, valB;
    string keyB;
};

struct PriorityQueue {
private:
    vector<pairMap> A;

    void heapifyDown(int i) {
        vector<int> nodeInfo = getNodeInfo(i);
        int left = nodeInfo[1];
        int right = nodeInfo[2];
 
        int smllest = i;
        if (left < size() and A[left].keyA < A[i].keyA) smllest = left;
        if (right < size() and A[right].keyA < A[smllest].keyA) smllest = right;
 
        if (smllest != i) {
            swap(A[i], A[smllest]);
            heapifyDown(smllest);
        }
    }
 
    void heapifyUp(int i) {
        if (i and A[(i-1)/2].keyA > A[i].keyA) {
            swap(A[i], A[(i-1)/2]);
            heapifyUp((i-1)/2);
        }
    }
 
public:
    int size() {
        return A.size();
    }
    bool empty() {
        return size() == 0;
    }
    void pushInHeap(pairMap key) {
        A.push_back(key);
        int index = size() - 1;
        heapifyUp(index);
    }

    void popFromHeap() {
        int index = size() - 1;
        A[0] = A[index];
        A.pop_back();
        heapifyDown(0);
    }
 
    pairMap top() {
        return A.at(0);
    }
};

void mergeKArrays(vector<string> fileNames) {
    for(auto x:fileNames) cout<<x<<"---";
    cout<<runs_counter;
    cout<<endl;
    int n = fileNames.size();
    vector<vector<pair<int,string>>> arr(n);
    string line;
    vector<string> tokens;
    int i = 0;
    for(string file:fileNames) {
        ifstream rd(file);
        while(getline(rd,line)) {
            vector<string> tokens = stringTokenizer(line);
            arr[i].push_back({stoi(tokens[1]), line});
            tokens.clear();
            line = "";
        }
        i++;
    }

    ofstream output("runs"+to_string(runs_counter)+".txt");
    PriorityQueue pq;
    struct pairMap txn1;
    for (int i = 0; i < n; i++) {
        txn1.keyA = arr[i][0].first, txn1.keyB = arr[i][0].second;
        txn1.valA = i, txn1.valB = 0;
        pq.pushInHeap(txn1);
    }
 
    while (pq.empty() == false) {
        pairMap curr = pq.top();
        pq.popFromHeap();
        int i = curr.valA;
        int j = curr.valB;
 
        output << curr.keyB << endl;
        if (j + 1 < arr[i].size()) {
            txn1.keyA = arr[i][j+1].first;
            txn1.keyB = arr[i][j+1].second;
            txn1.valA = i;
            txn1.valB = j+1;
            pq.pushInHeap(txn1);
        }
    }
    runs_counter++;
    output.close();
}

struct txnRow {
    int ID, AMOUNT, CATEGORY;
    string NAME;
};

////////////// IMPLEMENTATION OF MIN HEAP //////////////
struct MinHeap
{
private:
    vector<txnRow> A; 
    void heapifyDown(int i)
    {
        vector<int> nodeInfo = getNodeInfo(i);
        int left = nodeInfo[1];
        int right = nodeInfo[2];
 
        int smllest = i;
        if (left < size() and A[left].AMOUNT < A[i].AMOUNT) smllest = left;
        if (right < size() and A[right].AMOUNT < A[smllest].AMOUNT) smllest = right;
        
        if (smllest != i) {
            swap(A[i], A[smllest]);
            heapifyDown(smllest);
        }
    }

    void heapifyUp(int i) {
        if (i and A[(i-1)/2].AMOUNT > A[i].AMOUNT) {
            swap(A[i], A[(i-1)/2]);
            heapifyUp((i-1)/2);
        }
    }
 
public:
    int size() {
        return A.size();
    }
    bool empty() {
        return size() == 0;
    }
    void pushInHeap(txnRow key) {
        A.push_back(key);
        int index = size() - 1;
        heapifyUp(index);
    }

    void popFromHeap() {
        int index = size() - 1;
        A[0] = A[index];
        A.pop_back();
        heapifyDown(0);
    }
 
    txnRow top() {
        return A.at(0);
    }
};


/////////////////// SORT FILE USING HEAP SORT - O(nlogn) /////////////////////////
void sortRuns(string filename) {
    ifstream fh(filename);
    string line;
    vector<string> v;
    while(getline(fh,line)) {
        v.push_back(line);
    }
    vector<vector<string>> file;
    for(string line:v) {
        vector <string> tokens;
        stringstream strStream(line);
        string inter;
        while(getline(strStream, inter, ' ')) {
            tokens.push_back(inter);
        }
        file.push_back(tokens);
    }

    MinHeap pq;
    struct txnRow txn2;
    for(auto x:file) {
        txn2.ID = stoi(x[0]), txn2.AMOUNT = stoi(x[1]), txn2.NAME = x[2], txn2.CATEGORY = stoi(x[3]);
        pq.pushInHeap(txn2);
    }

    ofstream FH (filename);
    int sz = pq.size();
    while(sz--) {
        txnRow txnn = pq.top();
        pq.popFromHeap();
        FH << txnn.ID <<" " <<txnn.AMOUNT<<" " << txnn.NAME <<" "<< txnn.CATEGORY<<" " <<endl;
    }
    fh.close();
    FH.close();
}

int main() {

    string dataTable = generateSyntheticTable(60000, 1500, 3);
    generateDiskBlocks(dataTable);

    int totalDiskBlocks = records/B + ((records%B)!=0);

    int runNo = 1;
    for(int i=0;i<totalDiskBlocks;i+=M) {
        ofstream outFile("runs"+ to_string(runNo) + ".txt");
        string line;
        for(int j=i;j<min(i+M,totalDiskBlocks);j++) {
            ifstream opFile("blk" + to_string(j+1) + ".txt");
            while(getline(opFile, line)) {
                outFile << line << endl;
            }
        }
        sortRuns("runs"+ to_string(runNo)+".txt");
        runNo++;
    }
    runs_counter = runNo;
    
    int runs = runNo-1;
    vector<string> runFiles;
    for(int i=1;i<=runs;i++) {
        runFiles.push_back("runs"+ to_string(i)+".txt");
    }
    if(runs==1) exit(0);

    if(runs < M) {   
        cout<<"CASE 1 : runs < M ----> Single N-way merge"<<endl;
        mergeKArrays(runFiles);
    }
    else {
        cout<<"CASE 2 : runs >= M ----> Make merge passes and in each pass sort N-blocks"<<endl;
        int counter = 0, idx=0;
        int var = 1;
        while(runs > 1) {
            cout<<runs<<"-";
            int order = N;
            vector<string> aux;

            while(idx < runs and order > 0) {
                aux.push_back("runs"+to_string(var++)+".txt");
                idx++;
                order--;
            }
            counter++;
            mergeKArrays(aux); 
            if(idx == runs) {
                runs = counter;
                counter = 0;
                idx = 0;
            }
        }
    }
    cout<<"Final sorted department transaction output data is in: runs" <<runs_counter-1<<".txt"<<endl;

}