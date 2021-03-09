#pragma once 

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <cassert>
#include <map>

#include <ga/core/ga.h>
#include <ga/core/policy.h>
#include <ga/core/gene.h>

namespace ga {

struct population;
struct individual;

template 
<
	typename encoding_method
>
struct problem_model {
public:
	problem_model() = default;
	problem_model(const problem_model&) = default;
	problem_model(problem_model&&) = default;
	problem_model& operator=(const problem_model&) = default;
	problem_model& operator=(problem_model&&) = default;
	~problem_model() = default;
	
public:
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
	
public:
	algorithm_model() = default;
	algorithm_model(const algorithm_model&) = default;
	algorithm_model(algorithm_model&&) = default;
	algorithm_model& operator=(const algorithm_model&) = default;
	algorithm_model& operator=(algorithm_model&&) = default;
	~algorithm_model() = default;
	
public:
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
	typename encoding_method = default_encoding_method,
	typename fitness_function = default_fitness_function,
	typename initialization_method = default_initialization_method,
	typename selection_method = default_selection_method,
	typename crossover_method = default_crossover_method,
	typename mutation_method = default_mutation_method
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