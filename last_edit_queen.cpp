#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>

using namespace std;

class Population {

private:

    vector<vector<int>> population;

public:

    // Constructor
    Population() {}

    Population(vector<vector<int>> pop)
    {
        population = pop;
    }

    // Calculate Fitness
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

                for (int j = 0; j < 8; j++)
                {
                    if (i == j)
                        continue;

                    int d = abs(i - j);

                    if (x[j] == r ||
                        x[j] == r - d ||
                        x[j] == r + d)
                    {
                        conflicts_num++;
                    }
                }
            }

            int maxPairs = 28;

            fitness_vals.push_back(
            maxPairs - (conflicts_num / 2));
        }

        return fitness_vals;
    }

    // Roulette Wheel Selection
    vector<vector<int>> selection(
    vector<int> fitness_vals,
    ofstream& out)
    {
        vector<double> probs;

        // Copy fitness values
        for (int i = 0; i < fitness_vals.size(); i++)
        {
            probs.push_back(fitness_vals[i]);
        }

        // Sum probabilities
        double total = 0;

        for (int i = 0; i < probs.size(); i++)
        {
            total += probs[i];
        }

        // Calculate probabilities
        for (int i = 0; i < probs.size(); i++)
        {
            probs[i] = probs[i] / total;

            out << "Selection Probability "
                << i + 1
                << " = "
                << probs[i]
                << endl;
        }

        int N = population.size();

        vector<vector<int>> selected_population;

        // Random selection
        for (int k = 0; k < N; k++)
        {
            double r =
            (double) rand() / RAND_MAX;

            double cumulative = 0;

            for (int i = 0; i < N; i++)
            {
                cumulative += probs[i];

                if (r <= cumulative)
                {
                    selected_population.push_back(
                    population[i]);

                    break;
                }
            }
        }

        return selected_population;
    }

    // Tournament Selection
    vector<vector<int>> tournamentSelection(
    int k,
    ofstream& out)
    {
        vector<int> fitness =
        calcFitness();

        vector<vector<int>> selected_population;

        int N = population.size();

        for (int s = 0; s < N; s++)
        {
            int bestFitness = -1;

            int bestIndex = 0;

            out << "\nTournament "
                << s + 1
                << endl;

            for (int i = 0; i < k; i++)
            {
                int randomIndex =
                rand() % N;

                out << "Chosen Individual "
                    << randomIndex + 1
                    << " Fitness = "
                    << fitness[randomIndex]
                    << endl;

                if (fitness[randomIndex]
                    > bestFitness)
                {
                    bestFitness =
                    fitness[randomIndex];

                    bestIndex =
                    randomIndex;
                }
            }

            out << "Winner Individual = "
                << bestIndex + 1
                << endl;

            selected_population.push_back(
            population[bestIndex]);
        }

        return selected_population;
    }

    // Crossover
    vector<vector<int>> crossover(
    vector<vector<int>> selected_population,
    double pc,
    ofstream& out)
    {
        vector<vector<int>> new_population;

        for (int i = 0;
             i < selected_population.size();
             i += 2)
        {
            vector<int> parent1 =
            selected_population[i];

            vector<int> parent2 =
            selected_population[i + 1];

            vector<int> child1;
            vector<int> child2;

            double r =
            (double) rand() / RAND_MAX;

            if (r < pc)
            {
                int m =
                rand() % 7 + 1;

                //out << "\nCrossover Point = "
                  //  << m
                  //  << endl;

                // Child 1
                for (int j = 0; j < m; j++)
                {
                    child1.push_back(
                    parent1[j]);
                }

                for (int j = m; j < 8; j++)
                {
                    child1.push_back(
                    parent2[j]);
                }

                // Child 2
                for (int j = 0; j < m; j++)
                {
                    child2.push_back(
                    parent2[j]);
                }

                for (int j = m; j < 8; j++)
                {
                    child2.push_back(
                    parent1[j]);
                }
            }

            else
            {
                child1 = parent1;

                child2 = parent2;
            }

            new_population.push_back(child1);

            new_population.push_back(child2);
        }

       /// out << "\nTotal Offspring Generated = "
          //  << new_population.size()
          //  << endl;
//
        return new_population;
    }

    // Mutation
    vector<vector<int>> mutation(
    vector<vector<int>> population_after_crossover,
    double pm,
    ofstream& out)
    {
        for (int i = 0;
             i < population_after_crossover.size();
             i++)
        {
            double r =
            (double) rand() / RAND_MAX;

            if (r < pm)
            {
                int m = rand() % 8;

                int new_value =
                rand() % 8;

               /* out << "\nMutation happened in individual "
                    << i + 1
                    << " at index "
                    << m
                    << endl;*/

                population_after_crossover[i][m]
                = new_value;
            }
        }

        return population_after_crossover;
    }

    // Print Population
    void printPopulation(ofstream& out)
    {
        for (int i = 0; i < population.size(); i++)
        {
            out << "[ ";

            for (int j = 0;
                 j < population[i].size();
                 j++)
            {
                out << population[i][j]
                    << " ";
            }

            out << "]" << endl;
        }
    }
};

