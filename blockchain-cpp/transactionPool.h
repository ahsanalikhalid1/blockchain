#ifndef MYFILE4_H
#define MYFILE4_H

#include <iostream>
#include "transactions.h"
#include "blockchain.h"

vector<Transaction> getTransactionPool();

void setTransactionPool(vector<Transaction> &newTransactionPool);

bool containsTxIn(vector<TxIn>& txIns, TxIn txIn){

    for (int i = 0; i< txIns.size(); i++){
      if (txIns[i].txOutIndex == txIn.txOutIndex && txIns[i].txOutId == txIn.txOutId){
        return true;
      }
    }

    return false;
}

vector<TxIn> getTxPoolIns(vector<Transaction>& aTransactionPool){
  vector<TxIn> result;
  for (int i = 0; i <aTransactionPool.size(); i++){
    for (int j = 0; j <aTransactionPool[i].txIns.size(); j++){
      result.push_back(aTransactionPool[i].txIns[j]);
    }
  }
  return result;
}

bool isValidTxForPool(Transaction tx, vector<Transaction> aTransactionPool){
  vector<TxIn> txPoolIns = getTxPoolIns(aTransactionPool);

    for (int j = 0; j <tx.txIns.size(); j++){
    if (containsTxIn(txPoolIns, tx.txIns[j])){
      cout << "TxIn already found";
      return false;
    }
  }

return true;
}

bool hasTxIn(TxIn txIn, vector<UnspentTxOut>& unspentTxOuts){
  for (int i = 0; i < unspentTxOuts.size(); i++){
    if(unspentTxOuts[i].txOutId == txIn.txOutId && unspentTxOuts[i].txOutIndex == txIn.txOutIndex){
      return true;
    }
  }
  return false;
}

void updateTransactionPool(vector<UnspentTxOut> unspentTxOuts){
  int prevsize = 0;
  vector<Transaction> invalidTxs(1);
  vector<Transaction> transactionPool1 = getTransactionPool();
  for(int i = 0; i < transactionPool1.size(); i++){
    for (int j = 0; j < transactionPool1[i].txIns.size(); j++){
      if (!hasTxIn(transactionPool1[i].txIns[j], unspentTxOuts)){
        invalidTxs.push_back(transactionPool1[i]);
        break;
      }
    }
  }


if(invalidTxs.size() > 1){

  for (int a = 0; a < transactionPool1.size(); a++){

    for (int i = 0; i<transactionPool1[a].txIns.size(); i++){

      for (int j = 0; j < invalidTxs.size(); j++){

        for (int k = 0; k < invalidTxs[j].txIns.size(); k++){

          if(transactionPool1[a].txIns[i].txOutId == invalidTxs[j].txIns[k].txOutId && transactionPool1[a].txIns[i].txOutIndex == invalidTxs[j].txIns[k].txOutIndex && transactionPool1[a].txIns[i].signature == invalidTxs[j].txIns[k].signature){
            if(transactionPool1.size() > 1){
              transactionPool1[a] = transactionPool1[a+1];
              prevsize = transactionPool1.size();
              transactionPool1.resize(prevsize-1);
            } else {
          prevsize = transactionPool1.size();
          transactionPool1.resize(prevsize-1);
          setTransactionPool(transactionPool1);
                    }
                  }
                }
              }
            }
          }
        }

}



#endif
