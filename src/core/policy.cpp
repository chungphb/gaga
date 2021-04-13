#include <ga/core/ga.h>
#include <ga/core/policy.h>

namespace ga {

double default_fitness_function::operator()(individual&) {
	return 0;
}

void default_initialization_method::operator()(population&) {
	return;
}

void default_selection_method::operator()(population&) {
	return;
}

void default_crossover_method::operator()(population&) {
	return;
}

void default_mutation_method::operator()(population&) {
	return;
}

}
