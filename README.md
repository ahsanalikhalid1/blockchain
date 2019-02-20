----------------

AAK Coin: Blockchain simplified so that you can understand its ground rules.

----------------


Features:

1- SHA256 cryptographic algorithm used to hash the blocks

2- Public and Private Keys generated, signed and validated using simple Elliptic-Curve Cryptography (ECC)

3- Proof-of-Work (PoW) for Finding the Correct Hash Implemented - Difficulty can be adjusted as per will using (BLOCK_GEN_INTERVAL  & DIFF_ADJUSTMENT_INTERVAL)

4- Unspent Transaction Output (UTXO Model) Used For Keeping a Record of Balance

5- P2P framework not included yet

This code can also be considered as a single node at hand. All the workings, block structure,
transaction structure & generation, wallet creation, balance calculation etc. have been
explained using the simplest elements like vectors and text files so that a crystal-clear
understanding can be developed.

The code is not to be used in production as the code has not been optimized. Memory intensive for-loops,
fixed sized arrays etc. have been used for ease of understanding.

- The C++ implementation has also been inspired by NaiveCoin whose complete JavaScript code can be found
here. https://lhartikk.github.io/


----------------


In order to run the code, following libraries have been used.

1- https://github.com/nlohmann/json - In order to deal with JSON's

2- https://github.com/esxgx/easy-ecc - In order to use Elliptic-Curve Cryptography -
 * Copyright (c) 2013, Kenneth MacKay
 * All rights reserved.

3- http://www.zedwood.com/article/cpp-sha256-function - In order to implement sha256 hash generation
 * Updated to C++, zedwood.com 2012
 * Based on Olivier Gay's version
 * See Modified BSD License below:
 *
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Issue date:  04/30/2005
 * http://www.ouah.org/ogay/sha2/
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.

gcc version 7.4.0 (GCC) was used during the development of this code using Cygwin. If you are
facing any troubles with your compiler, making the compiler versions same will help

Running the Code:

- Compile & run the 3 files main.cpp ecc.cpp sha256.cpp
- A terminal will open up which signfies that your node is now up and running and you
can make your request to it using your browser or the curl tool. Following commands can
be used to interact with the node. You'll need to download the curl terminal from here the
link (https://curl.haxx.se/) in order to send HTTP requests to the node.

Git Bash will be needed for sending curl requests.

*GET Requests can also be send from the brower URL, for POST curl is better*

----------------

- Get Requests

1- Print Out Blockchain

curl localhost:8080/blocks

2- Mines new block transactions in the transaction pool & if it is empty onlt Coinbase transaction is mined

curl localhost:8080/mineBlock

3- Print Out Balance

curl localhost:8080/balance

4- Print Out Address Linked to the Node

curl localhost:8080/myAddr

5- Printing Out the Transaction Pool

curl localhost:8080/transPool

6- Printing Out All UTXOs

curl localhost:8080/utxos

7- Printing Out UTXOs Linking to Your Node's Address

curl localhost:8080/myUtxos

8- Printing Out Balance Information Linking to the Address Entered

curl localhost:8080/address/{Enter address here without these square brackets}

9- Printing Out Transaction Linked to the ID Entered

curl localhost:8080/transaction/{Enter id here without these square brackets}

10- Printing Out Block Linking to the Hash Entered

curl localhost:8080/block/{Enter block hash here without these square brackets}

11- Stopping the server

curl localhost:8080/stop


- Posts Requests

1- Send New Transaction to the Transaction Pool

curl -H "Content-type:application/json" --data '{"address" : "DummyAddress" ,  "amount" : 10}' http://localhost:8080/sendTrans

2- Mines A New Block With the Transaction Entered

curl -H "Content-type:application/json" --data '{"address" : "DummyAddress" ,  "amount" : 10}' http://localhost:8080/mineTrans

*NOTE: In the text files public and private keys will appear in distorted form. This is because the library used for
generating them uses the formate uint8_t. If keys are represented as normal string and stored in text file, they cannot
be extracted back in the code as an array with uint8_t array can contain elements like 128, 45 etc. while for a string
or char array, each element represents only a specific integer or character such as 1,2,a etc. So in order to avoid
memory issues keys are stored in a way they can be salvaged again.

If you want to see your keys, I have provided functions which can be used for printing them.*


----------------


Contribution to the code is highly incentivized and if you have any questions or recommendation, feel free to reach out.

Twitter: @ahsanalikhaled

Have a great day!

Peace!
