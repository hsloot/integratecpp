#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
std::string max_subdivision_error(std::string what) {
  integratecpp::integrator::result_type result{};
  try {
    throw integratecpp::max_subdivision_error(what, result);
  } catch (const integratecpp::max_subdivision_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string roundoff_error(std::string what) {
  integratecpp::integrator::result_type result{};
  try {
    throw integratecpp::roundoff_error(what, result);
  } catch (const integratecpp::roundoff_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string bad_integrand_error(std::string what) {
  integratecpp::integrator::result_type result{};
  try {
    throw integratecpp::bad_integrand_error(what, result);
  } catch (const integratecpp::bad_integrand_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string extrapolation_roundoff_error(std::string what) {
  integratecpp::integrator::result_type result{};
  try {
    throw integratecpp::extrapolation_roundoff_error(what, result);
  } catch (const integratecpp::extrapolation_roundoff_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string divergence_error(std::string what) {
  integratecpp::integrator::result_type result{};
  try {
    throw integratecpp::divergence_error(what, result);
  } catch (const integratecpp::divergence_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string invalid_input_error(std::string what) {
  integratecpp::integrator::result_type result{};
  try {
    throw integratecpp::invalid_input_error(what, result);
  } catch (const integratecpp::invalid_input_error &e) {
    return std::string(e.what());
  }
}
