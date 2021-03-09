#pragma once

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

#include <ga/core/model.h>
#include <ga/core/gene.h>

namespace ga {

struct individual {
public:
	individual() = delete;
	explicit individual(default_model&);
	individual(const individual&) = default;
	individual(individual&&) = default;
	individual& operator=(const individual&);
	individual& operator=(individual&&);
	
	template <typename gene_t>
	void set_gene(std::string name, gene_t value) {
		auto gene_it = _chromosome.find(name);
		if (gene_it == _chromosome.end()) {
			auto gene_ptr = std::make_shared<gene_impl<gene_t>>();
			auto result = _chromosome.emplace(name, gene_ptr);
			assert(result.second);
			gene_it = result.first;
		}
		const auto& alleles = _model.get_alleles_of_gene<gene_t>(name);
		auto allele_it = std::find(alleles.begin(), alleles.end(), value);
		assert(allele_it != alleles.end());
		auto gene_ptr = std::dynamic_pointer_cast<gene_impl<gene_t>>(gene_it->second);
		assert(gene_ptr);
		gene_ptr->set(value);
	}
	
	template <typename gene_t>
	gene_t get_gene(std::string name) {
		auto gene_it = _chromosome.find(name);
		assert(gene_it != _chromosome.end());
		auto gene_ptr = std::dynamic_pointer_cast<gene_impl<gene_t>>(gene_it->second);
		assert(gene_ptr);
		return gene_ptr->get();
	}
	
	void compute_fitness();
	double get_fitness() const;
	friend std::ostream& operator<<(std::ostream&, const individual&);
	
private:
	default_model& _model;
	std::map<std::string, std::shared_ptr<gene>> _chromosome;
	double _fitness = -1;
};


struct population {
public:
	population() = delete;
	explicit population(default_model&, uint32_t);
	population(const population&) = default;
	population(population&&) = default;
	population& operator=(const population&);
	population& operator=(population&&);
	void increase_generation();
	uint32_t get_generation();
	void add_individual(const individual&);
	individual& get_individual(size_t);
	void remove_individuals_backward(size_t);
	void compute_fitness();
	double get_fitness() const;
	size_t size() const;
	friend std::ostream& operator<<(std::ostream&, const population&);
	
private:
	default_model& _model;
	uint32_t _generation;
	std::vector<individual> _individuals;
	double _fitness = -1;
};


struct algorithm {
public:
	default_model& get_model();
	void init(population&);
	void select(population&);
	void crossover(population&);
	void mutate(population&);
	void evolve();
	static void compute_fitness(population&);
	static bool is_converged(population&);
	static void print(population&);
	
private:
	default_model _model;
};

}