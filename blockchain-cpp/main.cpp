
#include "blockchain.h"
#include "sha256.h"
#include "ecc.h"
#include "sha256.h"
#include "wallet.h"
#include "transactions.h"
#include "transactionPool.h"

using namespace std;



int main() {

  initWallet();
  initUnspent();

  runode(8080);

}
