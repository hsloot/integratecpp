#include <cmath>
#include <limits>
#include <stdexcept>

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
Rcpp::List Rcpp_integrate(Rcpp::Function fn, const double lower,
                          const double upper, const int subdivisions,
                          const double epsrel, const double epsabs) {
  auto fn_ = [&fn](const double x) { return Rcpp::as<double>(fn(x)); };
  const auto cfg = integratecpp::integrator::config_type{
      subdivisions, epsrel, epsabs, 4 * subdivisions};
  try {
    const auto result = integratecpp::integrate(fn_, lower, upper, cfg);
    return Rcpp::List::create(Rcpp::Named("value") = result.value(),
                              Rcpp::Named("abs.error") = result.abserr(),
                              Rcpp::Named("subdivisions") =
                                  result.subdivisions(),
                              Rcpp::Named("message") = "OK");
  } catch (const integratecpp::integration_error &e) {
    const auto result = integratecpp::integrate(fn_, lower, upper, cfg);
    return Rcpp::List::create(Rcpp::Named("value") = result.value(),
                              Rcpp::Named("abs.error") = result.abserr(),
                              Rcpp::Named("subdivisions") =
                                  result.subdivisions(),
                              Rcpp::Named("message") = e.what());
  }
}
