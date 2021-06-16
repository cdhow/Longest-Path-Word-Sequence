#include "PathGeneration.h"

/* Random number generator */
std::random_device random_device;
std::mt19937 engine{random_device()};

/* ------------------ Helper Functions ------------------------- */


int randomNumber(int lower, int upper)
{
    std::uniform_int_distribution<int> dist(lower, upper);
    return dist(engine);
}

std::string getFrontKey(const std::string& word)
{
    return word.substr(1, 2);
}

std::string getBackKey(const std::string& word)
{
    return word.substr(word.size()-3, 2);
}


std::string randomWordFromVector(const std::vector<std::string>& word_list,
                               std::unordered_map<std::string, bool>& visited)
{
    if (word_list.empty())
        return "";

    int idx = randomNumber(0, (int)word_list.size()-1);

    // Try 10 times to find an unvisited word.
    // (Initially I was copying all unvisited, shuffling vector, and selecting,
    //  but I found this method of selection gave better results).

    int attempts = 0;
    while (visited.find(word_list[idx]) != visited.end()) {
        idx = randomNumber(0, (int)word_list.size()-1);
        if (attempts > 10)
            return "";
        attempts++;
    }
    return word_list[idx];
}


std::string randomWordFromDictionary(dict_t& dict)
{
    auto random_it = dict.begin();
    do {
        random_it = std::next(dict.begin(), randomNumber(0, dict.size()-1));
    } while (random_it->second.empty()); // Until we find an entry with a non empty vector.

    // Choose random word.
    auto& v = random_it->second;
    return v[randomNumber(0, v.size()-1)];
}


/* ------------------ Random Path Generation ------------------------- */


path_t newRandomPath(dict_t& dict, bool find_back_path)
{
    std::list<std::string> path;
    std::unordered_map<std::string, bool> visited;

    // Choose a random start word.
    std::string word = randomWordFromDictionary(dict);
    path.push_back(word);
    visited[word] = true;

    // If find_back_path is false, then traverse forward,
    // else traverse backward.
    while (true) {
        const std::string& key = (find_back_path) ? getFrontKey(word) : getBackKey(word);

        // Next word.
        word = randomWordFromVector(dict[key], visited);

        if (word.empty()) {
            return std::make_pair(path, visited); // end of sequence.
        }

        if (find_back_path) path.push_front(word);
        else path.push_back(word);

        visited[word] = true;
    }

}

path_t generateRandomPath(dict_t& dict, 
                          const path_t& path_data,
                          size_t remove_size,
                          bool find_back_path)
{
    auto& current_path = path_data.first;
    auto& current_visited = path_data.second;

    // If the backup length is equal to the current path size,
    // do a complete restart.
    if (current_path.size() == remove_size) {
        return newRandomPath(dict, find_back_path);
    }

    std::list<std::string> new_path;
    std::unordered_map<std::string, bool> new_visited;

    // Otherwise backup remove_size steps in the sequence and branch from there.
    new_path = current_path;
    new_visited = current_visited;
    for (size_t i = 0; i < remove_size; i++) {
        if (find_back_path) {
            // traversing from front and backward.
            new_visited.erase(new_path.front());
            new_path.pop_front();
        } else {
            // traversing from back and forward.
            new_visited.erase(new_path.back());
            new_path.pop_back();
        }

    }

    // Random traverse a new path
    std::string word = (find_back_path) ? new_path.front() : new_path.back();
    while (true)
    {
        std::string key = (find_back_path) ? getBackKey(word) : getFrontKey(word);
        word = randomWordFromVector(dict[key], new_visited);

        if (word.empty()) // End of sequence
            return std::make_pair(new_path, new_visited); // Return new path

        if(find_back_path) new_path.push_front(word);
        else new_path.push_back(word);

        new_visited[word] = true;
    }
}


/* ------------------ Greedy Path Generation ------------------------- */


void appendGreedyPath(dict_t& dict, path_t& path_data, bool find_back_path)
{
    auto& current_path = path_data.first;
    auto& current_visited = path_data.second;

    // run greedy starting from last word in path (if traversing forward),
    // or from first word in path (if traversing backward).
    std::string word = (find_back_path) ? current_path.front() : current_path.back();

    // Run greedy
    while (true) {
        std::string key = (find_back_path) ? getFrontKey(word) : getBackKey(word);

        auto& word_list = dict[key];

        // Get word with max hanging list.
        size_t max = 0;
        std::string next_word;
        for (const std::string& w : word_list) {
            std::string possible_key = (find_back_path) ? getFrontKey(w) : getBackKey(w);
       
            size_t word_list_size = dict[possible_key].size();

            if (word_list_size > max && current_visited.find(w) == current_visited.end()) {
                max = word_list_size;
                next_word = w;
            }
        }

        // End of sequence, path_data has greedy path appended.
        if (max == 0) {
            return;
        }

        if (find_back_path) current_path.push_front(next_word);
        else current_path.push_back(next_word);

        current_visited[next_word] = true;
        
        word = next_word;
    }
}


path_t newGreedyPath(dict_t& front_dict, dict_t& back_dict)
{
    std::string word;
    // Get the front key with the max list
    auto it = front_dict.begin();
    auto max_it = it;
    while (++it != front_dict.end()) {
        if (it->second.size() > max_it->second.size()) {
            max_it = it;
        }
    }
    std::string front_key = max_it->first;

    // If the word length is 4, the front key == the back key.
    // Therefore max sequence is contained a dict[key].
    auto& v = front_dict[front_key];
    if (v.front().size() == 4) {
        path_t p;
        p.first = std::list<std::string>(v.begin(), v.end());
        return p;
    }

    // use the front_key in the back_dict to get a word whose back key
    // is equal to front_key. This is the start word.
    word = back_dict[front_key][0];

    path_t path_data;
    path_data.first.push_back(word);
    path_data.second[word] = true;

    // Append a greedy path.
    appendGreedyPath(front_dict, path_data, false);

    return path_data;
}

path_t generateGreedyRandomPath(dict_t& dict,
                                const path_t& path_data,
                                size_t remove_size,
                                bool find_back_path)
{
    auto& current_path = path_data.first;
    auto& current_visited = path_data.second;

    std::list<std::string> new_path;
    std::unordered_map<std::string, bool> new_visited;

    // If the backup length is equal to the current path size,
    // do a complete restart, starting at a random word in the dictionary.
    if (current_path.size() == remove_size) {
        std::string word = randomWordFromDictionary(dict);

        if (find_back_path) new_path.push_front(word);
        else new_path.push_back(word);

        new_visited[word] = true;

        path_t new_path_data = std::make_pair(new_path, new_visited);
        appendGreedyPath(dict, new_path_data, find_back_path);
        return new_path_data;
    }

    // Otherwise backup remove_size steps in the sequence and branch from there.
    new_path = current_path;
    new_visited = current_visited;
    for (int i = 0; i < remove_size; i++) {
        if (find_back_path) {
            new_visited.erase(new_path.front());
            new_path.pop_front();
        } else {
            new_visited.erase(new_path.back());
            new_path.pop_back();
        }
    }

    std::string word = (find_back_path) ? new_path.front() : new_path.back();
    std::string key = (find_back_path) ? getFrontKey(word) : getBackKey(word);

    // Random starting word
    word = randomWordFromVector(dict[key], new_visited);
    if (word.empty())
        return std::make_pair(new_path, new_visited); // end of sequence.

    if (find_back_path) new_path.push_front(word);
    else new_path.push_back(word);

    new_visited[word] = true;

    path_t p = std::make_pair(new_path, new_visited);
    appendGreedyPath(dict, p, find_back_path);

    return p;
}