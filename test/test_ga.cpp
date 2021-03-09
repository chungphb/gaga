#include <ga/core/ga.h>

size_t get_random(size_t min, size_t max) {
	std::random_device rd;
	std::mt19937 mt{rd()};
	std::uniform_int_distribution<size_t> uid{min, max};
	return uid(mt);
}

int main(int argc, char** argv) {
	ga::algorithm alg;
	ga::default_model& model = alg.get_model();

	model.create_gene<int>("x", {0, 1, 2});
	model.create_gene<int>("y", {0, 1, 2});
	model.create_gene<int>("z", {0, 1, 2});

	model.register_fitness_cb([](ga::individual& ind) {
		return 6 - (ind.get_gene<int>("x") + ind.get_gene<int>("y") + ind.get_gene<int>("y"));
	});

	model.set_population_size(16);
	model.set_mutation_rate(12);

	model.register_initialization_cb([&model](ga::population& ppl) {
		const auto ppl_size = model.get_population_size();
		const auto& genes = model.get_gene_list();
		for (size_t i = 0; i < ppl_size; i++) {
			ga::individual ind{model};
			for (const auto& gene : genes) {
				const auto& alleles = model.get_alleles_of_gene<int>(gene);
				auto allele = alleles[get_random(0, alleles.size() - 1)];
				ind.set_gene<int>(gene, allele);
			}
			ppl.add_individual(std::move(ind));
		}
	});

	model.register_selection_cb([&model](ga::population& ppl) {
		ppl.remove_individuals_backward(model.get_population_size() >> 1);
	});

	model.register_crossover_cb([&model](ga::population& ppl) {
		const auto n_pairs = ppl.size() >> 1;
		for (size_t i = 0; i < n_pairs; i++) {
			ga::individual& p_1 = ppl.get_individual((i << 1));
			ga::individual& p_2 = ppl.get_individual((i << 1) + 1);
			ga::individual c_1{model};
			c_1.set_gene<int>("x", p_1.get_gene<int>("x"));
			c_1.set_gene<int>("y", p_2.get_gene<int>("y"));
			c_1.set_gene<int>("z", p_2.get_gene<int>("z"));
			ppl.add_individual(std::move(c_1));
			ga::individual c_2{model};
			c_2.set_gene<int>("x", p_2.get_gene<int>("x"));
			c_2.set_gene<int>("y", p_1.get_gene<int>("y"));
			c_2.set_gene<int>("z", p_1.get_gene<int>("z"));
			ppl.add_individual(std::move(c_2));
		}
	});

	model.register_mutation_cb([&model](ga::population& ppl) {
		const auto n_mutated = static_cast<size_t>(ppl.size() * model.get_mutation_rate() / 100);
		const auto ppl_size = ppl.size();
		const auto& genes = model.get_gene_list();
		const auto chr_len = model.get_chromosome_length();
		for (size_t i = 0; i < n_mutated; i++) {
			const auto& rand_gene = genes[get_random(0, chr_len - 1)];
			const auto& alleles = model.get_alleles_of_gene<int>(rand_gene);
			const auto& rand_allele = alleles[get_random(0, alleles.size() - 1)];
			auto& rand_ind = ppl.get_individual(get_random(0, ppl_size - 1));
			rand_ind.set_gene<int>(rand_gene, rand_allele);
		}
	});

	alg.evolve();
}