#include <ga/core/ga.h>
#include <assert.h>

namespace ga {

size_t model::get_chromosome_length() const {
	return _pm._genes.size();
}

void model::set_population_size(uint16_t ppl_s) {
	_am._population_size = ppl_s;
}

uint16_t model::get_population_size() const {
	return _am._population_size;
}

void model::set_mutation_rate(double mutation_rate) {
	_am._mutation_rate = mutation_rate;
}

double model::get_mutation_rate() const {
	return _am._mutation_rate;
}

void model::register_fitness_cb(fitness_cb_t cb) {
	_am._fitness_cb = std::move(cb);
}

const model::fitness_cb_t& model::get_fitness_cb() const {
	return _am._fitness_cb;
}

void model::register_initialization_cb(initialization_cb_t cb) {
	_am._initialization_cb = std::move(cb);
}

const model::initialization_cb_t& model::get_initialization_cb() const {
	return _am._initialization_cb;
}

void model::register_selection_cb(selection_cb_t cb) {
	_am._selection_cb = std::move(cb);
}

const model::selection_cb_t& model::get_selection_cb() const {
	return _am._selection_cb;
}

void model::register_crossover_cb(crossover_cb_t cb) {
	_am._crossover_cb = std::move(cb);
}

const model::crossover_cb_t& model::get_crossover_cb() const {
	return _am._crossover_cb;
}

void model::register_mutation_cb(mutation_cb_t cb) {
	_am._mutation_cb = std::move(cb);
}

const model::mutation_cb_t& model::get_mutation_cb() const {
	return _am._mutation_cb;
}

individual::individual(model& md) : _model{md} {}

individual& individual::operator=(const individual& ind) {
	this->_model = ind._model;
  	this->_chromosome = ind._chromosome;
	this->_fitness = ind._fitness;
	return *this;
}

individual& individual::operator=(individual&& ind) {
	this->_model = ind._model;
  	this->_chromosome = std::move(ind._chromosome);
	this->_fitness = ind._fitness;
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
		auto gene = std::dynamic_pointer_cast<gene_value<int>>(chr[i]); // FIXME
		os << *gene << ((i == chr_l - 1) ? "" : ", ");
	}
	os << "): " << ind._fitness;
	return os;
}

population::population(model& md, uint32_t gen) : _model{md}, _generation{gen} {}

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
	assert(id >= 0 && id < _individuals.size());
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

model& algorithm::get_model() {
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