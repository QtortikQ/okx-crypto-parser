# okx-crypto-parser

A small GUI application in C++ that shows public data from OKX crypto exchange - www.okx.com.
API reference is here https://www.okx.com/docs-v5/en/.

Application shows a table of futures instruments for a given crypto pair, for example - BTC-USDT,
this will be the instruments list.
You can also take any other instrument listed in `instruments?instType=FUTURES`.

Rows are clickable and show another table - Order Book.

All data from those two tables dynamic and updated in real time.

- Only public data is used(no need to create an account)
- The Best BID columns (and Best ASK, respectively) in the instrument list show the top quantities/prices in the Order Book.
- Shows up to 50 price levels per side in the Order Book
- The BID prices are growing in up direction, and ASK prices are growing in down direction.

Project done in VSCode + Qt extension.
Cmake + Ninja + MinGW(g++/gcc) used for building.
