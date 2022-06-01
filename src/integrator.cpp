#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
Rcpp::XPtr<integratecpp::integrator> Rcpp__integrator__new(const int limit,
                                                           const double epsrel,
                                                           const double epsabs,
                                                           const int lenw) {
  return Rcpp::XPtr<integratecpp::integrator>(
      new integratecpp::integrator{limit, epsrel, epsabs, lenw});
}

// [[Rcpp::export(rng=false)]]
int Rcpp__integrator__get_limit(Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->limit();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_limit(Rcpp::XPtr<integratecpp::integrator> ptr,
                                 const int limit) {
  ptr->limit(limit);
}

// [[Rcpp::export(rng=false)]]
double Rcpp__integrator__get_epsrel(Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->epsrel();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_epsrel(Rcpp::XPtr<integratecpp::integrator> ptr,
                                  const double epsrel) {
  ptr->epsrel(epsrel);
}

// [[Rcpp::export(rng=false)]]
double Rcpp__integrator__get_epsabs(Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->epsabs();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_epsabs(Rcpp::XPtr<integratecpp::integrator> ptr,
                                  const double epsabs) {
  ptr->epsabs(epsabs);
}

// [[Rcpp::export(rng=false)]]
int Rcpp__integrator__get_lenw(Rcpp::XPtr<integratecpp::integrator> ptr) {
  return ptr->lenw();
}
// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__set_lenw(Rcpp::XPtr<integratecpp::integrator> ptr,
                                const int lenw) {
  ptr->lenw(lenw);
}

// [[Rcpp::export(rng=false)]]
bool Rcpp__integrator__is_valid(Rcpp::XPtr<integratecpp::integrator> ptr) {
  try {
    return (*ptr).is_valid();
  } catch (::Rcpp::exception &e) { // # nocov
    return false;                  // # nocov
  } catch (...) {                  // # nocov
    Rcpp::stop("Unknown error");   // # nocov
  }
}

// [[Rcpp::export(rng=false)]]
void Rcpp__integrator__assert_validity(
    Rcpp::XPtr<integratecpp::integrator> ptr) {
  try {
    (*ptr).assert_validity();
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
  return Rcpp::List::create(Rcpp::Named("value") = result.value(),
                            Rcpp::Named("abs.error") = result.abserr(),
                            Rcpp::Named("subdivisions") = result.subdivisions(),
                            Rcpp::Named("message") = message);
}
