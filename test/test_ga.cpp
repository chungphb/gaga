#include <ga/core/ga.h>

size_t get_random(size_t min, size_t max) {
	std::random_device rd;
	std::mt19937 mt{rd()};
	std::uniform_int_distribution<size_t> uid{min, max};
	return uid(mt);
}

int main(int argc, char** argv) {
	ga::algorithm alg;
	ga::model<>& model = alg.get_model();
	model.add_gene<int>({0, 1, 2});
	model.add_gene<int>({0, 1, 2});
	model.add_gene<int>({0, 1, 2});
	model.register_fitness_cb([](ga::individual& ind) {
		return 6 - (ind.get_gene<int>(0) + ind.get_gene<int>(1) + ind.get_gene<int>(2));
	});
	model.set_population_size(16);
	model.set_mutation_rate(12);
	model.register_initialization_cb([&model](ga::population& ppl) {
		const size_t chr_l = model.get_chromosome_length();
		const size_t ppl_s = model.get_population_size();
		for (size_t i = 0; i < ppl_s; i++) {
			ga::individual ind{model};
			for (size_t j = 0; j < chr_l; j++) {
				const auto& alleles = model.get_alleles<int>(j);
				auto allele = alleles[get_random(0, alleles.size() - 1)];
				ind.add_gene<int>(allele);
			}
			ppl.add_individual(std::move(ind));
		}
	});
	model.register_selection_cb([&model](ga::population& ppl) {
		ppl.remove_individuals_backward(model.get_population_size() >> 1);
	});
	model.register_crossover_cb([&model](ga::population& ppl) {
		size_t n_pairs = ppl.size() >> 1;
		for (size_t i = 0; i < n_pairs; i++) {
			ga::individual& p_1 = ppl.get_individual((i << 1));
			ga::individual& p_2 = ppl.get_individual((i << 1) + 1);
			ga::individual c_1{model};
			c_1.add_gene<int>(p_1.get_gene<int>(0));
			c_1.add_gene<int>(p_2.get_gene<int>(1));
			c_1.add_gene<int>(p_2.get_gene<int>(2));
			ppl.add_individual(std::move(c_1));
			ga::individual c_2{model};
			c_2.add_gene<int>(p_2.get_gene<int>(0));
			c_2.add_gene<int>(p_1.get_gene<int>(1));
			c_2.add_gene<int>(p_1.get_gene<int>(2));
			ppl.add_individual(std::move(c_2));
		}
	});
	model.register_mutation_cb([&model](ga::population& ppl) {
		const size_t n_mutated = static_cast<size_t>(ppl.size() * model.get_mutation_rate() / 100);
		const size_t chr_l = model.get_chromosome_length();
		const size_t ppl_s = ppl.size();
		for (size_t i = 0; i < n_mutated; i++) {
			auto ind_id = get_random(0, ppl_s - 1);
			auto gene_id = get_random(0, chr_l  - 1);
			const auto& alleles = model.get_alleles<int>(gene_id);
			auto allele_id = get_random(0, alleles.size() - 1);
			auto& ind = ppl.get_individual(ind_id);
			ind.set_gene<int>(gene_id, alleles[allele_id]);
		}
	});
	alg.evolve();
}
