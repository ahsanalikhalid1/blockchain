#ifndef MYFILE1_H
#define MYFILE1_H

#include <iostream>
#include <vector>
#include <ctime>
#include "sha256.h"
#include <cstring>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <fstream>
#include <cmath>
#include "transactions.h"
#include "wallet.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include "transactionPool.h"



using namespace std;
using std::string;
using json = nlohmann::json;
using namespace std;


//in seconds
#define BLOCK_GEN_INTERVAL 5
//in blocks
#define DIFF_ADJUSTMENT_INTERVAL 1

string getPrivateFromWallet();

class Block {
public:
  int index;
  string prevHash;
  vector<Transaction> data;
  long timestamp;
  string hash;
  int difficulty;
  int nonce;

public:
Block(){
  int index;
  string prevHash;
  vector<Transaction> data(5);
  long timestamp;
  string hash;
  int difficulty;
  int nonce;
}


  public:
  Block(int indexx,string prevHashh, long timestampp, vector<Transaction>& dataa,string hashh, int difficultyy, int noncee){
    index = indexx;
    prevHash = prevHashh;
    timestamp = timestampp;
    data = dataa;
    hash = hashh;
    difficulty = difficultyy;
    nonce = noncee;
  }
};

string getBlockString(Block B){

    string myBlock = "";
    auto i = std::to_string(B.index);
    auto d = std::to_string(B.difficulty);
    auto n = std::to_string(B.nonce);
    auto t = std::to_string(B.timestamp);



    myBlock += "\n{\nIndex: " + i + "\n";
    myBlock +=" Current Hash: " + B.hash + "\n";
    myBlock +=" PrevHash: " + B.prevHash + "\n";
    myBlock += " Timestamp: " + t + "\n" ;
    myBlock += " Nonce: " + n + "\n";
    myBlock +=" Difficulty: " + d + "\n";

    for (int i = 0; i < B.data.size(); i++){
      myBlock += " Transaction ID: " + B.data[i].id + "\n";
      myBlock += "\n";
      for (int j = 0; j <B.data[i].txIns.size(); j++){
        myBlock += " TxIn[" + to_string(j) + "] = TxOutId: " + B.data[i].txIns[j].txOutId + " TxOutIndex: " + to_string(B.data[i].txIns[j].txOutIndex) + " Signature: " + B.data[i].txIns[j].signature + "\n";
      }
      myBlock += "\n";
      for (int j = 0; j <B.data[i].txOuts.size(); j++){
      myBlock += " TxOut[" + to_string(j) + "] = TxOutId: " + B.data[i].txOuts[j].address + " Amount: " + to_string(B.data[i].txOuts[j].amount) + "\n\n";
      }

    }
     myBlock += "}";

     return myBlock;
};

Block getGenesisBlock () {
  vector<Transaction> t = {};
  Transaction t1;
  TxIn t2("0", 0, "0");
  TxOut t3("Cba", 50);
  t1.txIns.push_back(t2);
  t1.txOuts.push_back(t3);
  t1.id = getTransactionId(t1);
  t.push_back(t1);
  long now = time(0);
  Block genBlock(0, "0", now, t, "hashofgenblock", 0, 0);

  ofstream file;
  file.open("data.txt");
  file<<"\n";
  file <<getBlockString(genBlock);
  file.close();

  return genBlock;
};

vector<Block> blockchain = {getGenesisBlock()};

string getBlockchainString(){
  string blockchainString = "";
  for (int i = 0; i < blockchain.size(); i++){
    blockchainString += getBlockString(blockchain[i]);
  }
  return blockchainString.c_str();
}

vector<UnspentTxOut> unspentTxOuts = processTransactions(blockchain[0].data,unspentTxOuts, 0);

vector<Transaction> transactionPool;

vector<Block> getBlockchain() {
 return blockchain;
};

vector<UnspentTxOut> getUnspentTxOuts(){
  return unspentTxOuts;
}

void setUnspentTxOuts(vector<UnspentTxOut> &newUnspentTxOut){
  cout << "Unspent Transactions Are Replaced!\n";
  unspentTxOuts = newUnspentTxOut;
}

Block getLatestBlock(){
  Block latest = blockchain[blockchain.size()-1];
  return latest;
}

