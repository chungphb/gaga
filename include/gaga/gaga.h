#ifndef GAGA_H
#define GAGA_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cassert>
#include <cfloat>
#include <map>
#include <string>

namespace gaga {

struct alleles {
	virtual ~alleles() = default;
};


template <typename gene_t>
struct alleles_impl : public alleles {
public:
	explicit alleles_impl(const std::vector<gene_t>& values) : _alleles{std::move(values)} {}

	const std::vector<gene_t>& get() const {
		return _alleles;
	}

private:
	std::vector<gene_t> _alleles;
};


struct gene {
	virtual ~gene() = default;
};


template <typename gene_t>
struct gene_impl : public gene {
public:
	explicit gene_impl(gene_t value) : _value{value} {}

	void set(gene_t value) {
		_value = value;
	}

	gene_t get() const {
		return _value;
	}

private:
	gene_t _value;
};


template <typename gene_t>
inline std::ostream& operator<<(std::ostream& os, gene_impl<gene_t> g) {
	os << g.get();
	return os;
}


struct individual;
struct population;

namespace model {

namespace policy {

struct default_encoding_method {};


struct default_fitness_function {
	double operator()(individual&) {
		return 0;
	}
};


struct default_initialization_method {
	void operator()(population&) {
		return;
	}
};


struct default_selection_method {
	void operator()(population&) {
		return;
	}
};


struct default_crossover_method {
	void operator()(population&) {
		return;
	}
};


struct default_mutation_method {
	void operator()(population&) {
		return;
	}
};

}


template
<
	typename encoding_method
>
struct problem_model {
	std::map<std::string, std::shared_ptr<alleles>> _genes;
};


template
<
	typename fitness_function,
	typename initialization_method,
	typename selection_method,
	typename crossover_method,
	typename mutation_method
>
struct algorithm_model {
	using fitness_cb_t = std::function<double(individual&)>;
	using initialization_cb_t = std::function<void(population&)>;
	using selection_cb_t = std::function<void(population&)>;
	using crossover_cb_t = std::function<void(population&)>;
	using mutation_cb_t = std::function<void(population&)>;

	uint16_t _population_size = 50;
	double _mutation_rate = 10;
	fitness_cb_t _fitness_cb;
	initialization_cb_t _initialization_cb;
	selection_cb_t _selection_cb;
	crossover_cb_t _crossover_cb;
	mutation_cb_t _mutation_cb;
};


template
<
	typename encoding_method = policy::default_encoding_method,
	typename fitness_function = policy::default_fitness_function,
	typename initialization_method = policy::default_initialization_method,
	typename selection_method = policy::default_selection_method,
	typename crossover_method = policy::default_crossover_method,
	typename mutation_method = policy::default_mutation_method
>
struct model {
	using problem_model_t = problem_model<encoding_method>;
	using algorithm_model_t = algorithm_model<fitness_function, initialization_method, selection_method, crossover_method, mutation_method>;
	using fitness_cb_t = typename algorithm_model_t::fitness_cb_t;
	using initialization_cb_t = typename algorithm_model_t::initialization_cb_t;
	using selection_cb_t = typename algorithm_model_t::selection_cb_t;
	using crossover_cb_t = typename algorithm_model_t::crossover_cb_t;
	using mutation_cb_t = typename algorithm_model_t::mutation_cb_t;

public:
	size_t get_chromosome_length() const {
		return _pm._genes.size();
	}

	template <typename gene_t>
	void create_gene(std::string name, const std::vector<gene_t>& values) {
		auto gene_it = _pm._genes.find(name);
		assert(gene_it == _pm._genes.find(name));
		auto alleles_ptr = std::make_shared<alleles_impl<gene_t>>(std::move(values));
		assert(alleles_ptr);
		auto result = _pm._genes.emplace(std::move(name), alleles_ptr);
		assert(result.second);
	}

	template <typename gene_t>
	const std::vector<gene_t>& get_alleles_of_gene(std::string name) const {
		auto gene_it = _pm._genes.find(name);
		assert(gene_it != _pm._genes.end());
		auto alleles_ptr = std::dynamic_pointer_cast<alleles_impl<gene_t>>(gene_it->second);
		assert(alleles_ptr);
		return alleles_ptr->get();
	}

	std::vector<std::string> get_gene_list() {
		std::vector<std::string> result;
		const auto& gene_list = _pm._genes;
		for (const auto& gene : gene_list) {
			result.push_back(gene.first);
		}
		return result;
	}

	void set_population_size(uint16_t ppl_size) {
		_am._population_size = ppl_size;
	}

	uint16_t get_population_size() const {
		return _am._population_size;
	}

	void set_mutation_rate(double mutation_rate) {
		_am._mutation_rate = mutation_rate;
	}

	double get_mutation_rate() const {
		return _am._mutation_rate;
	}

	void register_fitness_cb(fitness_cb_t cb) {
		_am._fitness_cb = std::move(cb);
	}

	const fitness_cb_t& get_fitness_cb() const {
		return _am._fitness_cb;
	}

	void register_initialization_cb(initialization_cb_t cb) {
		_am._initialization_cb = std::move(cb);
	}

	const initialization_cb_t& get_initialization_cb() const {
		return _am._initialization_cb;
	}

	void register_selection_cb(selection_cb_t cb) {
		_am._selection_cb = std::move(cb);
	}

	const selection_cb_t& get_selection_cb() const {
		return _am._selection_cb;
	}

	void register_crossover_cb(crossover_cb_t cb) {
		_am._crossover_cb = std::move(cb);
	}

