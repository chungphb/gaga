#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cassert>
#include <cfloat>

namespace ga {

struct gene {
	gene() = default;
	gene(const gene&) = default;
	gene(gene&&) = default;
	gene& operator=(const gene&) = default;
	gene& operator=(gene&&) = default;
	virtual ~gene() = default;
};

template <typename gene_t>
struct alleles : public gene {
public:
	alleles() = default;
    explicit alleles(const std::vector<gene_t>& values) : _alleles{std::move(values)} {}
	alleles(const alleles&) = default;
	alleles(alleles&&) = default;
	alleles& operator=(const alleles&) = default;
	alleles& operator=(alleles&&) = default;
	~alleles() override = default;
	const std::vector<gene_t>& get_alleles() const {
		return _alleles;
	}
private:
	std::vector<gene_t> _alleles;
};

template <typename gene_t>
struct gene_value : public gene {
public:
	gene_value() = default;
    explicit gene_value(gene_t value) : _value{value} {}
	gene_value(const gene_value&) = default;
	gene_value(gene_value&&) = default;
	gene_value& operator=(const gene_value&) = default;
	gene_value& operator=(gene_value&&) = default;
	~gene_value() override = default;
	void set_value(gene_t value) {
		_value = value;
	}
	gene_t get_value() const {
		return _value;
	}
private:
	gene_t _value;
};

template <typename gene_t>
inline std::ostream& operator<<(std::ostream& os, gene_value<gene_t> gene) {
	os << gene.get_value();
	return os;
}

struct population;

struct individual;

struct problem_model {
public:
	problem_model() = default;
    problem_model(const problem_model&) = default;
    problem_model(problem_model&&) = default;
    problem_model& operator=(const problem_model&) = default;
    problem_model& operator=(problem_model&&) = default;
    ~problem_model() = default;
    friend struct model;
private:
	std::vector<std::shared_ptr<gene>> _genes;
};

struct algorithm_model {
	using fitness_cb_t = std::function<bool(individual&)>;
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
    friend struct model;
private:
	uint16_t _population_size = 50;
	double _mutation_rate = 10;
	fitness_cb_t _fitness_cb;
	initialization_cb_t _initialization_cb;
	selection_cb_t _selection_cb;
	crossover_cb_t _crossover_cb;
	mutation_cb_t _mutation_cb;
};

struct model {
	using fitness_cb_t = typename algorithm_model::fitness_cb_t;
	using initialization_cb_t = typename algorithm_model::initialization_cb_t;
	using selection_cb_t = typename algorithm_model::selection_cb_t;
	using crossover_cb_t = typename algorithm_model::crossover_cb_t;
	using mutation_cb_t = typename algorithm_model::mutation_cb_t;
public:
	model() = default;
    model(const model&) = default;
    model(model&&) = default;
    model& operator=(const model&) = default;
    model& operator=(model&&) = default;
    ~model() = default;
	size_t get_chromosome_length() const;
	template <typename gene_t>
	void add_gene(const std::vector<gene_t>& values) {
		auto gene = std::make_shared<alleles<gene_t>>(std::move(values));
		_pm._genes.push_back(std::move(gene));
    }
	template <typename gene_t>
  	const std::vector<gene_t>& get_alleles(size_t id) const {
      	assert(id >= 0 && id < _pm._genes.size());
		auto gene = std::dynamic_pointer_cast<alleles<gene_t>>(_pm._genes[id]);
      	assert(gene);
		return gene->get_alleles();
    }
    void set_population_size(uint16_t);
	uint16_t get_population_size() const;
	void set_mutation_rate(double);
	double get_mutation_rate() const;
	void register_fitness_cb(fitness_cb_t);
	const fitness_cb_t& get_fitness_cb() const;
  	void register_initialization_cb(initialization_cb_t);
	const initialization_cb_t& get_initialization_cb() const;
  	void register_selection_cb(selection_cb_t);
	const selection_cb_t& get_selection_cb() const;
  	void register_crossover_cb(crossover_cb_t);
	const crossover_cb_t& get_crossover_cb() const;
  	void register_mutation_cb(mutation_cb_t);
	const mutation_cb_t& get_mutation_cb() const;
private:
	problem_model _pm;
	algorithm_model _am;
};

struct individual {
public:
	individual() = delete;
	explicit individual(model&);
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
		auto gene = std::make_shared<gene_value<gene_t>>(value);
		_chromosome.push_back(gene);
	}
	template <typename gene_t>
	void set_gene(size_t id, gene_t value) {
		assert(id >= 0 && id < _chromosome.size());
		auto gene = std::dynamic_pointer_cast<gene_value<gene_t>>(_chromosome[id]);
		const auto& alleles = _model.get_alleles<gene_t>(id);
		auto it = std::find(alleles.begin(), alleles.end(), value);
		assert(it != alleles.end());
		gene->set_value(value);
	}
	template <typename gene_t>
	gene_t get_gene(size_t id) {
		assert(id >= 0 && id < _chromosome.size());
		auto gene = std::dynamic_pointer_cast<gene_value<gene_t>>(_chromosome[id]);
		assert(gene);
      	return gene->get_value();
	}
	void compute_fitness();
	double get_fitness() const;
	friend std::ostream& operator<<(std::ostream&, const individual&);
private:
	model& _model;
	std::vector<std::shared_ptr<gene>> _chromosome;
	double _fitness = DBL_MAX;
};

struct population {
public:
	population() = delete;
	explicit population(model&, uint32_t);
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
	model& _model;
	uint32_t _generation;
	std::vector<individual> _individuals;
	double _fitness = DBL_MAX;
};

struct algorithm {
public:
	model& get_model();
	void init(population&);
	void select(population&);
	void crossover(population&);
	void mutate(population&);
	void evolve();
  	static void compute_fitness(population&);
	static bool is_converged(population&);
	static void print(population&);
private:
	model _model;
};

}