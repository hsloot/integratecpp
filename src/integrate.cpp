#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
Rcpp::List Rcpp__integrate(Rcpp::Function fn, const double lower,
                           const double upper, const int subdivisions,
                           const double epsrel, const double epsabs,
                           const int lenw) {
  auto fn_ = [&fn](const double x) { return Rcpp::as<double>(fn(x)); };
  decltype(integratecpp::integrate(fn_, lower, upper)) result;
  std::string message;
  try {
    auto cfg = integratecpp::integrator::config_type{subdivisions, epsrel,
                                                     epsabs, lenw};
    result = integratecpp::integrate(fn_, lower, upper, std::move(cfg));
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
  return Rcpp::List::create(Rcpp::Named("value") = result.value,
                            Rcpp::Named("abs.error") = result.abserr,
                            Rcpp::Named("subdivisions") = result.subdivisions,
                            Rcpp::Named("message") = message);
}
