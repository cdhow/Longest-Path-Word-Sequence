#pragma once

#include <unordered_map>
#include <vector>
#include <fstream>
#include <list>
#include <algorithm>
#include <random>

typedef std::unordered_map<std::string, std::vector<std::string>> dict_t;

typedef std::pair<std::list<std::string>, std::unordered_map<std::string, bool>> path_t;

/* ------------------ Helper Functions ------------------------- */

int randomNumber(int lower, int upper);

std::string getFrontKey(const std::string& word);

std::string getBackKey(const std::string& word);

/* Returns a random word from a vector. Return empty string if it fails to find a word */
std::string randomWordFromVector(const std::vector<std::string>& word_list,
                                 std::unordered_map<std::string, bool>& visited);


std::string randomWordFromDictionary(dict_t& dict);


/* ------------------ Random Path Generation ------------------------- */


/*
 * Generates a new random path given a dictionary.
 * If find_back_path is true: generated path will follow sequence back_key->front_key.
 * If find_back_path is false: generated path will follow sequence front_key->back_key.
 */
path_t newRandomPath(dict_t& dict, bool find_back_path);

/*
 * Takes a dictionary and a path and generates a new path by
 * backtracking "remove_size" steps.
 * If find_back_path is true: words are removed from front of path and 
 *  random traversal occurs backward from the new front.
 * If find_back_path is false: words are removed from the back of the path and
  * and random traversal occurs forward from the new back.
 */
path_t generateRandomPath(dict_t& dict,
                          const path_t& path_data,
                          size_t remove_size,
                          bool find_back_path);


/* ------------------ Greedy Path Generation ------------------------- */


/*  
 * (Helper Function).
 * Takes a dictionary and a path. Appends a greedy path to the given path.
 * If find_back_path is true: append from front and traverse backward.
 * If find_back_path is false: append from back and traverse forward.
 */
void appendGreedyPath(dict_t& dict, path_t& path_data, bool find_back_path);


/* Returns the greedy path of the dictionary */
path_t newGreedyPath(dict_t& front_dict, dict_t& back_dict);


/*
 * Takes a dictionary and a path and generates a new path by
 * backtracking "remove_size" steps. A random word is chosen for the next node in
 * the path (that matches the sequence constraint) and a greedy traversal is done from there.
 * If find_back_path is true: words are removed from front of path and 
 *  traversal occurs backward from the new front.
 * If find_back_path is false: words are removed from the back of the path and
 *  traversal occurs forward from the new back.
 */
path_t generateGreedyRandomPath(dict_t& dict,
                                const path_t& path_data,
                                size_t remove_size,
                                bool find_back_path);

                        

