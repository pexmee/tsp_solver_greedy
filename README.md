# Greedy TSP solver
A simple TSP solver made in C++ using a greedy approach.

The algorithm works as follows:

1. An arbitrary node is chosen as the current node and is marked as visited and removed
from the dataset. All other nodes are considered unvisited.
2. The distance to all unvisited nodes is calculated.
3. The closest node is chosen as the current node and is marked as visited.
4. Iteratively execute step 2 and 3 until all nodes have been visited.
5. The total distance of the cycle is calculated and compared to any previous result.
The lowest result is recorded.
6. Iteratively execute step 1-5 until the dataset has been exhausted.

The program will output the resulting cycles in json, however not prettified.