	const crossover_cb_t& get_crossover_cb() const {
		return _am._crossover_cb;
	}

	void register_mutation_cb(mutation_cb_t cb) {
		_am._mutation_cb = std::move(cb);
	}

	const mutation_cb_t& get_mutation_cb() const {
		return _am._mutation_cb;
	}

private:
	problem_model_t _pm;
	algorithm_model_t _am;
};

using default_model = model<>;

}


struct individual {
public:
	explicit individual(model::default_model& md) : _model{md} {}

	individual(const individual& ind) : _model{ind._model}, _chromosome{ind._chromosome} {
		compute_fitness();
	}

	individual(individual&& ind) : _model{ind._model}, _chromosome{std::move(ind._chromosome)} {
		compute_fitness();
	}

	individual& operator=(const individual& ind) {
		_model = ind._model;
		_chromosome = ind._chromosome;
		compute_fitness();
		return *this;
	}

	individual& operator=(individual&& ind) {
		_model = ind._model;
		_chromosome = std::move(ind._chromosome);
		compute_fitness();
		return *this;
	}

	template <typename gene_t>
	void set_gene(std::string name, gene_t value) {
		const auto& alleles = _model.get_alleles_of_gene<gene_t>(name);
		auto allele_it = std::find(alleles.begin(), alleles.end(), value);
		assert(allele_it != alleles.end());
		auto gene_it = _chromosome.find(name);
		if (gene_it == _chromosome.end()) {
			auto gene_ptr = std::make_shared<gene_impl<gene_t>>(value);
			auto result = _chromosome.emplace(name, gene_ptr);
			assert(result.second);
		} else {
			auto gene_ptr = std::dynamic_pointer_cast<gene_impl<gene_t>>(gene_it->second);
			assert(gene_ptr);
			gene_ptr->set(value);
		}

	}

	template <typename gene_t>
	gene_t get_gene(std::string name) {
		auto gene_it = _chromosome.find(name);
		assert(gene_it != _chromosome.end());
		auto gene_ptr = std::dynamic_pointer_cast<gene_impl<gene_t>>(gene_it->second);
		assert(gene_ptr);
		return gene_ptr->get();
	}

	void compute_fitness() {
		_fitness = _model.get_fitness_cb()(*this);
	}

	double get_fitness() const {
		return _fitness;
	}

	friend std::ostream& operator<<(std::ostream& os, const individual& ind);

private:
	model::default_model& _model;
	std::map<std::string, std::shared_ptr<gene>> _chromosome;
	double _fitness = -1;
};

inline std::ostream& operator<<(std::ostream& os, const individual& ind) {
	os << "Individual (";
	const auto end_it = ind._chromosome.end();
	for (auto it = ind._chromosome.begin(); it != end_it;) {
		auto gene_ptr = std::dynamic_pointer_cast<gene_impl<int>>(it->second);
		it++;
		os << *gene_ptr << (it == end_it ? "" : ", ");
	}
	os << "): " << ind.get_fitness();
	return os;
}


struct population {
public:
	population() = delete;

	explicit population(model::default_model& md, uint32_t gen) : _model{md}, _generation{gen} {}

	void increase_generation() {
		_generation++;
	}

	uint32_t get_generation() const {
		return _generation;
	}

	void add_individual(const individual& ind) {
		_individuals.push_back(ind);
	}

	individual& get_individual(size_t id) {
		assert(id < _individuals.size());
		return _individuals[id];
	}

	const individual& get_individual(size_t id) const {
		assert(id < _individuals.size());
		return _individuals[id];
	}

	void remove_individuals_backward(size_t num) {
		assert(num <= _individuals.size());
		_individuals.erase(_individuals.begin() + _individuals.size() - num, _individuals.end());
	}

	void compute_fitness() {
		for (auto& ind : _individuals) {
			ind.compute_fitness();
		}
		std::sort(_individuals.begin(), _individuals.end(), [](const individual& lhs, const individual& rhs) {
			return lhs.get_fitness() < rhs.get_fitness();
		});
		_fitness = _individuals.front().get_fitness();
	}

	double get_fitness() const {
		return _fitness;
	}

	size_t size() const {
		return _individuals.size();
	}

private:
	model::default_model& _model;
	uint32_t _generation;
	std::vector<individual> _individuals;
	double _fitness = -1;
};


inline std::ostream& operator<<(std::ostream& os, const population& ppl) {
	os << "Generation: " << ppl.get_generation() << "\n";
	os << "Fitness: " << ppl.get_fitness() << "\n";
	const auto ppl_size = ppl.size();
	for (size_t id = 0; id < ppl_size; id++) {
		const auto& ind = ppl.get_individual(id);
		os << ind << "\n";
	}
	return os;
}


struct algorithm {
public:
	model::default_model& get_model() {
		return _model;
	}

	void evolve() {
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

private:
	void init(population& ppl) {
		_model.get_initialization_cb()(ppl);
	}

	void select(population& ppl) {
		_model.get_selection_cb()(ppl);
	}

	void crossover(population& ppl) {
		_model.get_crossover_cb()(ppl);
	}

	void mutate(population& ppl) {
		_model.get_mutation_cb()(ppl);
	}

	static void compute_fitness(population& ppl) {
		ppl.compute_fitness();
	}

	static bool is_converged(population& ppl) {
		return ppl.get_fitness() == 0 || ppl.get_generation() == 1000;
	}

	static void print(population& ppl) {
		std::cout << ppl << "\n";
	}

private:
	model::default_model _model;
};

}

#endif // GAGA_H