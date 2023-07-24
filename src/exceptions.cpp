// Copyright (C) 2022 Henrik Sloot
//
// This file is part of integratecpp
//
// integratecpp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// integratecpp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#define RCPP_NO_UNWIND_PROTECT 1
#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
std::string Rcpp__integration_logic_error__catch_what(std::string what) {
  try {
    throw integratecpp::integration_logic_error(what);
  } catch (const integratecpp::integration_logic_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__integration_runtime_error__catch_what(std::string what) {
  try {
    throw integratecpp::integration_runtime_error(what);
  } catch (const integratecpp::integration_runtime_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__max_subdivision_error__catch_what(std::string what) {
  try {
    throw integratecpp::max_subdivision_error(what);
  } catch (const integratecpp::max_subdivision_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__roundoff_error__catch_what(std::string what) {
  try {
    throw integratecpp::roundoff_error(what);
  } catch (const integratecpp::roundoff_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__bad_integrand_error__catch_what(std::string what) {
  try {
    throw integratecpp::bad_integrand_error(what);
  } catch (const integratecpp::bad_integrand_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__extrapolation_roundoff_error__catch_what(std::string what) {
  try {
    throw integratecpp::extrapolation_roundoff_error(what);
  } catch (const integratecpp::extrapolation_roundoff_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__divergence_error__catch_what(std::string what) {
  try {
    throw integratecpp::divergence_error(what);
  } catch (const integratecpp::divergence_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
std::string Rcpp__invalid_input_error__catch_what(std::string what) {
  try {
    throw integratecpp::invalid_input_error(what);
  } catch (const integratecpp::invalid_input_error &e) {
    return std::string(e.what());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unknown error"); // # nocov
  }
}
