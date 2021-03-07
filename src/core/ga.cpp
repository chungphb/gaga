#include <ga/core/ga.h>

namespace ga {

individual::individual(model<>& md) : _model{md} {}

individual& individual::operator=(const individual& ind) {
	_model = ind._model;
  	_chromosome = ind._chromosome;
	compute_fitness();
	return *this;
}

individual& individual::operator=(individual&& ind) {
	_model = ind._model;
  	_chromosome = std::move(ind._chromosome);
	compute_fitness();
	return *this;
}

void individual::compute_fitness() {
	_fitness = _model.get_fitness_cb()(*this);
}

double individual::get_fitness() const {
	return _fitness;
}

std::ostream& operator<<(std::ostream& os, const individual& ind) {
	os << "Individual (";
	const auto& chr = ind._chromosome;
	size_t chr_l = chr.size();
	for (size_t i = 0; i < chr_l; i++) {
		auto gene = std::dynamic_pointer_cast<gene_impl<int>>(chr[i]); // FIXME
		os << *gene << ((i == chr_l - 1) ? "" : ", ");
	}
	os << "): " << ind._fitness;
	return os;
}

population::population(model<>& md, uint32_t gen) : _model{md}, _generation{gen} {}

population& population::operator=(const population& ppl) {
	this->_model = ppl._model;
	this->_generation = ppl._generation;
	this->_individuals = ppl._individuals;
	this->_fitness = ppl._fitness;
	return *this;
}

population& population::operator=(population&& ppl) {
	this->_model = ppl._model;
	this->_generation = ppl._generation;
	this->_individuals = std::move(ppl._individuals);
	this->_fitness = ppl._fitness;
	return *this;
}

void population::increase_generation() {
	_generation++;
}

uint32_t population::get_generation() {
	return _generation;
}

void population::add_individual(const individual& ind) {
	_individuals.push_back(ind);
}

individual& population::get_individual(size_t id) {
	assert(id < _individuals.size());
	return _individuals[id];
}

void population::remove_individuals_backward(size_t n) {
	_individuals.erase(_individuals.begin() + _individuals.size() - n, _individuals.end());
}

void population::compute_fitness() {
	for (auto& ind : _individuals) {
		ind.compute_fitness();
	}
	std::sort(_individuals.begin(), _individuals.end(), [](const individual& lhs, const individual& rhs) {
		return lhs.get_fitness() < rhs.get_fitness();
	});
	_fitness = _individuals.front().get_fitness();
}

double population::get_fitness() const {
	return _fitness;
}

size_t population::size() const {
	return _individuals.size();
}

std::ostream& operator<<(std::ostream& os, const population& ppl) {
	os << "Generation: " << ppl._generation << "\n";
	os << "Fitness: " << ppl._fitness << "\n";
	for (const auto& ind : ppl._individuals) {
		os << ind << "\n";
	}
	return os;
}

model<>& algorithm::get_model() {
	return _model;
}

void algorithm::init(population& ppl) {
	_model.get_initialization_cb()(ppl);
}

void algorithm::select(population& ppl) {
	_model.get_selection_cb()(ppl);
}

void algorithm::crossover(population& ppl) {
	_model.get_crossover_cb()(ppl);
}

void algorithm::mutate(population& ppl) {
	_model.get_mutation_cb()(ppl);
}

void algorithm::evolve() {
	population ppl{_model, 0};
	init(ppl);
	compute_fitness(ppl);
	print(ppl);
	while (!is_converged(ppl)) {
		select(ppl);
		ppl.increase_generation();
		crossover(ppl);
		mutate(ppl);
		compute_fitness(ppl);
		print(ppl);
	}
}

void algorithm::compute_fitness(population& ppl) {
	ppl.compute_fitness();
}

bool algorithm::is_converged(population& ppl) {
	return ppl.get_fitness() == 0 || ppl.get_generation() == 1000;
}

void algorithm::print(population& ppl) {
	std::cout << ppl << "\n";
}

}