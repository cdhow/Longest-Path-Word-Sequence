#include <iostream>
#include <chrono>
#include "../local_search/common/PathGeneration.h"
#include "../include/VariadicTable.h"

/*
 * Simple greedy algorithm for solving longest sequence problem.
 *
 * Compile with : g++ main.cpp ../local_search/common/PathGeneration.cpp -o greedy
 */

/*
 * Table structure for printing results to console.
 */
typedef VariadicTable<size_t, size_t, size_t, double, double, std::string> TableFormat;

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
 * Executes Greedy for words of size word_len.
 */
void solveMaxSequence(int word_len, TableFormat& vt)
{
    dict_t front_dict, back_dict;

    auto total_time = Clock::now(); // start total clock.
    int num_words = loadDictionary("../dictionary.txt", front_dict, back_dict, word_len);

    auto algo_time = Clock::now(); // start algorithm runtime clock.
    auto max_path = newGreedyPath(front_dict, back_dict).first;

    auto end = Clock::now(); // end clock.

    double total = std::chrono::duration<double>(end - total_time).count();
    double found = std::chrono::duration<double>(end - algo_time).count();

    // Test the path.
    std::string passed = (testPath(max_path)) ? "True" : "False";

    // Add data to table for printing
    vt.addRow(word_len, num_words, max_path.size(), found, total, passed);

}


int main()
{

    // Stores the data to print to console.
    TableFormat vt({"Word Length", "Num. Words", "Seq. Length", "CPU Found (sec)",
                    "CPU Total (sec)", "Correct"});

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