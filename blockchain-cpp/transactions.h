#ifndef MYFILE2_H
#define MYFILE2_H

#include <iostream>
#include <string>
#include <vector>
#include "blockchain.h"
#include "sha256.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <algorithm>
#include "ecc.h"

using namespace std;
using std::string;
using std::endl;

#define COINBASE_AMOUNT 50

string getPrivateFromWallet();
string getPublicFromWallet();

class UnspentTxOut {
public:

  string txOutId;
  int txOutIndex;
  string address;
  int amount;

public:
  UnspentTxOut(){
    string txOutId;
    int txOutIndex;
    string address = "0";
    int amount;
  }

public:
  UnspentTxOut(string txOutIdd, int txOutIndexx, string addresss, int amountt){
    txOutId = txOutIdd;
    txOutIndex = txOutIndexx;
    address = addresss;
    amount = amountt;
  }
};

struct signOut{
  string signature;
  string hash;
};

class TxIn {
  public:
  string txOutId;
  int txOutIndex;
  string signature;

public:
  TxIn(){
    string txOutId;
    int txOutIndex;
    string signature;
  }

public:
  TxIn(string txOutidd, int txOutIndexx, string signaturee){
    txOutId = txOutidd;
    txOutIndex = txOutIndexx;
    signature = signaturee;
  }
};

class TxOut{
  public:
  string address;
  int amount;

public:
  TxOut(){
    string address;
    int amount;
  }

  public:
  TxOut(string addresss, int amountt){
    address = addresss;
    amount = amountt;
  }
};

class Transaction{
      public:
      string id;
      vector<TxIn> txIns;
      vector<TxOut> txOuts;

    public:
    Transaction(){
      string id;
      vector<TxIn> txIns;
      vector<TxOut> txOuts;

  }

      public:
      Transaction(string idd,vector<TxIn> txInss,vector<TxOut> txOutss){
    id = idd;
    txIns = txInss;
    txOuts = txOutss;

  }
};

string getTransactionId(Transaction transaction){
  string txInContent;
  string txOutContent;
  string transId;

  for (int i=1; i<transaction.txIns.size(); i++){
    txInContent += transaction.txIns[i].txOutId + to_string(transaction.txIns[i].txOutIndex);
  }
  for (int i=1; i<transaction.txOuts.size(); i++){
    txOutContent += transaction.txOuts[i].address + to_string(transaction.txOuts[i].amount);
  }

  transId = sha256(txInContent+txOutContent);
  return transId;
}

bool validateTxIn(TxIn txIn, Transaction transaction, vector<UnspentTxOut>& aUnspentTxOuts){
  UnspentTxOut referencedUtxOut("0",0,"0",0);
  uint8_t hashsig[ECC_BYTES];
  uint8_t u_publicKey[ECC_BYTES+1];
  uint8_t u_signature[ECC_BYTES*2];

  string s = getPublicFromWallet();

  for (int i = 0; i<aUnspentTxOuts.size(); i++){
    if(aUnspentTxOuts[i].txOutId == txIn.txOutId && aUnspentTxOuts[i].txOutIndex == txIn.txOutIndex){
      referencedUtxOut = aUnspentTxOuts[i];
      break;
    }
   }

   if(referencedUtxOut.txOutId == "0" && referencedUtxOut.txOutIndex == 0){
     cout << "UnspentTxOut not found";
     return false;
   };


   for(int i =0; i < ECC_BYTES; i++){
     hashsig[i] = transaction.id[i];
   }

   copy(s.begin(), s.end(), u_publicKey);

   copy(txIn.signature.begin(), txIn.signature.end(), u_signature);

   int retval =  ecdsa_verify(u_publicKey,hashsig, u_signature);

   if (retval != 1){
    cout <<"Input transaction not verified";
    return false;
   }


  return true;
}

bool hasValidTxIns(Transaction transaction,vector<UnspentTxOut>& aUnspentTxOuts){
  for (int i=0; i<transaction.txIns.size(); i++){
  if(!validateTxIn(transaction.txIns[i],transaction,aUnspentTxOuts)){
    return false;
  }
  }
    return true;
}

UnspentTxOut findUnspentTxOut(string transactionId, int index, vector<UnspentTxOut> aUnspentTxOuts){
  UnspentTxOut U("0",0,"0",0);
  for (int i = 0; i <aUnspentTxOuts.size(); i++){
    if(aUnspentTxOuts[i].txOutId == transactionId && aUnspentTxOuts[i].txOutIndex == index){
      U = aUnspentTxOuts[i];
      return U;
    }
  }
  return U;
}