int getAdjustedDifficulty(Block latestblock, vector<Block>& aBlockchain){
  Block prevAdjustmentBlock = aBlockchain[blockchain.size() - DIFF_ADJUSTMENT_INTERVAL];
  int timeExpected = BLOCK_GEN_INTERVAL * DIFF_ADJUSTMENT_INTERVAL;
  int timeTaken = latestblock.timestamp - prevAdjustmentBlock.timestamp;
  if (timeTaken < timeExpected / 2){
    return prevAdjustmentBlock.difficulty + 1;
  } else if (timeTaken > timeExpected * 2){
    return prevAdjustmentBlock.difficulty - 1;
  } else {
    return prevAdjustmentBlock.difficulty;
  }
}

int getDifficulty (vector<Block> aBlockchain){
  Block latestblock = aBlockchain[aBlockchain.size()-1];
  if (latestblock.index % DIFF_ADJUSTMENT_INTERVAL == 0 && latestblock.index != 0){
    return getAdjustedDifficulty(latestblock, aBlockchain);
  } else {
    return latestblock.difficulty;
  }
}

long getCurrentTimeStamp(){
  return time(0);
}

vector<Transaction> getTransactionPool(){
  return transactionPool;
}

void setTransactionPool(vector<Transaction> &newTransactionPool){
  cout << "Transactions Pool is Replaced!\n";
  transactionPool = newTransactionPool;
}

string getTransactionString(Transaction t){
  string transactionString = "\n{\nTransaction ID: " + t.id + "\n\n";

  for (int j = 0; j <t.txIns.size(); j++){
    transactionString += "TxIn[" + to_string(j) + "] = TxOutId: " + t.txIns[j].txOutId + " TxOutIndex: " + to_string(t.txIns[j].txOutIndex) + " Signature: " + t.txIns[j].signature + "\n";
  }
  cout << "\n";
  for (int j = 0; j <t.txOuts.size(); j++){
    transactionString += "TxOut[" + to_string(j) + "] = TxOutId: " + t.txOuts[j].address + " Amount: " + to_string(t.txOuts[j].amount) + "\n\n";
  }

  transactionString += "}\n";

  return transactionString;
}

void initUnspent(){
  uint8_t myAddr[ECC_BYTES+1];
  string myAddress;
  string s = getPublicFromWallet();
  copy(s.begin(), s.end(), myAddr);
  for (int i = 0; i < ECC_BYTES + 1; i++){
       myAddress += to_string(myAddr[i]);
}
UnspentTxOut U1("1",1, myAddress,100);
unspentTxOuts.push_back(U1);
cout << "100 AAK coins transferred to your account\n";

}

void printUnspentTxOut(UnspentTxOut t){
  auto txOutIndexx = to_string(t.txOutIndex);
  auto amount = to_string(t.amount);

  cout << "\n{\nTxOutId: " + t.txOutId + "\n";
  cout << "TxOutIndex: " + txOutIndexx + "\n";
  cout << "Address: " + t.address + "\n";
  cout << "Amount: " + amount + "\n" ;
  cout << "}\n";
}

void printUnspentTxOuts(){
  for (int i = 0; i < unspentTxOuts.size(); i++){
    printUnspentTxOut(unspentTxOuts[i]);
  }
}

vector<UnspentTxOut> findUnspentTxOuts(string ownerAddress, vector<UnspentTxOut> utxos){
  vector<UnspentTxOut> result;
  for (int i = 0; i < utxos.size(); i++){
    if(utxos[i].address == ownerAddress){
      result.push_back(utxos[i]);
    }
  }
  return result;
}

vector<UnspentTxOut> getMyUnspentTransactionOutputs(){
  string myAddress = "";
  string s = getPublicFromWallet();
  uint8_t publicc[ECC_BYTES+1];
  copy(s.begin(),s.end(),publicc);
  for (int i = 0; i < ECC_BYTES + 1; i++){
    myAddress += to_string(publicc[i]);
}
  vector<UnspentTxOut> myutxos = findUnspentTxOuts(myAddress,getUnspentTxOuts());
  return myutxos;
}

