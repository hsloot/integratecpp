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

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
Rcpp::XPtr<integratecpp::integrator>
Rcpp__integrator__new(const int max_subdivisions,
                      const double relative_accuracy,
                      const double absolute_accuracy, const int work_size) {
  return Rcpp::XPtr<integratecpp::integrator>(new integratecpp::integrator{
      max_subdivisions, relative_accuracy, absolute_accuracy, work_size});
}

// [[Rcpp::export(rng=false)]]
int Rcpp__integrator__get_max_subdivisions(
    Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->max_subdivisions();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_max_subdivisions(
    Rcpp::XPtr<integratecpp::integrator> ptr, const int max_subdivisions) {
  ptr->max_subdivisions(max_subdivisions);
}

// [[Rcpp::export(rng=false)]]
double Rcpp__integrator__get_relative_accuracy(
    Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->relative_accuracy();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_relative_accuracy(
    Rcpp::XPtr<integratecpp::integrator> ptr, const double relative_accuracy) {
  ptr->relative_accuracy(relative_accuracy);
}

// [[Rcpp::export(rng=false)]]
double Rcpp__integrator__get_absolute_accuracy(
    Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->absolute_accuracy();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_absolute_accuracy(
    Rcpp::XPtr<integratecpp::integrator> ptr, const double absolute_accuracy) {
  ptr->absolute_accuracy(absolute_accuracy);
}

// [[Rcpp::export(rng=false)]]
int Rcpp__integrator__get_work_size(Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->work_size();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_work_size(Rcpp::XPtr<integratecpp::integrator> ptr,
                                     const int work_size) {
  ptr->work_size(work_size);
}

// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__throw_if_invalid(
    Rcpp::XPtr<integratecpp::integrator> ptr) {
  try {
    // NOTE: avoid code duplication using a dummy integration. the integration
    // itself should not fail under any circumstances if configuration
    // parameters are valid.
    (*ptr)([](const double) { return 0.; }, 0., 1.);
  } catch (::Rcpp::exception &e) {                       // # nocov
    Rcpp::stop("Not initialized");                       // # nocov
  } catch (const integratecpp::invalid_input_error &e) { // # nocov
    Rcpp::stop(e.what());                                // # nocov
  } catch (...) {                                        // # nocov
    Rcpp::stop("Unknown error");                         // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
Rcpp::List Rcpp__integrator__integrate(Rcpp::XPtr<integratecpp::integrator> ptr,
                                       Rcpp::Function fn, const double lower,
                                       const double upper) {
  auto fn_ = [&fn](const double x) { return Rcpp::as<double>(fn(x)); };
  decltype(integratecpp::integrate(fn_, lower, upper)) result;
  std::string message;
  try {
    result = (*ptr)(fn_, lower, upper);
    message = "OK";
  } catch (const Rcpp::exception &e) {
    Rcpp::stop(e.what());
  } catch (const integratecpp::integration_runtime_error &e) {
    result = e.result();
    message = e.what();
  } catch (const integratecpp::integration_logic_error &e) {
    result = e.result();
    message = e.what();
  } catch (const std::exception &e) {
    Rcpp::stop(e.what()); // # nocov
  } catch (...) {
    Rcpp::stop("Unexcpected error"); // # nocov
  }
  return Rcpp::List::create(Rcpp::Named("value") = result.value,
                            Rcpp::Named("abs.error") = result.absolute_error,
                            Rcpp::Named("subdivisions") = result.subdivisions,
                            Rcpp::Named("message") = message);
}
