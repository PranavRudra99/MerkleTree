cd source
make

In the source folder, there are 3 cpp files:
VerifyTx.cpp
MerkleGen.cpp
sha256.cpp

Running make command in the source folder will generate the binaries for MerkleGen, VerifyTx.

Running:
./MerkleGen n Txi
Will generate 2 text files: root.txt, branch.txt

./VerifyTx root.txt branch.txt Txi
Will verify whether the transaction(Txi) is valid using the specified files root.txt, and branch.txt