void printMyUnspentTransactionOutputs(){
  string myAddress = "";
  string s = getPublicFromWallet();
  uint8_t publicc[ECC_BYTES+1];
  copy(s.begin(),s.end(),publicc);
  for (int i = 0; i < ECC_BYTES + 1; i++){
    myAddress += to_string(publicc[i]);
}

  for (int i = 0; i < unspentTxOuts.size(); i++){
    if(unspentTxOuts[i].address == myAddress){
      printUnspentTxOut(unspentTxOuts[i]);
    }
  }
}

int getLengthOfChain() {
  int length = blockchain.size();
  return length;
};

void printSignatureForTxIn(TxIn T){
  uint8_t u_signature[ECC_BYTES*2];
  string mysig = "";
  copy(T.signature.begin(),T.signature.end(),u_signature);
  for(int i = 0; i < ECC_BYTES * 2; i++){
    mysig += to_string(u_signature[i]);
  }
  cout << mysig;
}

void printTransaction(Transaction t){

  cout << "\n{\nTransaction ID: " + t.id + "\n\n";

  for (int j = 0; j <t.txIns.size(); j++){
    cout <<"TxIn[" + to_string(j) + "] = TxOutId: " + t.txIns[j].txOutId + " TxOutIndex: " + to_string(t.txIns[j].txOutIndex);
    cout << " Signature: ";
    printSignatureForTxIn(t.txIns[j]);
    cout << "\n";
  }
  cout << "\n";
  for (int j = 0; j <t.txOuts.size(); j++){
    cout <<"TxOut[" + to_string(j) + "] = TxOutId: " + t.txOuts[j].address + " Amount: " + to_string(t.txOuts[j].amount) + "\n\n";
  }

  cout << "}\n";

}

void printTransactionPool(){
  if(transactionPool.size()>0){
  for (int i = 0; i <transactionPool.size(); i++){
    printTransaction(transactionPool[i]);
  }
} else {
  cout << "No transactions in the pool at the moment\n";
}

}

void printBlock(Block B){

    auto i = std::to_string(B.index);
    auto d = std::to_string(B.difficulty);
    auto n = std::to_string(B.nonce);
    auto t = std::to_string(B.timestamp);



    cout << "\n{\nIndex: " + i + "\n";
    cout << "Current Hash: " + B.hash + "\n";
    cout << "PrevHash: " + B.prevHash + "\n";
    cout << "Timestamp: " + t + "\n" ;
    cout << "Nonce: " + n + "\n";
    cout << "Difficulty: " + d + "\n";

    for (int i = 0; i < B.data.size(); i++){
      cout << "Transaction ID: " + B.data[i].id + "\n";
      cout << "\n";
      for (int j = 0; j <B.data[i].txIns.size(); j++){
        cout <<"TxIn[" + to_string(j) + "] = TxOutId: " + B.data[i].txIns[j].txOutId + " TxOutIndex: " + to_string(B.data[i].txIns[j].txOutIndex);
        cout << " Signature: ";
        printSignatureForTxIn(B.data[i].txIns[j]);
        cout << "\n";
      }
      cout << "\n";
      for (int j = 0; j <B.data[i].txOuts.size(); j++){
        cout <<"TxOut[" + to_string(j) + "] = TxOutId: " + B.data[i].txOuts[j].address + " Amount: " + to_string(B.data[i].txOuts[j].amount) + "\n\n";
      }

    }
cout << "}";
};

void printBlockchain() {

  int length =  getLengthOfChain();
  for (int i = 0; i <length; i++){
  printBlock(blockchain[i]);
};
};

string calculateHash(int index, string prevHash, long timestamp, vector<Transaction>& data, int difficulty, int nonce){
  string indexstring = to_string(index);
  string diffstring = to_string(difficulty);
  string noncestring = to_string(nonce);
  string timestring = to_string(timestamp);

  string dataString;
  for (int i = 0; i < data.size(); i++){
    dataString += data[i].id;
    for (int j = 0; j <data[i].txIns.size(); j++){
      dataString += data[i].txIns[j].txOutId + to_string(data[i].txIns[j].txOutIndex) + data[i].txIns[j].signature;
    }
    for (int j = 0; j <data[i].txOuts.size(); j++){
      dataString += data[i].txOuts[j].address + to_string(data[i].txOuts[j].amount);
    }

  }
  string output = sha256(indexstring + prevHash + timestring + dataString + diffstring + noncestring);
  return output;
}

