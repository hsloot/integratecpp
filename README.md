
<!-- README.md is generated from README.Rmd. Please edit that file -->

# integratecpp

<!-- badges: start -->

[![Codecov test
coverage](https://codecov.io/gh/hsloot/integratecpp/branch/main/graph/badge.svg)](https://app.codecov.io/gh/hsloot/integratecpp?branch=main)
<!-- badges: end -->

The package `integratecpp` provides a header-only interface to `R`’s
numerical integration methods.

## Installation

You can install the development version of `integratecpp` like so:

``` r
remotes::install_github("hsloot/integratecpp")
```

To include the header into your `C++` source files for building with
`Rcpp`, use

``` cpp
// C++
// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(integratecpp)]]

#include <integratecpp.h>

// your code
```

If you intend including the header in source files of an `R`-package,
you should drop the `Rcpp` attributes and include the following line in
your `DESCRIPTION` file:

``` batch
LinkingTo: integratecpp
```

Note that the header does includes only C++ standard library headers and
[`<R_ext/Applic.h>`](https://github.com/wch/r-source/blob/trunk/src/include/R_ext/Applic.h).

## Why is this useful?

Many `R` package authors implement critical parts in `C`, `Fortran` or
`C++` to improve performance. However, while `R` provides an [API for
`C`](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#The-R-API)
and it is possible to [mix `C` and
`C++`](https://isocpp.org/wiki/faq/mixing-c-and-cpp), doing this can
pose a higher burden for those more familiar with `R` and `Rcpp` than
`C++` or `C`. Consider the following example, approximating the integral
of the identity function
![x \\mapsto x](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;x%20%5Cmapsto%20x "x \mapsto x")
over the interval
![\[0, 1\]](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5B0%2C%201%5D "[0, 1]"):

``` cpp
// C++

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(integratecpp)]]

#include <algorithm> // for std::transform

#include <Rcpp.h>
#include <R_ext/Applic.h>

// [[Rcpp::export(rng=false)]]
Rcpp::List integrate_identity() {
  // define function as lambda
  auto fn = [](const double x) {
    return x;
  };

  // define bounds
  double lower = 0.;
  double upper = 1.;

  // initialize output variables for integration results
  double result;
  double abserr;
  int last;
  int neval;

  // initialize configuration parameters
  int limit = 100;
  double epsrel = 0.0001220703125;
  double epsabs = epsrel;
  int lenw = 4 * limit;

  // initialize working array
  auto iwork = std::vector<int>(limit);
  auto work = std::vector<double>(lenw);

  // initialize variable for error code
  int ier = 0;

  // `Rdqagi` requires a function pointer with signature
  // `void(*)(double *, int, void *)` and a void pointer
  // `void *` for the last argument.
  const auto fn_callback = [](double *x, int n, void *ex) {
    auto& fn_integrand = *static_cast<decltype(&fn)>(ex);
    std::transform(&x[0], &x[n], &x[0], fn_integrand);
    return;
  };

  // NOTE: finite bounds can be integrated with the `C`-method `Rdqags`,
  // requiring no extra boundary transformation.
  Rdqags(fn_callback, &fn, &lower, &upper, &epsabs, &epsrel, &result,
         &abserr, &neval, &ier, &limit, &lenw, &last, iwork.data(),
         work.data());

  return Rcpp::List::create(
      Rcpp::Named("value") = result,
      Rcpp::Named("absolute_error") = abserr,
      Rcpp::Named("subdivisions") = last,
      Rcpp::Named("neval") = neval);
}
```

``` r
integrate_identity()
#> $value
#> [1] 0.5
#> 
#> $absolute_error
#> [1] 5.551115e-15
#> 
#> $subdivisions
#> [1] 1
#> 
#> $neval
#> [1] 21
```

The key part for adhering to the interface of `Rdqags` is creating the
callback functor `fn_callback`, taking a `void *` pointer to the
original function, which is than internally casted to the correct type
and which overwrites an array of double with corresponding function
evaluations. This is rather complicated and requires being more
familiarity with pointers. Additionally, this snippet is missing a
translation of the error code into a proper error message. To make it
worse, without guarding the callback functions from `C++` exceptions, we
introduce possible undefined behavior.

Using our wrapper simplifies numerical integration in `Rcpp`
considerably:

``` cpp
// C++

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(integratecpp)]]

#include <stdexcept>
#include <exception>

#include <Rcpp.h>
#include <integratecpp.h>

// [[Rcpp::export(rng=false)]]
Rcpp::List integrate_identity_improved() {
  auto fn = [](const double x) {
    return x;
  };

  try {
    const auto result = integratecpp::integrate(fn, 0., 1.);
    return Rcpp::List::create(
        Rcpp::Named("value") = result.value,
        Rcpp::Named("absolute_error") = result.absolute_error,
        Rcpp::Named("subdivisions") = result.subdivisions,
        Rcpp::Named("neval") = result.neval);
  } catch (const std::exception& e) {
    Rcpp::stop(e.what());
  }
}

// [[Rcpp::export(rng=false)]]
Rcpp::List integrate_identity_error() {
  auto fn = [](const double x) {
    throw std::runtime_error("stop on purpose");
    return x;
  };

  try {
    const auto result = integratecpp::integrate(fn, 0., 1.);
    return Rcpp::List::create(
        Rcpp::Named("value") = result.value,
        Rcpp::Named("absolute_error") = result.absolute_error,
        Rcpp::Named("subdivisions") = result.subdivisions,
        Rcpp::Named("neval") = result.neval);
  } catch (const std::exception& e) {
    Rcpp::stop(e.what());
  }
}
```

``` r
integrate_identity_improved()
#> $value
#> [1] 0.5
#> 
#> $absolute_error
#> [1] 5.551115e-15
#> 
#> $subdivisions
#> [1] 1
#> 
#> $neval
#> [1] 21
tryCatch(integrate_identity_error(), error = function(cond) print(cond))
#> <Rcpp::exception in integrate_identity_error(): stop on purpose>
```

## Code of Conduct

Please note that the integratecpp project is released with a
[Contributor Code of
Conduct](https://contributor-covenant.org/version/2/0/CODE_OF_CONDUCT.html).
By contributing to this project, you agree to abide by its terms.
