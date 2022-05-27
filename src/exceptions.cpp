#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
std::string
integration_error(Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::integration_error(std::string(what_[0]), result);
    } else {
      throw integratecpp::integration_error(result);
    }
  } catch (const integratecpp::integration_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string
max_subdivision_error(Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::max_subdivision_error(std::string(what_[0]), result);
    } else {
      throw integratecpp::max_subdivision_error(result);
    }
  } catch (const integratecpp::max_subdivision_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string
roundoff_error(Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::roundoff_error(std::string(what_[0]), result);
    } else {
      throw integratecpp::roundoff_error(result);
    }
  } catch (const integratecpp::roundoff_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string
bad_integrand_error(Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::bad_integrand_error(std::string(what_[0]), result);
    } else {
      throw integratecpp::bad_integrand_error(result);
    }
  } catch (const integratecpp::bad_integrand_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string extrapolation_roundoff_error(
    Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::extrapolation_roundoff_error(std::string(what_[0]),
                                                       result);
    } else {
      throw integratecpp::extrapolation_roundoff_error(result);
    }
  } catch (const integratecpp::extrapolation_roundoff_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string
divergence_error(Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::divergence_error(std::string(what_[0]), result);
    } else {
      throw integratecpp::divergence_error(result);
    }
  } catch (const integratecpp::divergence_error &e) {
    return std::string(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
std::string
invalid_input_error(Rcpp::Nullable<Rcpp::CharacterVector> what = R_NilValue) {
  integratecpp::integrator::result_type result{};
  try {
    if (what.isNotNull()) {
      const auto what_ = Rcpp::CharacterVector(what);
      throw integratecpp::invalid_input_error(std::string(what_[0]), result);
    } else {
      throw integratecpp::invalid_input_error(result);
    }
  } catch (const integratecpp::invalid_input_error &e) {
    return std::string(e.what());
  }
}
