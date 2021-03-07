#pragma once

#include <iostream>
#include <vector>

#include <ga/core/gene.h>

namespace ga {

struct alleles {
    virtual ~alleles() = default;
};

template <typename gene_t>
struct alleles_impl : public alleles {
public:
	alleles_impl() = default;
    explicit alleles_impl(const std::vector<gene_t>& values) : _alleles{std::move(values)} {}
	alleles_impl(const alleles_impl&) = default;
	alleles_impl(alleles_impl&&) = default;
	alleles_impl& operator=(const alleles_impl&) = default;
	alleles_impl& operator=(alleles_impl&&) = default;
	~alleles_impl() override = default;
	const std::vector<gene_t>& get_alleles() const {
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
	gene_impl() = default;
    explicit gene_impl(gene_t value) : _value{value} {}
	gene_impl(const gene_impl&) = default;
	gene_impl(gene_impl&&) = default;
	gene_impl& operator=(const gene_impl&) = default;
	gene_impl& operator=(gene_impl&&) = default;
	~gene_impl() override = default;
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
inline std::ostream& operator<<(std::ostream& os, gene_impl<gene_t> g) {
	os << g.get_value();
	return os;
}

}