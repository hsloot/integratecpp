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
Rcpp::List Rcpp__integrate(Rcpp::Function fn, const double lower,
                           const double upper, const int max_subdivisions,
                           const double relative_accuracy,
                           const double absolute_accuracy,
                           const int work_size) {
  auto fn_ = [&fn](const double x) { return Rcpp::as<double>(fn(x)); };
  decltype(integratecpp::integrate(fn_, lower, upper)) result;
  std::string message;
  try {
    auto cfg = integratecpp::integrator::config_type{
        max_subdivisions, relative_accuracy, absolute_accuracy, work_size};
    result = integratecpp::integrate(fn_, lower, upper, std::move(cfg));
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
