#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <iostream>

const std::string GENES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
const std::string TARGET = "Bao Chung";
const int POPULATION_SIZE = 50;

int get_random(int min, int max) {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> uid(min, max);
	return uid(mt);
}

class individual {
public:
	individual() = delete;

	individual(std::string chrm) : _chromosome{std::move(chrm)} {}

	individual(const individual&) = default;
	
	individual(individual&&) = default;

	individual& operator=(const individual&) = default;

	individual& operator=(individual&&) = default;

	const std::string& get_chromosome() const {
		return _chromosome;
	}

	void compute_fitness() {
		int trgt_ln = TARGET.length();
		if (_chromosome.length() != trgt_ln) {
			return;
		}
		_fitness = trgt_ln;
		for (int i = 0; i < trgt_ln; i++) {
			if (_chromosome[i] == TARGET[i]) {
				_fitness--;
			}
		}
	}

	int get_fitness() const {
		return _fitness;
	}

	friend std::ostream& operator<<(std::ostream&, const individual&);

private:
	std::string _chromosome;
	int _fitness = INT_MAX;
};

std::ostream& operator<<(std::ostream& os, const individual& indv) {
	os << "Individual " << indv._chromosome << ": " << indv._fitness;
	return os;
}

class population {
public:
	population() = delete;

	population(int gnrt) : _generation{gnrt} {}

	population(int gnrt, std::vector<individual> indvs) : _generation{gnrt}, _indvs{std::move(indvs)} {}

	population(const population&) = default;

	population(population&&) = default;

	population& operator=(const population&) = default;

	population& operator=(population&&) = default;

	int get_generation() {
		return _generation;
	}

	const std::vector<individual>& get_individuals() const {
		return _indvs;
	}
	
	void add_individual(const individual& indv) {
		_indvs.push_back(indv);
	}
	
	const individual& get_individual(int i) const {
		return _indvs[i];
	}

	void compute_fitness() {
		for (auto& indv : _indvs) {
			indv.compute_fitness();
		}
		std::sort(_indvs.begin(), _indvs.end(), [](const individual& lhs, const individual& rhs) {
			return lhs.get_fitness() < rhs.get_fitness();
		});
		_fitness = _indvs.front().get_fitness();;
	}

	int get_fitness() const {
		return _fitness;
	}
	
	friend std::ostream& operator<<(std::ostream&, const population&);

private:
	int _generation;
	std::vector<individual> _indvs;
	int _fitness;
};

std::ostream& operator<<(std::ostream& os, const population& pplt) {
	os << "Generation: " << pplt._generation << "\n";
	os << "Fitness: " << pplt._fitness << "\n";
	for (const auto& indv : pplt._indvs) {
		os << indv << "\n";
	}
	return os;
}

class ga {
public:
	static population create_random_population() {
		population pplt{0};
		int n_genes = GENES.length();
		int trgt_ln = TARGET.length();
		for (int i = 0; i < POPULATION_SIZE; i++) {
			std::string chrm;
			for (int j = 0; j < trgt_ln; j++) {
				chrm += GENES[get_random(0, n_genes - 1)];
			}
			pplt.add_individual(chrm);
		}
		return std::move(pplt);
	}

	static void compute_fitness(population& pplt) {
		pplt.compute_fitness();
	}

	static std::vector<individual> do_selection(population& pplt) {
		int n_selected = POPULATION_SIZE >> 1;
		auto& indvs = pplt.get_individuals();
		std::vector<individual> selected(indvs.begin(), indvs.begin() + n_selected);
		return std::move(selected);
	}
	
	static void do_crossover(population& pplt) {
		int trgt_ln = TARGET.length();
		int pivot = trgt_ln >> 1;
		int n_pairs = pplt.get_individuals().size() >> 1;
		for (int i = 0; i < n_pairs; i++) {
			// std::cout << "c" << i << std::endl;
			std::string prnt1 = pplt.get_individual(i << 1).get_chromosome();
			std::string prnt2 = pplt.get_individual((i << 1) + 1).get_chromosome();
			std::string chld1 = prnt1.substr(0, pivot) + prnt2.substr(pivot, trgt_ln - pivot);
			std::string chld2 = prnt2.substr(0, pivot) + prnt2.substr(pivot, trgt_ln - pivot);
			pplt.add_individual(std::move(chld1));
			pplt.add_individual(std::move(chld2));
		}
	}
	
	static void do_mutation(population& pplt) {
		int n_mutated_indvs = POPULATION_SIZE * 20 / 100;
		int n_mutated_genes = 1;
		int n_genes = GENES.length();
		int pplt_sz = pplt.get_individuals().size();
		int trgt_ln = TARGET.length();
		for (int i = 0; i < n_mutated_indvs; i++) {
			// std::cout << "m" << i << std::endl;
			int r = get_random(pplt_sz >> 1, pplt_sz - 1);
			std::string rand_chrm = pplt.get_individual(r).get_chromosome();
			for (int j = 0; j < n_mutated_genes; j++) {
				char rand_gene = GENES[get_random(0, n_genes - 1)];
				int r = get_random(0, trgt_ln - 1);
				rand_chrm[r] = rand_gene;
			}
			pplt.add_individual(std::move(rand_chrm));
		}
	}

	static void evolve() {
		population pplt{std::move(create_random_population())};
		compute_fitness(pplt);
		print(pplt);
		while (!is_converged(pplt)) {
			auto indvs = std::move(do_selection(pplt));
			population new_pplt{pplt.get_generation() + 1, std::move(indvs)};
			do_crossover(new_pplt);
			do_mutation(new_pplt);
			compute_fitness(new_pplt);
			print(pplt);
			pplt = std::move(new_pplt);
		}
		std::cout << "SOLUTION\n" << pplt.get_individual(0).get_chromosome() << std::endl;
	}

	static bool is_converged(population& pplt) {
		return pplt.get_fitness() == 0 || pplt.get_generation() == 1000;
	}

	static void print(population& pplt) {
		std::cout << pplt << std::endl;
	}
};

int main(int argc, char** argv) {
	ga ga;
	ga.evolve();
}