int getTxInAmount(TxIn txIn, vector<UnspentTxOut> aUnspentTxOuts){
  int amount = 0;
  UnspentTxOut t = findUnspentTxOut(txIn.txOutId, txIn.txOutIndex, aUnspentTxOuts);
  amount += t.amount;
  return amount;
}

bool isValidTxInStructure(TxIn txIn){

  if (typeid(txIn) == typeid(TxIn)){
    return true;
  }
  return false;
}

bool isValidTxOutStructure(TxOut txOut){

  if (typeid(txOut) == typeid(TxOut)){
    return true;
  }
  return false;
}

bool isValidTransactionStructure (Transaction transaction){

  if (typeid(transaction) == typeid(Transaction)){
    return true;
  }
  return false;
}

bool isValidTransactionsStructure(vector<Transaction>& transaction){
  int i = 0;
  while (i<transaction.size()){
    if(isValidTransactionStructure(transaction[i])){
      i++;
    } else {
      return false;
    }
  }
  return true;
}

bool validateTransaction(Transaction transaction, vector<UnspentTxOut>& aUnspentTxOuts){

  if(!isValidTransactionStructure(transaction)){
    return false;
  }

  if(getTransactionId(transaction) != transaction.id) {
    cout<<"\nInvalid Transaction Id: " + transaction.id;
    return false;
  }



    if(!hasValidTxIns(transaction,aUnspentTxOuts)){
      cout<<"\nSome transactions invalid are: " + transaction.id;
      return false;
    }


  int totalTxInValues = 0;
  for (int i = 0; i < transaction.txIns.size(); i++){
    totalTxInValues += getTxInAmount(transaction.txIns[i], aUnspentTxOuts);
  }
  int totalTxOutValues = 0;
  for (int j = 0; j < transaction.txOuts.size(); j++){
    totalTxOutValues = totalTxOutValues + transaction.txOuts[j].amount;
  }

  if(totalTxInValues != totalTxOutValues){
    cout <<"\nTotal txOut values are not equal to txIn values in tx: ";
    return false;
  }

  return true;
}

bool hasDuplicates(vector<TxIn>& txIns){
  for (int i = 0; i < txIns.size(); i++){
    for (int j = i+1; j < txIns.size(); j++){
      if (txIns[i].txOutId == txIns[j].txOutId && txIns[i].txOutIndex == txIns[j].txOutIndex && txIns[i].signature == txIns[j].signature){
        return true;
      }
    }
  }
    return false;
}

bool validateCoinbaseTx(Transaction transaction, int blockIndex){

  if(getTransactionId(transaction) != transaction.id){
    cout<<"Invalid coinbase transaction: " + transaction.id;
    return false;
  }
  if(transaction.txIns.size() != 1){
    cout<<"One txIn must be specified in the coinbase transaction\n";
    return false;
  }
  if(transaction.txIns[0].txOutIndex != blockIndex){
    cout<<"The txIn signature in coinbase transaction must be the block height\n";
    return false;
  }
  if(transaction.txOuts.size() != 1){
    cout<<"Invalid number pf txOuts in the coinbase transaction\n";
    return false;
  }
  if(transaction.txOuts[0].amount != COINBASE_AMOUNT){
    cout<<"Invalid coinbase amount in the coinbase transaction\n";
    return false;
  }
  return true;
}

bool validateBlockTransactions (vector<Transaction>& aTransaction, vector<UnspentTxOut>& aUnspentTxOuts, int blockIndex){
  const Transaction coinbaseTx = aTransaction[0];
  int numberOfTrans = aTransaction.size();
  int prevsize = 0;
  if(!validateCoinbaseTx(coinbaseTx,blockIndex)){
    cout<<"Invalid Coinbase Transaction\n";
    return false;
  }

  for (int a = 0; a < aTransaction.size(); a++){

  for (int i = 0; i<aTransaction[a].txIns.size(); i++){
    for (int j = i+1; j < aTransaction[a].txIns.size(); j++){

      if(aTransaction[a].txIns[i].txOutId == aTransaction[a].txIns[j].txOutId && aTransaction[a].txIns[i].txOutIndex == aTransaction[a].txIns[j].txOutIndex && aTransaction[a].txIns[i].signature == aTransaction[a].txIns[j].signature){
        for (int k = j; j<aTransaction[a].txIns.size(); k++){
        aTransaction[a].txIns[k] = aTransaction[a].txIns[k+1];
        prevsize = aTransaction[a].txIns.size();
        aTransaction[a].txIns.resize(prevsize-1);}
      }
    }
  }
}
for (int a = 0; a < aTransaction.size(); a++){
if (hasDuplicates(aTransaction[a].txIns)){
  return false;
}
}

  if(aTransaction.size()>1){
    for (int i = 1; i  < numberOfTrans; i++){
    if(!validateTransaction(aTransaction[i], aUnspentTxOuts)){
      return false;
    }
  }
}

   return true;

}

