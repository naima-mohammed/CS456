#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <limits>
#include <chrono>
#include <fstream>
#include <iomanip>
using namespace std;

struct PerformanceMetrics {
    double execution_time_ms;
    long long backtracking_count;
    long long nodes_visited;
    int solutions_found;

    PerformanceMetrics() 
    : execution_time_ms(0), backtracking_count(0), nodes_visited(0), solutions_found(0) {}
};

// تمثل شبكة الكلمات المتقاطعة وقائمة الكلمات المطلوب وضعها
class CrosswordPuzzle {
public:
    vector<string> grid;
    vector<string> words;
    int n;

    CrosswordPuzzle(const vector<string>& initial_grid, const vector<string>& word_list)
        : grid(initial_grid), words(word_list)
         { n = initial_grid.size(); }
    
    // طباعة الشبكة الحالية بعد وضع الكلمات
    void print_grid(ostream& out) const
     {
        for (const auto& row : grid) 
        {out << "  " << row << endl;}
    }

    // التحقق من إمكانية وضع الكلمة أفقياً
    // دون مخالفة القيود أو تجاوز حدود الشبكة

    bool is_consistent_horizontal(int r, int c, const string& word) const {
        int len = word.length();
        if (c + len > n) return false;
        for (int i = 0; i < len; ++i) {
            if (grid[r][c + i] == '*') return false;
            if (grid[r][c + i] != '#' && grid[r][c + i] != word[i]) return false;
        }
        return true;
    }


    // التحقق من إمكانية وضع الكلمة رأسياً
    // مع التأكد من توافق الحروف وعدم تجاوز الحدود
    bool is_consistent_vertical(int r, int c, const string& word) const {
        int len = word.length();
        if (r + len > n) return false;
        for (int i = 0; i < len; ++i) {
            if (grid[r + i][c] == '*') return false;
            if (grid[r + i][c] != '#' && grid[r + i][c] != word[i]) return false;
        }
        return true;
    }


      // إنشاء نسخة جديدة من الشبكة ووضع الكلمة أفقياً
    vector<string> place_word_horizontal(int r, int c, const string& word) const {
        vector<string> new_grid = grid;
        for (int i = 0; i < (int)word.length(); ++i) new_grid[r][c + i] = word[i];
        return new_grid;
    }

    // إنشاء نسخة جديدة من الشبكة ووضع الكلمة رأسياً
    vector<string> place_word_vertical(int r, int c, const string& word) const {
        vector<string> new_grid = grid;
        for (int i = 0; i < (int)word.length(); ++i) new_grid[r + i][c] = word[i];
        return new_grid;
    }


    // حساب عدد الخلايا الفارغة (#)
// لمعرفة مدى اقترابنا من الحل الكامل
    int count_remaining_hashes() const {
        int count = 0;
        for (const auto& row : grid) {
            for (char c : row) if (c == '#') count++;
        }
        return count;
    }
};

class BacktrackingSolver {
public:
    CrosswordPuzzle puzzle;
    vector<bool> used_words;
    PerformanceMetrics metrics;
    ostream& out;
    
    // لتخزين أفضل حالة تم الوصول إليها
    vector<string> best_grid;
    int min_hashes;

    BacktrackingSolver(const CrosswordPuzzle& initial_puzzle, ostream& output_stream)
        : puzzle(initial_puzzle), used_words(initial_puzzle.words.size(), false), out(output_stream) 
        // حفظ أفضل حالة وصلت إليها الخوارزمية
        // حتى لو لم يتم العثور على حل كامل
        {
        best_grid = initial_puzzle.grid;   
        min_hashes = initial_puzzle.count_remaining_hashes();
    }

    void solve() {
        auto start_time = chrono::high_resolution_clock::now();
        backtrack(0);
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double,milli> duration = end_time - start_time;
        metrics.execution_time_ms = duration.count();
    }

