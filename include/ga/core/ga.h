#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cassert>
#include <cfloat>

#include <ga/core/model.h>
#include <ga/core/gene.h>

namespace ga {

struct individual {
public:
	individual() = delete;
	explicit individual(model<>&);
	individual(const individual&) = default;
	individual(individual&&) = default;
	individual& operator=(const individual&);
	individual& operator=(individual&&);
	
	template <typename gene_t>
	void add_gene(gene_t value) {
		assert(_chromosome.size() + 1 <= _model.get_chromosome_length());
		auto gene_id = _chromosome.size();
		const auto& alleles = _model.get_alleles<gene_t>(gene_id);
		auto it = std::find(alleles.begin(), alleles.end(), value);
		assert(it != alleles.end());
		auto gene = std::make_shared<gene_impl<gene_t>>(value);
		_chromosome.push_back(gene);
	}
	
	template <typename gene_t>
	void set_gene(size_t id, gene_t value) {
		assert(id < _chromosome.size());
		auto gene = std::dynamic_pointer_cast<gene_impl<gene_t>>(_chromosome[id]);
		const auto& alleles = _model.get_alleles<gene_t>(id);
		auto it = std::find(alleles.begin(), alleles.end(), value);
		assert(it != alleles.end());
		gene->set_value(value);
	}
	
	template <typename gene_t>
	gene_t get_gene(size_t id) {
		assert(id < _chromosome.size());
		auto gene = std::dynamic_pointer_cast<gene_impl<gene_t>>(_chromosome[id]);
		assert(gene);
		return gene->get_value();
	}
	
	void compute_fitness();
	double get_fitness() const;
	friend std::ostream& operator<<(std::ostream&, const individual&);
	
private:
	model<>& _model;
	std::vector<std::shared_ptr<gene>> _chromosome;
	double _fitness = -1;
};


struct population {
public:
	population() = delete;
	explicit population(model<>&, uint32_t);
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
	model<>& _model;
	uint32_t _generation;
	std::vector<individual> _individuals;
	double _fitness = -1;
};


struct algorithm {
public:
	model<>& get_model();
	void init(population&);
	void select(population&);
	void crossover(population&);
	void mutate(population&);
	void evolve();
	static void compute_fitness(population&);
	static bool is_converged(population&);
	static void print(population&);
	
private:
	model<> _model;
};

}