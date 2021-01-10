#include <ga/core/ga.h>

namespace ga {

individual::individual(std::string chrm) : _chromosome{std::move(chrm)} {}

const std::string& individual::get_chromosome() const {
	return _chromosome;
}

void individual::compute_fitness() {
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

int individual::get_fitness() const {
	return _fitness;
}

std::ostream& operator<<(std::ostream& os, const individual& indv) {
	os << "Individual " << indv._chromosome << ": " << indv._fitness;
	return os;
}

population::population(int gnrt) : _generation{gnrt} {}

population::population(int gnrt, std::vector<individual> indvs) : _generation{gnrt}, _indvs{std::move(indvs)} {}

int population::get_generation() {
	return _generation;
}

const std::vector<individual>& population::get_individuals() const {
	return _indvs;
}

void population::add_individual(const individual& indv) {
	_indvs.push_back(indv);
}

const individual& population::get_individual(int id) const {
	return _indvs[id];
}

void population::compute_fitness() {
	for (auto& indv : _indvs) {
		indv.compute_fitness();
	}
	std::sort(_indvs.begin(), _indvs.end(), [](const individual& lhs, const individual& rhs) {
		return lhs.get_fitness() < rhs.get_fitness();
	});
	_fitness = _indvs.front().get_fitness();;
}

int population::get_fitness() const {
	return _fitness;
}

std::ostream& operator<<(std::ostream& os, const population& pplt) {
	os << "Generation: " << pplt._generation << "\n";
	os << "Fitness: " << pplt._fitness << "\n";
	for (const auto& indv : pplt._indvs) {
		os << indv << "\n";
	}
	return os;
}

population algorithm::create_random_population() {
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

void algorithm::compute_fitness(population& pplt) {
	pplt.compute_fitness();
}

std::vector<individual> algorithm::do_selection(population& pplt) {
	int n_selected = POPULATION_SIZE >> 1;
	auto& indvs = pplt.get_individuals();
	std::vector<individual> selected(indvs.begin(), indvs.begin() + n_selected);
	return std::move(selected);
}

void algorithm::do_crossover(population& pplt) {
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

void algorithm::do_mutation(population& pplt) {
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

void algorithm::evolve() {
	population pplt{std::move(create_random_population())};
	compute_fitness(pplt);
	print(pplt);
	while (!is_converged(pplt)) {
		auto indvs = std::move(do_selection(pplt));
		population new_pplt{ pplt.get_generation() + 1, std::move(indvs) };
		do_crossover(new_pplt);
		do_mutation(new_pplt);
		compute_fitness(new_pplt);
		print(pplt);
		pplt = std::move(new_pplt);
	}
	std::cout << "SOLUTION\n" << pplt.get_individual(0).get_chromosome() << std::endl;
}

bool algorithm::is_converged(population& pplt) {
	return pplt.get_fitness() == 0 || pplt.get_generation() == 1000;
}

void algorithm::print(population& pplt) {
	std::cout << pplt << std::endl;
}

}

int main(int argc, char** argv) {
	ga::algorithm algr;
	algr.evolve();
}
