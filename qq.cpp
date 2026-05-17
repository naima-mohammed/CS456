#include <iostream>
#include <vector> 
#include <cmath>// for abs()
#include <cstdlib>// for rand() and srand()
#include <ctime> // for time() to seed the random number generator
using namespace std;

class Population {
private:
    vector<vector<int>> population;
public:
    // Constructor
    Population(){ }    
    Population(vector<vector<int>> pop)
     {population = pop;}

    // Calculate fitness
    vector<int> calcFitness() 
    {   
        vector<int> fitness_vals;
        for (int p = 0; p < population.size(); p++)
         {
            vector<int> x = population[p];
            int conflicts_num = 0;
            for (int i = 0; i < 8; i++)
             {

                int r = x[i];

                for (int j = 0; j < 8; j++) {

                    if (i == j)
                        continue;

                    int d = abs(i - j);

                    if (x[j] == r || x[j] == r - d || x[j] == r + d) {
                        conflicts_num++;
                    }
                }
            }

            int maxPairs = 28;
            fitness_vals.push_back(maxPairs - (conflicts_num / 2));
        }

        return fitness_vals;
    }

    // Selection
    vector<vector<int>> selection(vector<int> fitness_vals) {

        vector<double> probs;

        // Copy fitness values
        for (int i = 0; i < fitness_vals.size(); i++) {
            probs.push_back(fitness_vals[i]);
        }

        // Sum probabilities
        double total = 0;

        for (int i = 0; i < probs.size(); i++) {

            total += probs[i];
        }

        // Normalize
        //2.Determine the selection probabilities for each individual
        for (int i = 0; i < probs.size(); i++)
        {
            probs[i] = probs[i] / total;

           
           cout << "selection probabilities: " << i + 1 << " Probability = "<< probs[i]<< endl;
        }

        int N = population.size();

        vector<vector<int>> selected_population;

        // Random selection based on probability
        for (int k = 0; k < N; k++) {

            //3.Generate a random selection point
            double r = (double) rand() / RAND_MAX;

            double cumulative = 0;

            //4.Select an individual based on the random selection point
            for (int i = 0; i < N; i++) {

                cumulative += probs[i];

                if (r <= cumulative) {

                    selected_population.push_back(population[i]);

                    break;
                }
            }
        }

        //Return the selected individual for crossover or mutation
        return selected_population;
    }

    // Print population
    void printPopulation() {

        for (int i = 0; i < population.size(); i++) {

            cout << "[ ";

            for (int j = 0; j < population[i].size(); j++) {

                cout << population[i][j] << " ";
            }

            cout << "]" << endl;
        }
    }

    // Print fitness values
    void printFitness() {

        //Calculate the total fitness of the entire population
        vector<int> fitness = calcFitness();

        cout << "\nFitness Values:\n";

        for (int i = 0; i < fitness.size(); i++) {

            cout << fitness[i] << endl;
        }
    }

    // Print selected population
    void printSelectedPopulation() {

        vector<int> fitness = calcFitness();

        vector<vector<int>> selected = selection(fitness);

        cout << "\nSelected Population:\n";

        for (int i = 0; i < selected.size(); i++) {

            cout << "[ ";

            for (int j = 0; j < selected[i].size(); j++) {

                cout << selected[i][j] << " ";
            }

            cout << "]" << endl;
        }
    }
};

int main() {

    srand(time(0));
     // Initial Population
     // مثال شيت 
    vector<vector<int>> people = {

    {2,4,7,4,8,5,5,2},
    {3,2,7,5,2,4,1,1},
    {2,4,4,1,5,1,2,4},
    {3,2,5,4,3,2,1,3}
    };


    /*
// مثال فيديو 
     vector<vector<int>> people = {
    {6,1,7,3,6,1,4,0},
    {3,4,0,6,6,1,2,3},
    {0,3,7,0,1,1,4,0},
    {0,2,2,7,4,4,7, 3}
    };
*/

    Population initial_population(people);

    cout << "Initial Population:\n";

    initial_population.printPopulation();

    initial_population.printFitness();

    initial_population.printSelectedPopulation();

    return 0;
}
