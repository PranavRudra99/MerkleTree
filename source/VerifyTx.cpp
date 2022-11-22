#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <string>
#include <stdlib.h>
#include "sha256.h"
#include <bitset>


using namespace std;

bool fileExists(string fileName);
bool isPrefixTx(string transaction);
bool areEqual(char* buffer, string hash);
bool isValidTransaction(int transactionId, int n);
bool validRoot(string hash, string rootFile);
bool validBranch(string** tree, int transactionId, string branchFile, int treeSize);
int getExponentValue(int n);
int getFileSize(std::string fileName);
int getMerkleTreeHeight(string branchFile);
char getHexOfBinary(string bits);
string sha256(string input);
string getHexValue(char char1, char char2);
string* getTreeLeaves(int n);
string* constructBaseLevel(string* currentVals, int n);
string* constructUpperLevel(string* currentLevel, int n);
string** constructTree(int n, string transaction);

int main(int argc, char** argv){
  string rootFile, branchFile;
  string transaction = "";
  if(argc > 3){
    rootFile = argv[1];
    branchFile = argv[2];
    transaction = argv[3];
    if(fileExists(rootFile) && fileExists(branchFile)){
      if(!isPrefixTx(transaction)){
        cout << "no" << endl;
        return 1;
      }
      int transactionId = atoi(transaction.substr(2).c_str()) - 1;
      int treeSize = getMerkleTreeHeight(branchFile) - 1;
      if(!isValidTransaction(transactionId, treeSize)){
        cout << "no" << endl;
        return 1;
      }
      string root = "";
      string** tree = constructTree(treeSize, transaction);
      root = tree[treeSize][0];
      bool rootSatisfied = false;
      bool branchSatisfied = false;
      if(validBranch(tree, transactionId, branchFile, treeSize)){
        branchSatisfied = true;
      }
      if(validRoot(root, rootFile)){
        rootSatisfied = true;
      }
      if(rootSatisfied && branchSatisfied){
        cout << "yes" << endl;
      }else{
        cout << "no" << endl;
      }
      return 1;
    }
  }
  cout << "Enter valid Input!!" << endl;
  return -1;
}

bool isPrefixTx(string transaction){
  string prefix = transaction.substr(0,2);
  if(prefix == "Tx"){
    return true;
  }
  return false;
}

bool isValidTransaction(int transactionId, int n){
  int exponent = getExponentValue(n);
  if(transactionId < 0 || transactionId > exponent - 1){
    return false;
  }
  return true;
}

int getExponentValue(int n){
  return (int)pow(2, n);
}

string** constructTree(int n, string transaction){
  int exponent = getExponentValue(n);
  string* treeLeaves = getTreeLeaves(exponent);
  string** tree = new string*[n + 1];
  tree[0] = constructBaseLevel(treeLeaves, exponent);
  for(int i = 1; i <= n; i++){
    tree[i] = constructUpperLevel(tree[i-1], exponent);
    exponent = exponent/2;
  }
  return tree;
}

string* constructUpperLevel(string* currentLevel, int n){
  int upperLevelSize = n/2;
  string* upperLevel = new string[upperLevelSize];
  for(int i = 0; i < n; i++){
    string hash1 = currentLevel[i];
    string hash2 = currentLevel[i + 1];
    string input = hash1 + hash2;
    upperLevel[i/2] = sha256(input);
    i++;
  }
  return upperLevel;
}

string* constructBaseLevel(string* currentVals, int n){
  string* currentLevel = new string[n];
  for(int i = 0; i < n; i++){
    string hash = sha256(currentVals[i]);
    currentLevel[i] = hash;
  }
  return currentLevel;
}

string* getTreeLeaves(int n){
  string transaction = "Tx";
  string* leaves = new string[n];
  for(int i = 1; i <= n; i++){
    string currentTransaction = transaction + to_string(i);
    leaves[i - 1] = currentTransaction;
  }
  return leaves;
}

bool validRoot(string hash, string rootFile){
  std::ifstream root(rootFile, std::ios::binary);
  int buffSize = getFileSize(rootFile);
  char* buffer = new char[buffSize];
  size_t actual = root.readsome(buffer, buffSize);
  root.close();
  if(actual == hash.length()/2){
    return areEqual(buffer, hash);
  }
  else{
    return false;
  }
  return true;
}

bool validBranch(string** tree, int transactionId, string branchFile, int treeSize){
  std::ifstream branch(branchFile, std::ios::binary);
  int buffSize = 32;
  char* buffer = new char[buffSize];
  bool valid = true;
  for(int i = 0; i <= treeSize; i++){
    branch.seekg(33*i);
    size_t actual = branch.readsome(buffer, buffSize);
    std::string hash = tree[i][transactionId];
    valid = valid && areEqual(buffer, hash);
    transactionId = transactionId/2;
  }
  return valid;
}

bool areEqual(char* buffer, string hash){
    int j = 0;
    string hexHash = "";
    bool equals = true;
    for(int i = 0; i < hash.length(); i+=2){
      std::string nextbyte = hash.substr(i, 2);
      std::string bits = std::bitset<8>(buffer[j]).to_string();
      string hex = getHexValue(getHexOfBinary(bits.substr(0, 4)), getHexOfBinary(bits.substr(4, 4)));
      hexHash = hexHash + hex;
      if(nextbyte != hex){
        equals = false;
      }
      j++;
    }
    return equals;
}

string getHexValue(char char1, char char2){
  string hex = "";
  hex = hex + char1;
  hex = hex + char2;
  return hex;
}

char getHexOfBinary(string bits){
  if(bits == "0000") return '0';
  if(bits == "0001") return '1';
  if(bits == "0010") return '2';
  if(bits == "0011") return '3';
  if(bits == "0100") return '4';
  if(bits == "0101") return '5';
  if(bits == "0110") return '6';
  if(bits == "0111") return '7';
  if(bits == "1000") return '8';
  if(bits == "1001") return '9';
  if(bits == "1010") return 'a';
  if(bits == "1011") return 'b';
  if(bits == "1100") return 'c';
  if(bits == "1101") return 'd';
  if(bits == "1110") return 'e';
  if(bits == "1111") return 'f';
  return 'z';
}

bool fileExists(string fileName){
  ifstream f(fileName.c_str());
  return f.good();
}

int getMerkleTreeHeight(string branchFile){
  int hashSize = 33;
  return getFileSize(branchFile)/hashSize;
}

int getFileSize(std::string fileName){
  return std::filesystem::file_size(fileName);
}