string calculateHashForBlock(Block B){
  return calculateHash(B.index, B.prevHash, B.timestamp, B.data, B.difficulty, B.nonce);
};

bool hashMatchesBlockContent(Block B){
  string blockHash = calculateHashForBlock(B);
  return (blockHash == B.hash);
}

string hexToBinary(string s){
  int i = 0;
  string binarystring = "";
	while(s[i])
	{
		switch(s[i])
		{
			case '0' : binarystring  += "0000";
				break;
			case '1' : binarystring  += "0001";
				break;
			case '2' : binarystring  += "0010";
				break;
			case '3' : binarystring  += "0011";
				break;
			case '4' : binarystring  += "0100";
				break;
			case '5' : binarystring  += "0101";
				break;
			case '6' : binarystring  += "0110";
				break;
			case '7' : binarystring  += "0111";
				break;
			case '8' : binarystring  += "1000";
				break;
			case '9' : binarystring  += "1001";
				break;
			case 'A' : binarystring  += "1010";
				break;
			case 'B' : binarystring  += "1011";
				break;
			case 'C' : binarystring  += "1100";
				break;
			case 'D' : binarystring  += "1101";
				break;
			case 'E' : binarystring  += "1110";
				break;
			case 'F' : binarystring  += "1111";
				break;
			case 'a' : binarystring  += "1010";
				break;
			case 'b' : binarystring  += "1011";
				break;
			case 'c' : binarystring  += "1100";
				break;
			case 'd' : binarystring  += "1101";
				break;
			case 'e' : binarystring  += "1110";
				break;
			case 'f' : binarystring  += "1111";
				break;
			default : cout<<"\nInvalid hexadecimal digit "<<s[i];
		}
		i++;
	}
return binarystring;
}

bool hashMatchesDifficulty(string hash, int difficulty){
  string hashInBinary = hexToBinary(hash);
  for (int i = 0; i < difficulty-1; i++){
    if(hashInBinary[i] != '0'){
      return false;
    }
  }
  return true;
}

bool hasValidHash(Block B){
  if(!hashMatchesBlockContent(B)){
    cout << "Invalid hash, got: " + B.hash;
    return false;
  }
  if(!hashMatchesDifficulty(B.hash, B.difficulty)){
    auto d = std::to_string(B.difficulty);
    cout<<"Block Difficulty Not Satisfied. Expexted: " + d + "got: " + B.hash;
    return false;
  }
  return true;
}

Block findBlock(int index, string prevHash, long timestamp, vector<Transaction>& data, int difficulty){
  int nonce = 0;
  while (1){
    string hash = calculateHash(index, prevHash, timestamp, data, difficulty, nonce);
    if (hashMatchesDifficulty(hash, difficulty)){
      Block B;
      B.index = index;
      B.prevHash = prevHash;
      B.timestamp = timestamp;
      B.hash = calculateHash(index, prevHash, timestamp, data, difficulty, nonce);
      B.data = data;
      B.difficulty = difficulty;
      B.nonce = nonce;
      return B;
     }
    nonce++;
    cout << "Finding block.....\n";
  }
}

bool isValidTimestamp(Block newBlock, Block prevBlock){
  return (prevBlock.timestamp - 60 < newBlock.timestamp && newBlock.timestamp - 60 < getCurrentTimeStamp());
}

bool isValidBlockStructure (Block B){

  if (typeid(B) == typeid(Block)){
    return true;
  }
  return false;

}

bool isValidNewBlock (Block newBlock, Block prevBlock) {

  if (!isValidBlockStructure(newBlock)){
    cout<< "Invalid Block Structure";
    return false;
  }
  if (prevBlock.index + 1 != newBlock.index){
    cout<<"Invalid Index";
    return false;
  } else if (prevBlock.hash != newBlock.prevHash){
    cout<<"Invalid Previous Hash";
    return false;
  } else if (!hasValidHash(newBlock)){
    cout << "Invalid Hash";
    return false;
  }
  else if (!isValidTimestamp(newBlock, prevBlock)){
    cout << "Invalid timestamp";
    return false;
  }

  return true;
}

bool isValidGenesis(vector<Block>& aBlockchain){

    Block B = getGenesisBlock();
    bool b = (aBlockchain[0].hash == B.hash &&
            aBlockchain[0].index == B.index &&
            aBlockchain[0].timestamp == B.timestamp &&
            aBlockchain[0].difficulty == B.difficulty &&
            aBlockchain[0].nonce == B.nonce &&
            aBlockchain[0].prevHash == B.prevHash);

            return b;
}

