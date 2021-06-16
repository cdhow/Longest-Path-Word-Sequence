#include <iostream>
#include <chrono>
#include "../common/PathGeneration.h"
#include "../../include/VariadicTable.h"

/*
 * Hill Climbing algorithm for solving longest sequence problem.
 *
 * Compile with : g++ main.cpp ../common/PathGeneration.cpp -o hill_climbing
 */

/*
 * Table structure for printing results to console.
 */
typedef VariadicTable<size_t, size_t, size_t, double, double, int, std::string> TableFormat;

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
 * Attempts to find the longest sequence using a hill climbing algorithm.
 */
std::list<std::string> solveHillClimbing(dict_t& front_dict, dict_t& back_dict, size_t max_attempts)
{
    // Initial greedy path
    path_t current_path = newGreedyPath(front_dict, back_dict);
    int current_length = current_path.first.size();

    // If the word length is 4, the greedy path is the max solution (back key == front key).
    if (current_path.first.front().size() == 4)
        return current_path.first;

    // Loop until we cannot find a longer path for max consecutive attempts.
    size_t attempt = 0;
    while (attempt <= max_attempts)
    {
        // For each position in the current path.
        for (int i = 0; i <= current_length; i++) {
            path_t new_path;

            // Generate new path from current path by backtracking i steps.
            // Alternate traversal direction randomly (coin toss).
            if (randomNumber(0, 1))
                new_path = generateGreedyRandomPath(front_dict, current_path, i, false);
            else
                new_path = generateGreedyRandomPath(back_dict, current_path, i, true);

            int new_path_length = new_path.first.size();
            if (new_path_length > current_length) {
                // Update current path.
                current_path = new_path;
                current_length = new_path_length;
                attempt = 0;
            } else {
                if (attempt % 100 == 0 && attempt != 0) {
                    std::cout << attempt << std::endl;
                }
                attempt++; // failed to find better path.
            }

        }
        std::cout << "Found." << std::endl;

    }

    return current_path.first;
}


/*
 * Executes Hill climbing for words of size word_len.
 */
void solveMaxSequence(int word_len, TableFormat& vt)
{
    dict_t front_dict, back_dict;
    size_t max_attempts = 1000; // Optionally change parameter.

    auto total_time = Clock::now(); // start total clock.
    int num_words = loadDictionary("../../dictionary.txt", front_dict, back_dict, word_len);

    auto algo_time = Clock::now(); // start algorithm runtime clock.
    auto max_path = solveHillClimbing(front_dict, back_dict, max_attempts);

    auto end = Clock::now(); // end clock.

    double total = std::chrono::duration<double>(end - total_time).count();
    double found = std::chrono::duration<double>(end - algo_time).count();

    // Test the path.
    std::string passed = (testPath(max_path)) ? "True" : "False";

    // Add data to table for printing
    vt.addRow(word_len, num_words, max_path.size(), found, total, (int)max_attempts, passed);

}


int main()
{

    // Stores the data to print to console.
    TableFormat vt({"Word Length", "Num. Words", "Seq. Length", "CPU Found (sec)",
                    "CPU Total (sec)", "Max Attempts", "Correct"});

    std::cout << "Finding solutions for word sizes 4 to 15...\n" << std::endl;
    // Run for words of length 4 to 16
    for (int len=4; len <=15; len++) {
        solveMaxSequence(len, vt);
    }

    // Print results
    vt.print(std::cout);

    std::cout << std::endl;

    return 0;
}