int getTxInAmount(TxIn txIn,vector<UnspentTxOut>& aUnspentTxOuts){
  UnspentTxOut ReqUnspentTxOut;
  ReqUnspentTxOut = findUnspentTxOut(txIn.txOutId, txIn.txOutIndex, aUnspentTxOuts);
  return ReqUnspentTxOut.amount;
}

Transaction getCoinbaseTransaction(string addresss, int blockIndex){
  TxIn txIn("0",blockIndex,"0");
  string coinbaseAddress = "Cba";

  TxOut txOut(coinbaseAddress,COINBASE_AMOUNT);

  Transaction t;
  t.txIns.push_back(txIn);
  t.txOuts.push_back(txOut);
  t.id = getTransactionId(t);
  return t;
}

string signTxIn (Transaction transaction, int txInIndex, uint8_t privateKey[ECC_BYTES], vector<UnspentTxOut>& aUnspentTxOuts){

  TxIn txIn;
  txIn.txOutId = transaction.txIns[0].txOutId;
  txIn.txOutIndex = transaction.txIns[0].txOutIndex;
  txIn.signature = "dummy";
  UnspentTxOut RefUnspentTxOut;
  uint8_t privt[ECC_BYTES];
  uint8_t u_publicKey[ECC_BYTES+1];
  uint8_t hash[ECC_BYTES];
  uint8_t u_signature[ECC_BYTES*2];

  for(int i =0; i < ECC_BYTES; i++){
    hash[i] = transaction.id[i];
  }

  RefUnspentTxOut = findUnspentTxOut(txIn.txOutId, txIn.txOutIndex, aUnspentTxOuts);

  if (RefUnspentTxOut.address == "0"){
    cout << "Could not find the referenced txOut\n";
    return "0";
  }

  int signval = ecdsa_sign(privateKey, hash, u_signature);
  string signature(u_signature, u_signature+ECC_BYTES*2);


  if (signval == 1){
  return signature;

  } else {
     cout <<"Signatre could not be generated";
     return "0";
  }

}

vector<UnspentTxOut> updateUnspentTxOuts(vector<Transaction>& newTransactions, vector<UnspentTxOut>& aUnspentTxOuts){

vector<UnspentTxOut> newUnspentTxOuts;
vector<UnspentTxOut> consumedTxOuts;

for(int i = 0; i < newTransactions.size(); i++){
  for (int j = 0; j< newTransactions[i].txOuts.size(); j++){
  UnspentTxOut U(newTransactions[i].id, j, newTransactions[i].txOuts[j].address, newTransactions[i].txOuts[j].amount);
  newUnspentTxOuts.push_back(U);
}
}

for(int i = 0; i < newTransactions.size(); i++){
  for (int j = 0; j< newTransactions[i].txIns.size(); j++){
    UnspentTxOut U(newTransactions[i].txIns[j].txOutId, newTransactions[i].txIns[j].txOutIndex," ", 0);
    consumedTxOuts.push_back(U);
}
}

for(int i = 0; i < aUnspentTxOuts.size(); i++){
  for (int j = 0; j <consumedTxOuts.size(); j++){

  if(aUnspentTxOuts[i].txOutId == consumedTxOuts[j].txOutId && aUnspentTxOuts[i].txOutIndex == consumedTxOuts[j].txOutIndex){
    aUnspentTxOuts.erase(aUnspentTxOuts.begin() + i);
  }
  }
}

for (int i = 0; i < newUnspentTxOuts.size(); i++){
  aUnspentTxOuts.push_back(newUnspentTxOuts[i]);
}

return aUnspentTxOuts;

}

vector<UnspentTxOut> processTransactions(vector<Transaction>& aTransactions, vector<UnspentTxOut>& aUnspentTxOuts, int blockIndex){

vector<UnspentTxOut> v;

  if(!validateBlockTransactions(aTransactions, aUnspentTxOuts, blockIndex)){
    cout <<"Invalid block transactions\n";
    return v;
  }

  v = updateUnspentTxOuts(aTransactions, aUnspentTxOuts);
  return v;
}


#endif
