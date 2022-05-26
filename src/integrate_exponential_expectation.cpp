#include <cmath>
#include <limits>
#include <stdexcept>

#include <Rcpp.h>

#include "integratecpp.h"

// [[Rcpp::export(rng=false)]]
Rcpp::List integrate_exponential_expectation(const double lambda) {
  integratecpp::integrator default_integrator{};
  auto fn = [lambda](const double x) {
    return x * lambda * std::exp(-lambda * x);
  };

  try {
    const auto result =
        default_integrator(fn, 0., std::numeric_limits<double>::infinity());
    return Rcpp::List::create(Rcpp::Named("value") = result.value(),
                              Rcpp::Named("abserr") = result.abserr(),
                              Rcpp::Named("subdivisions") =
                                  result.subdivisions(),
                              Rcpp::Named("neval") = result.neval());

  } catch (const integratecpp::integration_error &e) {
    const auto result = e.result();

    Rcpp::warning(e.what());
    return Rcpp::List::create(Rcpp::Named("value") = result.value(),
                              Rcpp::Named("abserr") = result.abserr(),
                              Rcpp::Named("subdivisions") =
                                  result.subdivisions(),
                              Rcpp::Named("neval") = result.neval());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what());
  }
}
