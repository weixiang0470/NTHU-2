## Compile
- use `make` to compile all `ids.cpp`,`hc.cpp`,`ga.cpp`, `ga2.cpp`
- ex: enter `make`
## IDS
- `./ids <N>`
- 8-queen
    - `./ids 8`
- 10-queen
    - `./ids 10`

## Hill Climbing
- `./hc <N> <max_depth> <yes/no>`
- 8-queen, max_depth = 100, don't restart when no better neighbour
    - `./hc 8 100 no`
- 10-queen, max_depth = 200, random restart when no better neighbour
    - `./hc 10 200 yes`

# Genetic Algorithm
- `./ga <N> <populations_size> <generations> <k>` or `./ga2 <N> <populations_size> <generations> <k>`
    - Default N=8, populations=100, generations=100, k $<$ populations ? k : populations/10
- 8-queen, 1000 populations size, 500 generations, k=50
    - `./ga 8 1000 500 50`
- 10-queen, 500 populations size, 1000 generations, k=100
    - `./ga 10 500 1000 100`