bool addBlockToChain(Block& B){

    if(isValidNewBlock(B,getLatestBlock())){
      vector<UnspentTxOut> retVal;
      retVal = processTransactions(B.data, unspentTxOuts, B.index);
    if (retVal.size() == 0){
      cout << "Blockchain is not valid in terms of transactions\n";
      return false;
    } else {
      blockchain.push_back(B);
      ofstream file;
      file.open("data.txt", std::ios_base::app);
      file << "\n";
      file <<getBlockString(B).c_str();
      file.close();

      setUnspentTxOuts(retVal);
      updateTransactionPool(unspentTxOuts);
      return true;
    }
    return false;
  }
}

Block generateRawNextBlock(vector<Transaction>& blockData){

    Block previousBlock = getLatestBlock();
    int difficulty = getDifficulty(getBlockchain());
    int nextIndex = previousBlock.index + 1;
    long nextTimestamp = getCurrentTimeStamp();

    Block newBlock = findBlock(nextIndex, previousBlock.hash, nextTimestamp, blockData, difficulty);
    return newBlock;
}

Block generateNextBlock(){

  vector<Transaction> transPool = getTransactionPool();
  string coinbaseAddress = "Cba";
  Transaction coinbaseTx = getCoinbaseTransaction(coinbaseAddress, getLatestBlock().index + 1);
  vector<Transaction> blockData;
  blockData.push_back(coinbaseTx);
  if (transPool.size() != 0){
  for(int i = 0; i <transPool.size(); i++){
    blockData.push_back(transPool[i]);
  }
}

  return generateRawNextBlock(blockData);

}

Block generatenextBlockWithTransaction(string receiverAddress, int amount){

  Block B;
  B.hash = "0";

  vector<UnspentTxOut> utxos = getUnspentTxOuts();
  vector<Transaction> transPool = getTransactionPool();

  string coinbaseAddress = "Cba";
  Transaction coinbaseTx = getCoinbaseTransaction(coinbaseAddress, getLatestBlock().index + 1);
  Transaction tx = createTransaction(receiverAddress, amount, utxos, transPool);
  if(tx.id != "0"){
  vector<Transaction> blockData = {coinbaseTx, tx};
  return generateRawNextBlock(blockData);
  } else {
  return B;
  }

}

int getBalance(string address){
  int balance = 0;
  vector<UnspentTxOut> utxos = getUnspentTxOuts();
  for (int i = 0; i < utxos.size(); i++){
    if (utxos[i].address == address){
      balance += utxos[i].amount;
    }
  }
  return balance;
}

void getAccountBalance(){
  int balance = 0;
  string myAddress = "";
  string s = getPublicFromWallet();
  uint8_t publicc[ECC_BYTES+1];
  copy(s.begin(),s.end(),publicc);
  for (int i = 0; i < ECC_BYTES + 1; i++){
    myAddress += to_string(publicc[i]);
}
  balance = getBalance(myAddress);

  cout << "You have " + to_string(balance) + " AAK coins in your wallet";
}

void getAddress(){
  string myAddress = "";
  string s = getPublicFromWallet();
  uint8_t publicc[ECC_BYTES+1];
  copy(s.begin(),s.end(),publicc);
  for (int i = 0; i < ECC_BYTES + 1; i++){
    myAddress += to_string(publicc[i]);
}
  cout << "My address is: " + myAddress;
}

int getAccumulatedDifficulty(vector<Block>& aBlockchain){
  int AccumulativeDifficulty = 0;
  for (int i = 0; i<aBlockchain.size(); i++){
    AccumulativeDifficulty += pow(2,aBlockchain[i].difficulty);
  }
  return AccumulativeDifficulty;
}

void addToTransactionPool (Transaction tx, vector<UnspentTxOut>& unspentTxOuts){

  if(!validateTransaction(tx,unspentTxOuts)){
    cout << "Error: Invalid transaction\n";
  }
  if(!isValidTxForPool(tx,transactionPool)){
    cout << "Trying to add invalid tx to pool\n";
  }

  cout <<"Adding tx to the txPool\n";
  printTransaction(tx);
  transactionPool.push_back(tx);
  cout << "Tx Added\n";
}

