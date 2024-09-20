# PokerBot
Poker bot (Work in Progress)


## Overview
Implementation of counter factual regret minimization with public chance sampling [[1]](#1) on the game of texas holdem. Pseudocode from [[2]](#2). The goal of the algorithm is to approximate a nash equilibirum strategy to the 2 person game of texas no limit holdem. Due to the incredibly large number of possible states, exactly simulating the full game is intractable. Several abstractions were used to reduce the number of game states.


## To Run

Clone the repository, then run the following.

```
cd src
make
./cfrpoker
```

The algorithm will start running. Upon completion, we will save the computed strategy in a file called strat.txt. The node name is the history that defines the node (a sequence of player actions and chance buckets). The strategy probabilites are given at each node.

## Next Steps

Currently the project still is a poor approximation. Better parallelize and optimize code to allow for more iterations. Load parts of memory to hard drive to allow for more buckets.


## References
<a id="1">[1]</a> 
DJohanson, Michael, et al. "Efficient Nash equilibrium approximation through Monte Carlo counterfactual regret minimization." Aamas. 2012.
<a id="2">[2]</a> 
Zinkevich, Martin, et al. "Regret minimization in games with incomplete information." Advances in neural information processing systems 20 (2007).