#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <iostream>

namespace ga {

const std::string GENES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
const std::string TARGET = "Bao Chung";
const int POPULATION_SIZE = 50;

inline int get_random(int min, int max) {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> uid(min, max);
	return uid(mt);
}

class individual {
public:
	individual() = delete;
	individual(std::string);
	individual(const individual&) = default;
	individual(individual&&) = default;
	individual& operator=(const individual&) = default;
	individual& operator=(individual&&) = default;
	const std::string& get_chromosome() const;
	void compute_fitness();
	int get_fitness() const;
	friend std::ostream& operator<<(std::ostream&, const individual&);
private:
	std::string _chromosome;
	int _fitness = INT_MAX;
};

class population {
public:
	population() = delete;
	population(int);
	population(int, std::vector<individual>);
	population(const population&) = default;
	population(population&&) = default;
	population& operator=(const population&) = default;
	population& operator=(population&&) = default;
	int get_generation();
	const std::vector<individual>& get_individuals() const;
	void add_individual(const individual&);
	const individual& get_individual(int) const;
	void compute_fitness();
	int get_fitness() const;
	friend std::ostream& operator<<(std::ostream&, const population&);
private:
	int _generation;
	std::vector<individual> _indvs;
	int _fitness;
};

class algorithm {
public:
	static population create_random_population();
	static void compute_fitness(population&);
	static std::vector<individual> do_selection(population&);
	static void do_crossover(population&);
	static void do_mutation(population&);
	static void evolve();
	static bool is_converged(population&);
	static void print(population&);
};

}