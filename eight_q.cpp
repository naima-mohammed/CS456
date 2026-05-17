#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
 
class queen_problem {

public:
    vector<int> queens;
    int fitness;
    queen_problem(vector<int> q) 
    {   queens = q;
      fitness = calculateFitness();  }

    int calculateFitness() 
    {   int conflicts = 0;//

        for (int i = 0; i < 8; i++) 
        {

            for (int j = i + 1; j < 8; j++) 
            {
                if (queens[i] == queens[j]) 
                { conflicts++; } // Same column

                if (abs(queens[i] - queens[j]) == abs(i - j)) 
                { conflicts++; } //// Same diagonal  
            }
        }

        // Non-attacking pairs
        return 28 - conflicts;
    }

    void printBoard() {

        for (int i = 0; i < 8; i++)
         { cout << queens[i] << " ";} cout << endl;

        cout << "fitness = " << fitness << endl;
    }
};


queen_problem tournamentSelection(vector<queen_problem>& population) {

    int tournamentSize = 3;

    // اختيار أول عنصر كبداية
    queen_problem best = population[rand() % population.size()];

    for (int i = 1; i < tournamentSize; i++) {

        queen_problem current = population[rand() % population.size()];

        // إذا الحالي أفضل
        if (current.fitness > best.fitness) {

            best = current;
        }
    }

    return best;
}

int main() {

    // Initial states
    vector<int> s1 = {4, 6, 0, 0, 4, 4, 3, 6};
    vector<int> s2 = {6, 5, 5, 7, 1, 4, 5, 1};
    vector<int> s3 = {5, 3, 0, 7, 0, 6, 3, 5};
    vector<int> s4 = {6, 7, 4, 0, 3, 3, 6, 2};
    // Population
    vector<queen_problem> population;

    population.push_back(queen_problem(s1));
    population.push_back(queen_problem(s2));
    population.push_back(queen_problem(s3));
    population.push_back(queen_problem(s4));

    // Print population
    for (int i = 0; i < population.size(); i++) {

        cout << "state " << i + 1 << endl; population[i].printBoard();  cout << endl;
    }

    queen_problem parent1 = tournamentSelection(population);
    cout << "Selected Parent:" << endl;
    parent1.printBoard();

    return 0;
}