    void print_report() {
        out << "  - Execution Time: " << fixed << setprecision(4) << metrics.execution_time_ms << " ms" << endl;
        out << "  - Backtracking Count: " << metrics.backtracking_count << endl;
        out << "  - Nodes Visited: " << metrics.nodes_visited << endl;
        out << "  - Solutions Found: " << metrics.solutions_found << endl;
        
        if (metrics.solutions_found == 0) {
            out << "  - STATUS: NO COMPLETE SOLUTION FOUND" << endl;
            for (const auto& row : best_grid) out << "  " << row << endl;
        }
    
    }

private:
    int count_possible_positions(const vector<string>& current_grid, const string& word) const {
        int count = 0;
        int word_len = word.length();
        int n = puzzle.n;

        // حساب عدد المواضع الممكنة لوضع كلمة معينة
        // يستخدم بواسطة MRV لاختيار الكلمة الأنسب

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
            for (int r = 0; r <= n - word_len; ++r) 
            {   bool consistent = true;
                for (int i = 0; i < word_len; ++i) 
                {
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

    // تطبيق استراتيجية MRV
// اختيار الكلمة التي تمتلك أقل عدد من المواضع الممكنة
    int select_unassigned_variable()
     {
        int best_word_index = -1;
        int min_possible_positions = numeric_limits<int>::max();

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

    // الدالة الأساسية لخوارزمية Backtracking
// تقوم بتجربة جميع الاحتمالات والرجوع عند الفشل

    void backtrack(int placed_words) {
        metrics.nodes_visited++;

        int current_hashes = puzzle.count_remaining_hashes();
        
        // تحديث أفضل حالة تم الوصول إليها حتى الآن
        if (current_hashes < min_hashes) {
            min_hashes = current_hashes;
            best_grid = puzzle.grid;
        }

        // شرط الحل الكامل
        if (current_hashes == 0) {
            metrics.solutions_found++;
            if (metrics.solutions_found <= 3) {
                out << "Solution " << metrics.solutions_found << " Found!" << endl;
                puzzle.print_grid(out);
            }
            return;
        }

        if (placed_words == (int)puzzle.words.size()) {
            return;
        }
         // اختيار الكلمة التالية باستخدام MRV
        int word_index = select_unassigned_variable();
        if (word_index == -1) {
            metrics.backtracking_count++;
            return;
        }

        string current_word = puzzle.words[word_index];
        used_words[word_index] = true; // تعليم الكلمة بأنها استخدمت

        bool found_any_placement = false;
         // تجربة جميع المواضع الأفقية الممكنة للكلمة
        for (int r = 0; r < puzzle.n; ++r) {
            for (int c = 0; c <= puzzle.n - (int)current_word.length(); ++c) {
                if (puzzle.is_consistent_horizontal(r, c, current_word)) {
                    found_any_placement = true;
                    vector<string> original_grid = puzzle.grid;
                    puzzle.grid = puzzle.place_word_horizontal(r, c, current_word);
                    backtrack(placed_words + 1);
                    puzzle.grid = original_grid;
                    if (metrics.solutions_found >= 3) { used_words[word_index] = false; return; }
                }
            }
        }
           // تجربة جميع المواضع الرأسية الممكنة للكلمة
        for (int c = 0; c < puzzle.n; ++c) {
            for (int r = 0; r <= puzzle.n - (int)current_word.length(); ++r) {
                if (puzzle.is_consistent_vertical(r, c, current_word)) {
                    found_any_placement = true;
                    vector<string> original_grid = puzzle.grid;
                    puzzle.grid = puzzle.place_word_vertical(r, c, current_word);
                    backtrack(placed_words + 1);
                    puzzle.grid = original_grid;
                    // التراجع (Backtracking)
                    // واستعادة الشبكة قبل المحاولة التالية
                    if (metrics.solutions_found >= 3) { used_words[word_index] = false; return; }
                }
            }
        }

        if (!found_any_placement) {
            metrics.backtracking_count++; // زيادة عداد عمليات التراجع عند عدم إيجاد موضع مناسب
        }

        used_words[word_index] = false;
        // إلغاء استخدام الكلمة للسماح بتجربتها في مسار آخر
    }
};
// يمثل حالة اختبار واحدة
// تحتوي على الشبكة والكلمات الخاصة بها
struct PuzzleState {
    string name;
    vector<string> grid;
    vector<string> words;
};

int main() {
    ofstream outFile("output.txt");
    vector<PuzzleState> states = {
        {"State 1", {"*#********","*#********","*#****#***","*##***##**","*#****#***","*#****#***","*#****#***","*#*######*","*#********","***#######"}, {"PUNJAB", "JHARKHAND", "MIZORAM", "MUMBAI"}},
        {"State 2", {"###","#*#","###"}, {"TUB","BAT", "BIT", "TAB"}},
        {"State 3", {"####","####","####","####"}, {"CODE", "DATA", "NODE", "FILE", "VIEW"}},
        {"State 4", {"######","*#****","*#****","*#****","*#****","*#****"},{"BANANA","APPLE"}},
        {"State 5", {"########","########", "########", "########","########"}, {"APPLE","BERRY","MELON","ALOHA","GRAPE","MANGO"}},
       // {"State 6", {"#*****","#*****","######","#*****","#*****","#*****"}, {"GALAXY", "XENON"}},
        {"State 6", {"**********","*########*","*#******#*","*#******#*","*#******#*","*#******#*","*#******#*","*#******#*","*########*","**********"}, {"MOUNTAIN", "VALLEY"}},
        {"State 7", {"############","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","*#********#*","############"}, {"DEVELOPMENT","ALGORITHM","ENGINEERING","PROGRAMMING"}},
       // {"State 7", {"#####","*#*#*","#####","*#*#*","#####"}, {"LEVEL", "RADAR", "SOLOS"}},
      //  {"State 8", {"####","*#**","*#**","*#**"}, {"COOL", "COIL"}}
       // {"State 10", {"####","*#**","*#**","*#**"}, {"COOL","OVER","ORANGE", "COIL","OPEN"}}
    };

    for (const auto& state : states) 
    {
        outFile << "\n*********************";outFile  << state.name ;outFile << "*******************\n" << endl;
        
        CrosswordPuzzle puzzle(state.grid, state.words);
        BacktrackingSolver solver(puzzle, outFile);
        
        solver.solve();
        solver.print_report();
    }
    outFile.close();
    
     
    return 0;
}
