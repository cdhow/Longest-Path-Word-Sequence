# Longest-Path-Word-Sequence

The file dictionary.txt contains one word per line. Subsets of these words
can ordered such that, with the exception of the first word, the second and third letter
of each word is identical to the third last and second last of the preceding word. Words
may only be used once within a sequence.  <br> <br>
This repository contains a collection of algorithms implemented with C++ that find such sequences in the dictionary provided.
This problem is a longest path problem. 

## Data Structures
For the local search algorithms I used two an unordered maps to store the
dictionary data. The maps store a key-value pair where the key is a two letter string and the value is a vector of words.
One instance of the data structure uses a key to store a vector of words whose 2nd and 3rd letters match the key.
The other instance stores a vector of words whose 2nd last and 3rd last letters match the key.
The first instance allows for easy forward traversal in the sequence, whereas the second allows for easy backward traversal in the sequence.
A doubly linked list was used to store the path, this made pushing/popping from both the front and back of the list easy.
To keep track of visited words in a given path, an unordered map was used where the key was the word and the value was a boolean 
stating whether the word had been visited or not. For each path generated, the *path* list and *visited* map are stored in a pair structure.
<br> <br>
The Depth-First-Search solution uses only one unordered map data structure. 
Where the key in the key-value pair is a two letter string and the value is a vector of words whose 2nd and 3rd letters match the key.

## Algorithms
For my implementations I use a deterministic algorithm (DFS) and three random heuristic local search algorithms. I also used a simple greedy algorithm
for comparison. The algorithms I wrote were: Depth-First-Search (DFS), Simmulated Annealing, Hill Climbing with random restart, Greedy with
random backtracking and branching, and simple greedy.
<br><br>
The local search algorithms use common functions to produce random paths, greedy paths, and greedy paths with random starting words.
For the simmulated annealing and hill climbing solutions, I experimented with using a complete random path or greedy path for the initial path.
Starting with a greedy path gave better final results. Each local search solution is implemented in a similar way. 
A max path (initially set to a greedy path) is used to produced a new path on each iteration. The new path is generated by backtracking the max path
a certain number of steps (determining the backtrack size differs for each algorithm). If the new generated path is longer than the max
path, then the max path is updated (not always the case for simulated annealing). Additionly, a coin toss is used (50% probabilty) to determine whether the new path should backtrack and
branch from the front or the back of the current max path.
<br><br>
For all the algorithms, if the word size is of length 4, then the size of the largest vector in the 
unordered map is used. This is due to words of length 4 having the same 2nd and 3rd letters as 2nd last and 3rd last letters.
Therefore all seperate sequences are contained within each vector in the map. This can be illustrated with the following example: 
*map\[barb\] = {tart, bart, sark, narp}* can give sequence *tart->bart->sark->narp*. The sequence continuously follows the letters "ar"
and therefore never breaks from the vector.
<br><br>
Each algorithm is described in detail below.


### Hill Climbing
Hill climbing is a simple heuristic algorithm. It works by continuously finding a better neigbouring path based on a 
current path, and stopping when a better solution cannot be found after a certain number of attempts.
Each iteration of the algorithm loop tries to update the max path by backtracking and choosing a new random word to branch from.
If an improved neighbouring solution cannot be found after a set amount of attempts, then the loop terminates.

### Simulated Anealing 
Simulated annealing (SA) is a probabilistic technique for approximating the global optimum. The impementation is similar 
to that of hill climbing. However, SA uses a probabilty to determine whether a newly generated path should be accepted as the
max path even if it may be shorter than that of the current max path. This probabilty of accepting neighbouring states helps mitigate the possibility
of finding a local optimum. An intial temperature is set that is decreased on each itereation by a defined cooling factor. The probabilty
of accepting a failed solution is a function of the temperature and *delta*, where *delta* is the size difference between the current max
path and the newly generated path. The acceptance probabilty is determined by
*P = e^(-delta/temperature)*.
<br>
This means as our system cools (temperature decreases), the probability of accepting a failed solution decreases. Similiarly, a smaller *delta*
(distance in path lengths), gives a higher acceptance probability than that of a larger *delta*, which means failed paths are more
likely to be accepted the closer they are to the current solution.

### Biased Random Heuristic (Random Greedy)
The random greedy algorithm works by first generating a greedy path. The initial greedy path
is set to the max path. A new path is then generated by backtracking the max path. The number of steps
to backtrack is a random number between 1 and the path length. At every 10 consectutive failed attempts at
generating a longer path, a complete restart occurs, that is, the max path is not used to generate the new path.
If the number of consectutive failed attempts reaches a user defined number (currently set to 1000), the algorithm will
terminate and the solution will be the current max path at time of termination.

### Depth-First-Search
The DFS solution uses a stack data structure to store the search tree. Initially, all words (of size word length)
are pushed to the stack seperated by empty strings. An empty string in the stack indicates where a backtrack should occur in our path.
Therefore, an empty string is always pushed to the stack before a new word is pushed.
An example of how this works is, if a word is popped from the stack and the number of possible next words in the sequence
is 0 (end of current sequence) then an empty string is popped on the next iteration to indicate a backtrack should be triggered,
followed by a possible expansion (or another backtrack).
All visited words are stored in an unordered map to avoid cycling words.
