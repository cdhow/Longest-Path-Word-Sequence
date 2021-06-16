#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <stack>
#include <chrono>
#include "../include/VariadicTable.h"

/*
 * Depth-First-Search Algorithm to solve longest sequence problem.
 * Compile with: g++ main.cpp -o dfs
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



std::string getFrontKey(const std::string& word) {
    return word.substr(1, 2);
}

std::string getBackKey(const std::string& word) {
    return word.substr(word.size()-3, 2);
}

/*
 * Loads dictionary word of size word_len into the dict_t data structure.
 * A word_list vector is also updated.
 */
void loadDictionary(const std::string& filepath,
                    dict_t& dict,
                    std::vector<std::string>& word_list,
                    int word_len)
{
    std::ifstream infile(filepath);

    std::string word;
    while (infile >> word) {
        if (word_len == word.size()) {
            dict[getFrontKey(word)].push_back(word);
            word_list.push_back(word);
        }
    }
}

/*
 * Returns true if the path has no cycles and matches the sequence
 * of previous word's 2nd last and 3rd last matching current word's
 * 2nd and 3rd letters.
 */
bool testPath(std::vector<std::string>& path)
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
 * Takes a stack and appends a viable sequence of words
 * from a starting word that have not been visited.
 */
void appendSequence(std::stack<std::string>& s,
                    const std::string& start_word,
                    std::unordered_map<std::string, bool>& visited,
                    dict_t& dict)
{
    const std::string& key = getBackKey(start_word);

    for (const std::string& next_word : dict[key]) {
        if (visited.find(next_word) == visited.end()) {
            // If there does not exist a sequence after next_word,
            // the empty string will trigger a backtrack when popped.
            s.push("");
            s.push(next_word);
        }
    }
}

/*
 * Performs a depth first search on all words in the dictionary.
 * Following the sequence previous 2nd and 3rd letters match current
 * 2nd last and 3rd last letters.
 * @return maximum sequence found.
 */
std::vector<std::string> dfs(dict_t& dict, std::vector<std::string>& word_list)
{
    std::stack<std::string> s;
    std::vector<std::string> max_path;
    std::vector<std::string> current_path;
    std::unordered_map<std::string, bool> visited;
    int max_path_length = 0;

    // Add all nodes to initial level of DFS.
    for (const auto& w : word_list) {
        s.push(""); // empty string triggers a backtrack.
        s.push(w);
    }

    // Run DFS
    while (!s.empty())
    {
        const std::string& word = s.top();
        s.pop();

        if (word.empty()) {
            // If no words were added to the stack on the previous iteration
            // then backtrack.
            current_path.pop_back();
        } else {
            // Otherwise add the word to the current path
            // and branch from there.
            current_path.push_back(word);
            visited[word] = true;

            // Update current path if we have expanded further than the current max.
            if (current_path.size() > max_path_length) {
                max_path = current_path;
                max_path_length = max_path.size();
            }

            // Expand DFS.
            appendSequence(s, word, visited, dict);
        }
    }
    return max_path;
}

/*
 * Return the largest value vector in the dictionary
 */
std::vector<std::string> maxVectorInDictionary(dict_t& dict)
{
    auto it = dict.begin();
    auto max_it = it;
    while (++it != dict.end()) {
        if (it->second.size() > max_it->second.size()) {
            max_it = it;
        }
    }
    return max_it->second;
}

/*
 * Executes DFS for words of size word_len.
 */
void solveMaxSequence(int word_len, TableFormat& vt)
{
    std::vector<std::string> word_list;
    dict_t dict;

    auto total_time = Clock::now(); // start total clock.
    loadDictionary("../dictionary.txt", dict, word_list, word_len);

    auto algo_time = Clock::now(); // start algorithm runtime clock.
    std::vector<std::string> max_path;
    if (word_len == 4) {
        max_path = maxVectorInDictionary(dict);
    } else {
        max_path = dfs(dict, word_list);
    }

    auto end = Clock::now(); // end clock.

    double total = std::chrono::duration<double>(end - total_time).count();
    double found = std::chrono::duration<double>(end - algo_time).count();

    // Test the path.
    std::string passed = (testPath(max_path)) ? "True" : "False";

    std::cout << "Found for word length size: " << word_len << std::endl;

    // Add data to table for printing
    vt.addRow(word_len, word_list.size(), max_path.size(), found, total, passed);

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

    return 0;
}