void sendTransaction(string address, int amount){
  vector<Transaction> transPool = getTransactionPool();
  vector<UnspentTxOut> utxos = getUnspentTxOuts();

  Transaction tx = createTransaction(address,amount,utxos,transPool);

  if (tx.id != "0"){
  addToTransactionPool(tx,utxos);
  } else {
  cout << "Transaction not sent to the pool\n";
  }
}

void handleReceivedTransaction(Transaction T){
  vector<UnspentTxOut> utxos = getUnspentTxOuts();
  addToTransactionPool(T,utxos);
}

void findBlockInChain(string hash){
  for(int i = 0; i < blockchain.size(); i++){
    if (blockchain[i].hash == hash){
      cout<< "Block Found\n";
      printBlock(blockchain[i]);
    } else {
      cout << "Block not found in the chain\n";
    }
  }
}

void findTransaction(string id){

  for (int i = 0; i < blockchain.size(); i++){
    for (int j = 0; j < blockchain[i].data.size(); j++){
      if(blockchain[i].data[j].id == id){
        cout << "Transaction found\n";
        printTransaction(blockchain[i].data[j]);
      } else {
        cout << "Transaction not found\n";
      }
    }
  }
}

void runode(int portnumber){

      int backlog, i;
      int server_fd, new_socket; long valread;
      struct sockaddr_in address;
      int addrlen = sizeof(address);
      string url = "";

       const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

 if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
 {
     perror("In socket");
     exit(EXIT_FAILURE);
 }


 address.sin_family = AF_INET;
 address.sin_addr.s_addr = INADDR_ANY;
 address.sin_port = htons( portnumber );

 memset(address.sin_zero, '\0', sizeof address.sin_zero);


  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
  {
      perror("In bind");
      exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 10) < 0)
  {
      perror("In listen");
      exit(EXIT_FAILURE);
  }
  while(1)
     {

         printf("\n++++++++++ Waiting for Input ++++++++++\n\n");
         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
         {
             perror("In accept");
             exit(EXIT_FAILURE);
         }

         char buffer[30000] = {0};
         string post = "POST";
         valread = read(new_socket , buffer, 30000);

         char first = buffer[0];
         int end = strlen(buffer);

         //buffer contains the entire HTTP message received and is parsed by checking if it is a get or a post
         // you can print out buffer to see the entire HTTP message

         if (first == 'G'){

           if(buffer[5] == 'b' &&  buffer[6] == 'l' && buffer[7] == 'o' && buffer[8] == 'c' && buffer[9] == 'k' && buffer[10] == 's'){
                          printBlockchain();

           }

           else if (buffer[5] == 'm' && buffer[6] == 'i' && buffer[7] == 'n' && buffer[8] == 'e' && buffer[9] == 'B' && buffer[10] == 'l' && buffer[11] == 'o' && buffer[12] == 'c' && buffer[13] == 'k'){

                  Block B = generateNextBlock();
                  addBlockToChain(B);
                  cout << "\nBlock mined!\n";

          }

           else if (buffer[5] == 'b' && buffer[6] == 'a' && buffer[7] == 'l' && buffer[8] == 'a' && buffer[9] == 'n' && buffer[10] == 'c' && buffer[11] == 'e'){

                          getAccountBalance();
           }

           else if (buffer[5] == 'u' && buffer[6] == 't' && buffer[7] == 'x' && buffer[8] == 'o' && buffer[9] == 's'){

                          printUnspentTxOuts();

           }

           else if (buffer[5] == 'm' && buffer[6] == 'y' && buffer[7] == 'U' && buffer[8] == 't' && buffer[9] == 'x' && buffer[10] == 'o' && buffer[11] == 's'){

                          printMyUnspentTransactionOutputs();

           }

           else if (buffer[5] == 'm' && buffer[6] == 'y' && buffer[7] == 'A' && buffer[8] == 'd' && buffer[9] == 'd' && buffer[10] == 'r'){

                          getAddress();

           }

           else if (buffer[5] == 't' && buffer[6] == 'r' && buffer[7] == 'a' && buffer[8] == 'n' && buffer[9] == 's' && buffer[10] == 'P' && buffer[11] == 'o' && buffer[12] == 'o' && buffer[13] == 'l'){
                          printTransactionPool();
           }

           else if (buffer[5] == 'a' && buffer[6] == 'd' && buffer[7] == 'd' && buffer[8] == 'r' && buffer[9] == 'e' && buffer[10] == 's' && buffer[11] == 's' && buffer[12] == '/'){
            string myaddr = "";
             int length = sizeof(buffer)/sizeof(*buffer);
             int j = 12;
             while (j < length){
                if(buffer[j] != ' '){j++;}
                  else {break;}
                   };
                            for (int i = 13; i < j; i++){
                              myaddr += buffer[i];
                            }
                            cout << "Balance corresponding to this address is " + to_string(getBalance(myaddr));
           }

           else if (buffer[5] == 't' && buffer[6] == 'r' && buffer[7] == 'a' && buffer[8] == 'n' && buffer[9] == 's' && buffer[10] == 'a' && buffer[11] == 'c' && buffer[12] == 't' && buffer[13] == 'i' && buffer[14] == 'o' && buffer[15] == 'n' && buffer[16]  == '/'){
            string transId = "";
             int length = sizeof(buffer)/sizeof(*buffer);
             int j = 16;
             while (j < length){
                if(buffer[j] != ' '){j++;}
                  else {break;}
                   };
                            for (int i = 17; i < j; i++){
                              transId += buffer[i];
                            }

                            findTransaction(transId);
           }

           else if (buffer[5] == 'b' && buffer[6] == 'l' && buffer[7] == 'o' && buffer[8] == 'c' && buffer[9] == 'k' && buffer[10] == '/'){
            string blockhash = "";
             int length = sizeof(buffer)/sizeof(*buffer);
             int j = 10;
             while (j < length){
                if(buffer[j] != ' '){j++;}
                  else {break;}
                   };
                            for (int i = 11; i < j; i++){
                              blockhash += buffer[i];
                            }

                            findBlockInChain(blockhash);
           }

           else if (buffer[5] == 's' && buffer[6] == 't' && buffer[7] == 'o' && buffer[8] == 'p'){

            cout << "Server Stopped!\n";
            exit(1);
           }

           else {
              cout << "Wrong input\n";
          }

         };

         if (first == 'P'){


           if (buffer[6] == 'm' && buffer[7] == 'i' && buffer[8] == 'n' && buffer[9] == 'e' && buffer[10] == 'T' && buffer[11] == 'r' && buffer[12] == 'a' && buffer[13] == 'n' && buffer[14] == 's'){

             int length = sizeof(buffer)/sizeof(*buffer);
             int j = 0;
             while (j < length){
                if(buffer[j] != '{'){j++;}
                  else {break;}
                   };

                   string data;

                   while (j < end){
                     data += buffer[j];  // append character from char_array[index+1]
                     j++;
                   };

                   auto received = json::parse(data);
                   cout << "\n";


                   string address = received.at("address");
                   int amount = received.at("amount");

                   if(amount > 0){
                   Block B = generatenextBlockWithTransaction(address, amount);
                   if (B.hash != "0"){
                   addBlockToChain(B);
                 } else {
                   cout <<"Block Not Added\n";
                 }
               } else{
                 cout << "Inavlid Input\n";
               }

           }

           else if (buffer[6] == 's' && buffer[7] == 'e' && buffer[8] == 'n' && buffer[9] == 'd' && buffer[10] == 'T' && buffer[11] == 'r' && buffer[12] == 'a' && buffer[13] == 'n' && buffer[14] == 's'){

            int length = sizeof(buffer)/sizeof(*buffer);
            int j = 0;
   	        while (j < length){
   	  	       if(buffer[j] != '{'){j++;}
   	  	         else {break;}
   	              };

                  string data;

                  while (j < end){
                    data += buffer[j];  // append character from char_array[index+1]
                    j++;
                  };

                  auto received = json::parse(data);
                  string address = received.at("address");
                  int amount = received.at("amount");

                  if(amount > 0){
                  sendTransaction(address, amount);
                } else {
                  cout << "Invalid Input\n";
                }

           }

           else {
          cout <<"Invalid Input\n";
           }
      };

          write(new_socket , hello , strlen(hello));
          printf("\n\n----------------------------\n");


}


};


#endif
