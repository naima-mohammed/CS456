#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <map>

using namespace std;

struct PerformanceMetrics {
    double execution_time_ms;
    long long backtracking_count;
    long long nodes_visited;
    int solutions_found;

    PerformanceMetrics() : execution_time_ms(0), backtracking_count(0), nodes_visited(0), solutions_found(0) {}
};

class CrosswordPuzzle {
public:
    vector<string> grid;
    vector<string> words;
    int n;

    CrosswordPuzzle(const vector<string>& initial_grid, const vector<string>& word_list)
        : grid(initial_grid), words(word_list) {
        n = initial_grid.size();
    }

    void print_grid(ostream& out) const {
        for (const auto& row : grid) {
            out << "  " << row << endl;
        }
    }

    bool is_consistent_horizontal(int r, int c, const string& word) const {
        int len = word.length();
        if (c + len > n) return false;
        for (int i = 0; i < len; ++i) {
            if (grid[r][c + i] == '*') return false;
            if (grid[r][c + i] != '#' && grid[r][c + i] != word[i]) return false;
        }
        return true;
    }

    bool is_consistent_vertical(int r, int c, const string& word) const {
        int len = word.length();
        if (r + len > n) return false;
        for (int i = 0; i < len; ++i) {
            if (grid[r + i][c] == '*') return false;
            if (grid[r + i][c] != '#' && grid[r + i][c] != word[i]) return false;
        }
        return true;
    }

    vector<string> place_word_horizontal(int r, int c, const string& word) const {
        vector<string> new_grid = grid;
        for (int i = 0; i < (int)word.length(); ++i) new_grid[r][c + i] = word[i];
        return new_grid;
    }

    vector<string> place_word_vertical(int r, int c, const string& word) const {
        vector<string> new_grid = grid;
        for (int i = 0; i < (int)word.length(); ++i) new_grid[r + i][c] = word[i];
        return new_grid;
    }
};

class BacktrackingSolver {
public:
    CrosswordPuzzle puzzle;
    vector<bool> used_words;
    PerformanceMetrics metrics;
    ostream& out;

    BacktrackingSolver(const CrosswordPuzzle& initial_puzzle, ostream& output_stream)
        : puzzle(initial_puzzle), used_words(initial_puzzle.words.size(), false), out(output_stream) {}

    void solve() {
        auto start_time = std::chrono::high_resolution_clock::now();
        backtrack(0);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end_time - start_time;
        metrics.execution_time_ms = duration.count();
    }

    void print_report() const {
        out << "------------------------------------------" << endl;
        out << "Performance Report:" << endl;
        out << "  - Execution Time: " << fixed << setprecision(4) << metrics.execution_time_ms << " ms" << endl;
        out << "  - Backtracking Count: " << metrics.backtracking_count << endl;
        out << "  - Nodes Visited: " << metrics.nodes_visited << endl;
        out << "  - Solutions Found: " << metrics.solutions_found << endl;
        if (metrics.solutions_found == 0) {
            out << "  - STATUS: NO SOLUTION FOUND" << endl;
        }
        out << "------------------------------------------" << endl;
    }

private:
    int count_possible_positions(const vector<string>& current_grid, const string& word) const {
        int count = 0;
        int word_len = word.length();
        int n = puzzle.n;

        for (int r = 0; r < n; ++r) {
            for (int c = 0; c <= n - word_len; ++c) {
                bool consistent = true;
                for (int i = 0; i < word_len; ++i) {
                    if (current_grid[r][c + i] == '*' || (current_grid[r][c + i] != '#' && current_grid[r][c + i] != word[i])) {
                        consistent = false;
                        break;
                    }
                }
                if (consistent) count++;
            }
        }

        for (int c = 0; c < n; ++c) {
            for (int r = 0; r <= n - word_len; ++r) {
                bool consistent = true;
                for (int i = 0; i < word_len; ++i) {
                    if (current_grid[r + i][c] == '*' || (current_grid[r + i][c] != '#' && current_grid[r + i][c] != word[i])) {
                        consistent = false;
                        break;
                    }
                }
                if (consistent) count++;
            }
        }
        return count;
    }

    int select_unassigned_variable() {
        int best_word_index = -1;
        int min_possible_positions = std::numeric_limits<int>::max();

        for (int i = 0; i < (int)puzzle.words.size(); ++i) {
            if (!used_words[i]) {
                int possible_positions = count_possible_positions(puzzle.grid, puzzle.words[i]);
                if (possible_positions < min_possible_positions) {
                    min_possible_positions = possible_positions;
                    best_word_index = i;
                }
            }
        }
        return best_word_index;
    }

