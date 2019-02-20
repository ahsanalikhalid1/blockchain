#ifndef MYFILE3_H
#define MYFILE3_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include "sha256.h"
#include <fstream>
#include "ecc.h"
#include "blockchain.h"
#include "transactions.h"

using namespace std;

struct transplusamount {
  vector<UnspentTxOut> myIncludedUnspentTxOuts;
  int leftoveramount;
};

struct txoutplusleftover {
  TxOut txout;
  TxOut leftoveramount;
};

string privateKeyLocation = "private_key.txt";
string publicKeyLocation = "public_key.txt";

string getPrivateFromWallet(){
  string line;
  string neww = "";
  ifstream myfile (privateKeyLocation);
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      neww += line;
    }
    myfile.close();
  }
  return  neww;
}

string getPublicFromWallet(){
  string line;
  string neww = "";
  ifstream myfile (publicKeyLocation);
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      neww += line;
    }
    myfile.close();
  }
  return neww;
}

string printMyPublicKey(){
  string myAddress = "";
  uint8_t u_public[ECC_BYTES+1];
  string s = getPublicFromWallet();
  copy(s.begin(), s.end(), u_public);

    for (int i = 0; i < ECC_BYTES+1; i++){
       myAddress += to_string(u_public[i]);
  }

  return myAddress;

}

string printMyPrivateKey(){
  string myPrivate = "";
  uint8_t u_private[ECC_BYTES];
  string s = getPrivateFromWallet();
  copy(s.begin(), s.end(), u_private);

    for (int i = 0; i < ECC_BYTES; i++){
       myPrivate += to_string(u_private[i]);
  }
  return myPrivate;
}

void initWallet(){
  uint8_t u_publicKey[ECC_BYTES+1];
  uint8_t u_privateKey[ECC_BYTES];

  int retval = ecc_make_key(u_publicKey, u_privateKey);

  ofstream file1;
  file1.open(privateKeyLocation);
  file1 << u_privateKey;
  file1.close();

  ofstream file2;
  file2.open(publicKeyLocation);
  file2 << u_publicKey;
  file2.close();

  cout << "Your public address is " + printMyPublicKey();
  cout << "\n\n";
}

void deleteWallet(){
  ofstream file1;
  file1.open(privateKeyLocation);
  file1 << "0";
  file1.close();

  ofstream file2;
  file2.open(publicKeyLocation);
  file2 << "0";
  file2.close();
}

int getBalance(string address, vector<UnspentTxOut>& unspentTxOuts){
  int totalamount;

  for (int i = 0; i < unspentTxOuts.size(); i++){
    if (unspentTxOuts[i].address == address){
      totalamount += unspentTxOuts[i].amount;
    }
  }

  return totalamount;
}

transplusamount findTxOutsForAmount(int amount, vector<UnspentTxOut>& myUnspentTxOuts){
  int currentAmount = 0;
  vector<UnspentTxOut> includedUnspentTxOuts;
  transplusamount result;

  for (int i = 0; i < myUnspentTxOuts.size(); i++){
    includedUnspentTxOuts.push_back(myUnspentTxOuts[i]);
    currentAmount += myUnspentTxOuts[i].amount;
    if(currentAmount >= amount){
      int leftoveramount = currentAmount - amount;
      result = {includedUnspentTxOuts, leftoveramount};
      break;
    }
  }

  if (currentAmount >= amount ){
    return result;
  } else {
    cout << "Not enough AAK coins\n";

  }

}

txoutplusleftover createTxOuts (string receiverAddress, string myAddress, int amount, int leftOverAmount){
  TxOut txOut1(receiverAddress, amount);
  if (leftOverAmount == 0){
    TxOut leftOverAmountTx(myAddress, 0);
    txoutplusleftover result = {txOut1, leftOverAmountTx};
    return result;
  } else {
    TxOut leftOverAmountTx(myAddress, leftOverAmount);
    txoutplusleftover result = {txOut1, leftOverAmountTx};
    return result;
  }
}

TxIn toUnsignedTxIn (UnspentTxOut unspentTxOut){
  TxIn taxIn;
  taxIn.txOutId = unspentTxOut.txOutId;
  taxIn.txOutIndex = unspentTxOut.txOutIndex;
  return taxIn;
}

vector<UnspentTxOut> filterTxPoolTxs(vector<UnspentTxOut>& myUnspentTxOuts, vector<Transaction>& transactionPool){
  vector<TxIn> txIns;
  vector<UnspentTxOut> result;

  for (int i = 0; i < transactionPool.size(); i++){
    for (int j = 0; j <transactionPool[i].txIns.size(); j++){
    txIns.push_back(transactionPool[i].txIns[j]);
    }
  }

      for (int j = 0; j < myUnspentTxOuts.size(); j++){
        for (int i = 0; i < txIns.size(); i++){
        if(myUnspentTxOuts[j].txOutId == txIns[i].txOutId && myUnspentTxOuts[j].txOutIndex != txIns[i].txOutIndex){
          myUnspentTxOuts.erase(myUnspentTxOuts.begin() + j);
        }
      }
  }

  return myUnspentTxOuts;


}

Transaction createTransaction(string receiverAddress, int amount, vector<UnspentTxOut>& unspentTxOuts, vector<Transaction>& txPool){
  string myAddress = "";
  uint8_t publicc[ECC_BYTES+1];
  uint8_t priv[ECC_BYTES];
  vector<UnspentTxOut> myUnspentTxOutsA;
  vector<UnspentTxOut> myUnspentTxOuts;
  vector<TxIn> unsignedTxIns;
  vector<TxOut> createdTxOuts;
  Transaction t2;
  t2.id = "0";

  string s = getPublicFromWallet();
  copy(s.begin(), s.end(), publicc);
  string s2 = getPrivateFromWallet();
  copy(s2.begin(), s2.end(), priv);

    for (int i = 0; i < ECC_BYTES+1; i++){
       myAddress += to_string(publicc[i]);
  }

    for (int i = 0; i < unspentTxOuts.size(); i++){
      if (unspentTxOuts[i].address == myAddress){
        myUnspentTxOutsA.push_back(unspentTxOuts[i]);
      }
    }

    myUnspentTxOuts = filterTxPoolTxs(myUnspentTxOutsA, txPool);

    transplusamount s1 = findTxOutsForAmount(amount, myUnspentTxOuts);

    if(s1.myIncludedUnspentTxOuts.size() == 0){
      cout << "Not enough amount\n";
      return t2;
    }

    for (int i = 0; i < s1.myIncludedUnspentTxOuts.size(); i++){
      unsignedTxIns.push_back(toUnsignedTxIn(s1.myIncludedUnspentTxOuts[i]));
    }

    Transaction tx;
    // tx.id = sha256()
    tx.txIns = unsignedTxIns;
    txoutplusleftover t = createTxOuts(receiverAddress, myAddress, amount, s1.leftoveramount);
    if(t.leftoveramount.amount != 0){
    createdTxOuts.push_back(t.txout);
    createdTxOuts.push_back(t.leftoveramount);
  } else {
      createdTxOuts.push_back(t.txout);
  }
    tx.txOuts = createdTxOuts;


      tx.id = getTransactionId(tx);
      for (int i = 0; i < tx.txIns.size(); i++){
      string result = signTxIn(tx,i, priv, unspentTxOuts);
        tx.txIns[i].signature = result;
      }
      return tx;
}


#endif
