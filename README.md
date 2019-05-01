# p3-ssoo-test-uc3m
## Authors
- Álvaro Galisteo Álvarez
- Álberto Lobato Díaz

## Tests
This are the automated tests used in the third assignments of the subject Sistemas Operativos at UC3M.

## Getting Started
Clone the project:

```bash
git clone https://github.com/SrGMC/p3-ssoo-test-uc3m
```

Add the following assignment files and folders to the cloned directory:

```
include/
lib/
Makefile
```

The directory tree should look like this:

```
.
├── Makefile
├── README.md
├── batch_files
│   ├── batch_inv.txt
│   ├── batch_op1.txt
│   ├── batch_op10.txt
│   ├── batch_op2.txt
│   ├── batch_op3.txt
│   ├── batch_op4.txt
│   ├── batch_op7.txt
│   └── batch_op9.txt
├── concurrent_market.c
├── include
│   ├── concurrency_layer.h
│   ├── operations_queue.h
│   ├── parser.h
│   └── stock_market_lib.h
├── lib
│   ├── concurrency_layer.c
│   ├── concurrency_layer.o
│   ├── operations_queue.c
│   ├── operations_queue.o
│   ├── parser.c
│   ├── parser.o
│   ├── stock_market_lib.c
│   └── stock_market_lib.o
├── stocks.txt
└── test.sh

3 directories, 25 files
```

Then execute the automated tests with:

```bash
chmod a+x test.sh
./test.sh
```

## `test.sh` usage
```
./test.sh [n]
```
- `n` - Optional. Run each test n times before showing the execution result
