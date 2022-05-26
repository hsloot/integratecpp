
<!-- README.md is generated from README.Rmd. Please edit that file -->

# integratecpp

<!-- badges: start -->
<!-- badges: end -->

The package `integratecpp` provides a header-only interface to `R`’s
numerical integration methods.

## Installation

You can install the development version of integratecpp like so:

``` r
remotes::install_github("hsloot/integratecpp")
```

## Example

This is a basic example which shows you how to solve a common problem:
We want to calculate the expectation of an exponential distribution with
rate
![\\lambda](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Clambda "\lambda")
by numerical integration. The theoretical expectation is

![
  \\int\_{0}^{\\infty}{ x \\lambda \\exp{\\{ -\\lambda x \\}} } \\mathrm{d}x
    = \\frac{1}{\\lambda} .
](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%0A%20%20%5Cint_%7B0%7D%5E%7B%5Cinfty%7D%7B%20x%20%5Clambda%20%5Cexp%7B%5C%7B%20-%5Clambda%20x%20%5C%7D%7D%20%7D%20%5Cmathrm%7Bd%7Dx%0A%20%20%20%20%3D%20%5Cfrac%7B1%7D%7B%5Clambda%7D%20.%0A "
  \int_{0}^{\infty}{ x \lambda \exp{\{ -\lambda x \}} } \mathrm{d}x
    = \frac{1}{\lambda} .
")

``` cpp
// C++

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(integratecpp)]]

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
                              Rcpp::Named("subdivisions") = result.subdivisions(),
                              Rcpp::Named("neval") = result.neval());

  } catch (const integratecpp::integration_error &e) {
    const auto result = e.result();
    Rcpp::warning(e.what());
    return Rcpp::List::create(Rcpp::Named("value") = result.value(),
                              Rcpp::Named("abserr") = result.abserr(),
                              Rcpp::Named("subdivisions") = result.subdivisions(),
                              Rcpp::Named("neval") = result.neval());
  } catch (const std::exception &e) {
    Rcpp::stop(e.what());
  }
}
```

``` r
## R

integrate_exponential_expectation(1)
#> $value
#> [1] 1
#> 
#> $abserr
#> [1] 6.357144e-06
#> 
#> $subdivisions
#> [1] 4
#> 
#> $neval
#> [1] 105
integrate_exponential_expectation(2)
#> $value
#> [1] 0.5
#> 
#> $abserr
#> [1] 8.604832e-06
#> 
#> $subdivisions
#> [1] 3
#> 
#> $neval
#> [1] 75
integrate_exponential_expectation(3)
#> $value
#> [1] 0.3333333
#> 
#> $abserr
#> [1] 8.06807e-08
#> 
#> $subdivisions
#> [1] 3
#> 
#> $neval
#> [1] 75

integrate_exponential_expectation(-1)
#> Warning in integrate_exponential_expectation(-1): roundoff error was detected
#> $value
#> [1] -Inf
#> 
#> $abserr
#> [1] NaN
#> 
#> $subdivisions
#> [1] 41
#> 
#> $neval
#> [1] 1215
```

## Code of Conduct

Please note that the integratecpp project is released with a
[Contributor Code of
Conduct](https://contributor-covenant.org/version/2/0/CODE_OF_CONDUCT.html).
By contributing to this project, you agree to abide by its terms.
