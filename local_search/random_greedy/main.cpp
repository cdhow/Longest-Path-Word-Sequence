#include <iostream>
#include <chrono>
#include "../common/PathGeneration.h"
#include "../../include/VariadicTable.h"

/*
 * Greedy Algorithm with random backtracking/restart for solving longest sequence problem.
 *
 * Compile with : g++ main.cpp ../common/PathGeneration.cpp -o random_greedy
 */

/*
 * Table structure for printing results to console.
 */
typedef VariadicTable<size_t, size_t, size_t, double, double, size_t, std::string> TableFormat;

/*
 * Data structure where key is a two letter string and
 * value is a vector of words whose 2nd and 3rd letters
 * match the key.
 */
typedef std::unordered_map<std::string, std::vector<std::string>> dict_t;

/* Chrono clock */
typedef std::chrono::steady_clock Clock;


/*
 * Loads dictionary word of size word_len into the dict_t data structures.
 * Return number of words of size word_len.
 */
int loadDictionary(const std::string& filepath,
                  dict_t& front_dict,
                  dict_t& back_dict,
                  int word_len)
{
    std::ifstream infile(filepath);

    if(infile.fail()){
       std::cerr << "Incorrect filepath\n";
       std::terminate;
    }

    int num = 0;
    std::string word;
    while (infile >> word) {
        if (word_len == word.size()) {
            front_dict[getFrontKey(word)].push_back(word);
            back_dict[getBackKey(word)].push_back(word);
            num++;
        }
    }
    return num;
}

/*
 * Returns true if the path has no cycles and matches the sequence
 * of previous word's 2nd last and 3rd last matching current word's
 * 2nd and 3rd letters.
 */
bool testPath(std::list<std::string>& path)
{
    std::unordered_map<std::string, bool> visited;
    std::string prev_word;
    for (const std::string& word : path) {
        if (visited.find(word) != visited.end()) {
            return false;
        }
        visited[word] = true;

        if (!prev_word.empty()
            && getBackKey(prev_word) != getFrontKey(word)) {
            return false;
        }
    }
    return true;
}

/*
 * Attempts to find the longest sequence using a greedy algorithm with random backtracking/restart.
 */
std::list<std::string> solveRandomGreedy(dict_t& front_dict, dict_t& back_dict, size_t max_attempts)
{
    // Initial greedy path
    path_t max_path = newGreedyPath(front_dict, back_dict);
    int max_length = max_path.first.size();

    // If the word length is 4, the greedy path is the max solution (back key == front key)
    if (max_path.first.front().size() == 4)
        return max_path.first;

    // Loop until we cannot find a longer path, after trying for max consecutive attempts.
    int attempt = 0;
    while (attempt < max_attempts)
    {
        path_t new_path;

        // Choose a random backtrack length OR every 10th failed attempt restart.
        int backtrack;
        if (attempt % 10 == 0)
            backtrack = max_length; // when backtrack equals path length, path generator does complete restart.
        else
            backtrack = randomNumber(1, max_length-1);

        // Generate new path from max path.
        // Alternate traversal direction randomly (coin toss).
        if (randomNumber(0, 1))
            new_path = generateGreedyRandomPath(front_dict, max_path, backtrack, false); 
        else
            new_path = generateGreedyRandomPath(back_dict, max_path, backtrack, true);

        int new_path_length = new_path.first.size();

        if (new_path_length > max_length) {
            // Update max path.
            max_path = new_path;
            max_length = new_path_length;
            attempt = 0;
        } else {
            if (attempt % 100 == 0 && attempt != 0)
                std::cout << "attempt: " << attempt << '\n';
            attempt++;
        }
    }

    std::cout << "Found." << std::endl;

    return max_path.first;
}



/*
 * Executes Simulated Annealing for words of size word_len.
 */
void solveMaxSequence(int word_len, TableFormat& vt)
{
    dict_t front_dict, back_dict;
    size_t max_attempts = 1000; // Optionally change parameter.

    auto total_time = Clock::now(); // start total clock.
    int num_words = loadDictionary("../../dictionary.txt", front_dict, back_dict, word_len);

    auto algo_time = Clock::now(); // start algorithm runtime clock.
    auto max_path = solveRandomGreedy(front_dict, back_dict, max_attempts);

    auto end = Clock::now(); // end clock.

    double total = std::chrono::duration<double>(end - total_time).count();
    double found = std::chrono::duration<double>(end - algo_time).count();

    // Test the path.
    std::string passed = (testPath(max_path)) ? "True" : "False";

    // Add data to table for printing
    vt.addRow(word_len, num_words, max_path.size(), found, total, max_attempts, passed);

}


int main()
{

    // Stores the data to print to console.
    TableFormat vt({"Word Length", "Num. Words", "Seq. Length", "CPU Found (sec)",
                    "CPU Total (sec)", "Max Attempts", "Correct"});

    std::cout << "Finding solutions for word sizes 4 to 15...\n" << std::endl;
    // Run for words of length 4 to 15
    for (int len=4; len <=15; len++) {
        solveMaxSequence(len, vt);
    }

    // Print results
    vt.print(std::cout);

    std::cout << std::endl;

    return 0;
}