    void backtrack(int placed_words) {
        metrics.nodes_visited++;

        if (placed_words == (int)puzzle.words.size()) {
            metrics.solutions_found++;
            if (metrics.solutions_found <= 1) {
                out << "Solution Found!" << endl;
                puzzle.print_grid(out);
            }
            return;
        }

        int word_index = select_unassigned_variable();
        if (word_index == -1) {
            metrics.backtracking_count++;
            return;
        }

        string current_word = puzzle.words[word_index];
        used_words[word_index] = true;

        for (int r = 0; r < puzzle.n; ++r) {
            for (int c = 0; c <= puzzle.n - (int)current_word.length(); ++c) {
                if (puzzle.is_consistent_horizontal(r, c, current_word)) {
                    vector<string> original_grid = puzzle.grid;
                    puzzle.grid = puzzle.place_word_horizontal(r, c, current_word);
                    backtrack(placed_words + 1);
                    puzzle.grid = original_grid;
                }
            }
        }

        for (int c = 0; c < puzzle.n; ++c) {
            for (int r = 0; r <= puzzle.n - (int)current_word.length(); ++r) {
                if (puzzle.is_consistent_vertical(r, c, current_word)) {
                    vector<string> original_grid = puzzle.grid;
                    puzzle.grid = puzzle.place_word_vertical(r, c, current_word);
                    backtrack(placed_words + 1);
                    puzzle.grid = original_grid;
                }
            }
        }

        used_words[word_index] = false;
    }
};

struct PuzzleState {
    string name;
    vector<string> grid;
};

map<string, vector<string>> load_words(const string& filename) {
    map<string, vector<string>> word_bank;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string state_name = line.substr(0, colon_pos);
            string words_part = line.substr(colon_pos + 1);
            stringstream ss(words_part);
            string word;
            vector<string> words;
            while (ss >> word) {
                words.push_back(word);
            }
            word_bank[state_name] = words;
        }
    }
    return word_bank;
}

int main() {
    map<string, vector<string>> word_bank = load_words("words_bank.txt");
    ofstream outFile("output_from_file.txt");

    vector<PuzzleState> states = {
        {"State 1", {"*#********","*#********","*#****#***","*##***##**","*#****#***","*#****#***","*#****#***","*#*######*","*#********","***#######"}},
        {"State 2", {"###*","#***","#***","****"}},
        {"State 3", {"#####","#****","#****","#****","#****"}},
        {"State 4", {"###","#*#","###"}},
        {"State 5", {"####","####","####","####"}},
        {"State 6", {"######","*#****","*#****","*#****","*#****","*#****"}},
        {"State 7", {"########","########","########","########","########","########","########","########"}},
        {"State 8", {"###","*#*","###"}},
        {"State 9", {"####","****","****","****"}},
        {"State 10", {"#*****","#*****","######","#*****","#*****","#*****"}},
        {"State 11", {"**********","*########*","*#******#*","*#******#*","*#******#*","*#******#*","*#******#*","*#******#*","*########*","**********"}},
        {"State 12", {"#*#","***","#*#"}},
        {"State 13", {"############","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","############"}},
        {"State 14", {"#####","*#*#*","#####","*#*#*","#####"}},
        {"State 15", {"####","*#**","*#**","*#**"}}
    };

    cout << "Reading words from 'words_bank.txt' and solving 15 states..." << endl;

    for (const auto& state : states) {
        if (word_bank.find(state.name) == word_bank.end()) {
            cout << "Skipping " << state.name << " (No words found in bank)" << endl;
            continue;
        }

        outFile << "\n==========================================" << endl;
        outFile << "RUNNING: " << state.name << endl;
        outFile << "Words: ";
        for (const auto& w : word_bank[state.name]) outFile << w << " ";
        outFile << "\n==========================================" << endl;
        
        CrosswordPuzzle puzzle(state.grid, word_bank[state.name]);
        BacktrackingSolver solver(puzzle, outFile);
        
        solver.solve();
        solver.print_report();
        cout << "Completed: " << state.name << endl;
    }

    outFile.close();
    cout << "\nResults saved in 'output_from_file.txt'." << endl;

    return 0;
}