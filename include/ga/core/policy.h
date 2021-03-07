#pragma once

namespace ga {

struct population;
struct individual;

struct default_encoding_method {};

struct default_fitness_function {
    double operator()(individual&);
};

struct default_initialization_method {
    void operator()(population&);
};

struct default_selection_method {
    void operator()(population&);
};

struct default_crossover_method {
    void operator()(population&);
};

struct default_mutation_method {
    void operator()(population&);
};

}