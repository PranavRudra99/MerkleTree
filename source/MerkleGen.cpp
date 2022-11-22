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

using namespace std;

bool isPrefixTx(string transaction);
bool isValidInput(string transaction, int n);
int getExponentValue(int n);
string sha256(string input);
string* getTreeLeaves(int n);
string* constructBaseLevel(string* currentVals, int n);
string* constructUpperLevel(string* currentLevel, int n);
void constructTree(int n, string transaction);
void createRoot(string root);
void createEmptyTextFile(string fileName);
void createBranch(string** tree, string transaction, int n);

int main(int argc, char** argv){
  int n = 0;
  string transaction = "";
  if(argc > 2){
    n = atoi(argv[1]);
    transaction = argv[2];
    if(!isPrefixTx(transaction) || !isValidInput(transaction, n)){
      cout << "Enter valid input" << endl;
      return -1;
    }
    constructTree(n, transaction);
  }
  else{
    cout << "Enter valid input" << endl;
    return -1;
  }
}

bool isPrefixTx(string transaction){
  string prefix = transaction.substr(0,2);
  if(prefix == "Tx"){
    return true;
  }
  return false;
}

bool isValidInput(string transaction, int n){
  int transactionId = atoi(transaction.substr(2).c_str()) - 1;
  if(n < 1){
    return false;
  }
  int exponent = getExponentValue(n);
  if(transactionId < 0 || transactionId > exponent - 1){
    return false;
  }
  return true;
}

void constructTree(int n, string transaction){
  int exponent = getExponentValue(n);
  string* treeLeaves = getTreeLeaves(exponent);
  string** tree = new string*[n + 1];
  tree[0] = constructBaseLevel(treeLeaves, exponent);
  string root = "";
  for(int i = 1; i <= n; i++){
    tree[i] = constructUpperLevel(tree[i-1], exponent);
    exponent = exponent/2;
    if(i == n){
      root = tree[i][0];
    }
  }
  createRoot(root);
  createBranch(tree, transaction, n);
}

void createRoot(string hash){
  std::string rootName = "root.txt";
  createEmptyTextFile(rootName);
  std::ofstream root(rootName, std::ios::binary);
  for (size_t i = 0; i < hash.length(); i += 2)
  {
    uint16_t byte;
    std::string nextbyte = hash.substr(i, 2);
    std::istringstream(nextbyte) >> std::hex >> byte;
    root << static_cast<uint8_t>(byte);
  }
  root.close();
}

void createBranch(string** tree, string transaction, int n){
  std::string branchName = "branch.txt";
  int transactionId = atoi(transaction.substr(2).c_str()) - 1;
  createEmptyTextFile(branchName);
  std::ofstream branch(branchName, std::ios::binary);
  for(int i = 0; i <= n; i++)
  {
    std::string hash = tree[i][transactionId] + "0A";
    for (size_t j = 0; j < hash.length(); j += 2)
    {
      uint16_t byte;
      std::string nextbyte = hash.substr(j, 2);
      std::istringstream(nextbyte) >> std::hex >> byte;
      branch << static_cast<uint8_t>(byte);
    }
    transactionId = transactionId/2;
  }
  branch.close();
}

int getExponentValue(int n){
  return (int)pow(2, n);
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

void createEmptyTextFile(string fileName){
  std::string cmdBuilder = fileName;
  cmdBuilder.insert(0, "touch ");
  system(cmdBuilder.c_str());
  cmdBuilder = "echo \"\" > ";
  cmdBuilder.insert(cmdBuilder.length(), fileName);
  system(cmdBuilder.c_str());
}