int main()
{
    srand(time(0));

    int generations = 8;

    ofstream out("output.txt");

    // Population 1
    vector<vector<int>> people1 =
    {
        {6,1,7,3,6,1,4,0},
        {3,4,0,6,6,1,2,3},
        {0,3,7,0,1,1,4,0},
        {0,2,2,7,4,4,7,3}
    };

    // Population 2
    vector<vector<int>> people2 =
    {
        {1,5,8,6,3,7,2,4},
        {4,2,7,3,6,8,5,1},
        {6,3,1,8,5,2,4,7},
        {7,4,2,5,8,1,3,6}
    };

    // Population 3
    vector<vector<int>> people3 =
    {
        {0,4,7,5,2,6,1,1},
    {6,1,5,2,0,7,4,4},
    {3,7,0,4,6,1,5,5},
    {5,2,6,1,7,4,0,0}

    };

    // Population 4
    vector<vector<int>> people4 =
    {
        {0,4,1,5,2,6,1,3},
        {6,1,5,2,0,7,4,3},
        {3,7,0,4,6,1,5,2},
        {5,2,6,1,7,4,0,3},
        {1,6,2,5,7,4,0,3},
        {4,7,3,0,2,5,1,6}
    };

    // Population 5
    vector<vector<int>> people5 =
    {
        {4,6,0,0,4,4,3,6},
        {6,5,5,7,1,4,5,1},
        {5,3,0,7,0,6,3,5},
        {6,7,4,0,3,3,6,2}
    };

    // All Populations
    vector<vector<vector<int>>> all_populations =
    {
        people1,
        people2,
        people3,
        people4,
        people5
    };


    for (int p = 0;
         p < all_populations.size();
         p++)
    {
        Population roulette_population(
        all_populations[p]);

      
        out << "\n*********Population "<< p + 1 << " - Roulette Wheel************\n";

      
        roulette_population.printPopulation(out);

        auto roulette_start =
        chrono::high_resolution_clock::now();

        for (int g = 1;
             g <= generations;
             g++)
        {
            out << "\nGeneration "
                << g
                << endl;

            vector<int> fitness =
            roulette_population.calcFitness();

            // Best Fitness
            int bestFitness =
            fitness[0];

            for (int i = 1;
                 i < fitness.size();
                 i++)
            {
                if (fitness[i]
                    > bestFitness)
                {
                    bestFitness =
                    fitness[i];
                }
            }

            // Stop if perfect solution found
            if (bestFitness == 28)
            {
                out << "\nPerfect Solution Found!\n";

                break;
            }

            vector<vector<int>> selected =
            roulette_population.selection(
            fitness,
            out);

            vector<vector<int>> crossed =
            roulette_population.crossover(
            selected,
            0.8,
            out);

            vector<vector<int>> mutated =
            roulette_population.mutation(
            crossed,
            0.2,
            out);

            out << "\nPopulation After Mutation:\n";

            for (int i = 0;
                 i < mutated.size();
                 i++)
            {
                out << "[ ";

                for (int j = 0;
                     j < mutated[i].size();
                     j++)
                {
                    out << mutated[i][j]
                        << " ";
                }

                out << "]" << endl;
            }
        }

        auto roulette_end =
        chrono::high_resolution_clock::now();

        auto roulette_duration =
        chrono::duration_cast
        <chrono::milliseconds>
        (roulette_end - roulette_start);

        out << "\nRoulette Execution Time = "
            << roulette_duration.count()
            << " ms\n";

        out << "\nRoulette Finished\n";
    }

   

    for (int p = 0;
         p < all_populations.size();
         p++)
    {
        Population tournament_population(
        all_populations[p]);

       
        out << "\n*********Population "
            << p + 1
            << " - Tournament********";

   
        tournament_population.printPopulation(out);

        auto tournament_start =
        chrono::high_resolution_clock::now();

        for (int g = 1;
             g <= generations;
             g++)
        {
            out << "\nGeneration "
                << g
                << endl;

            vector<int> fitness =
            tournament_population.calcFitness();

            // Best Fitness
            int bestFitness =
            fitness[0];

            for (int i = 1;
                 i < fitness.size();
                 i++)
            {
                if (fitness[i]
                    > bestFitness)
                {
                    bestFitness =
                    fitness[i];
                }
            }

            // Stop if perfect solution found
            if (bestFitness == 28)
            {
                out << "\nPerfect Solution Found!\n";

                break;
            }

            vector<vector<int>> selected =
            tournament_population
            .tournamentSelection(
            2,
            out);

            vector<vector<int>> crossed =
            tournament_population.crossover(
            selected,
            0.8,
            out);

            vector<vector<int>> mutated =
            tournament_population.mutation(
            crossed,
            0.2,
            out);

            out << "\nPopulation After Mutation:\n";

            for (int i = 0;
                 i < mutated.size();
                 i++)
            {
                out << "[ ";

                for (int j = 0;
                     j < mutated[i].size();
                     j++)
                {
                    out << mutated[i][j]
                        << " ";
                }

                out << "]" << endl;
            }
        }

        auto tournament_end =
        chrono::high_resolution_clock::now();

        auto tournament_duration =
        chrono::duration_cast
        <chrono::milliseconds>
        (tournament_end - tournament_start);

        out << "\nTournament Execution Time = "
            << tournament_duration.count()
            << " ms\n";

        out << "\nTournament Finished\n";
    }

    out.close();

    return 0;
}