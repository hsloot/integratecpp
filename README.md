
<!-- README.md is generated from README.Rmd. Please edit that file -->

# integratecpp

<!-- badges: start -->

[![Codecov test
coverage](https://codecov.io/gh/hsloot/integratecpp/branch/main/graph/badge.svg)](https://app.codecov.io/gh/hsloot/integratecpp?branch=main)
[![](https://img.shields.io/badge/pkgdown-integratecpp-blue.svg)](https://hsloot.github.io/integratecpp/)
[![](https://img.shields.io/badge/Doxygen-integratecpp-blue.svg)](https://hsloot.github.io/integratecpp/html/index.html)
<!-- badges: end -->

The package `integratecpp` provides a header-only `C++11` interface to
`R`’s `C`-API for numerical integration.

## Installation

You can install the development version of `integratecpp` like so:

``` r
# R
 
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
you should drop the `Rcpp` attributes and include the following lines in
your `DESCRIPTION` file:

``` deb-control
LinkingTo: integratecpp
SystemRequirements: C++11
```

Addtionally, if you are using an `Makevars` or `Makevars.win` file,
[*WRE*](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#Using-C_002b_002b-code)
suggests adding the following:

``` makefile
CXX_STD = CXX11
```

Note that the header does includes only C++ standard library headers and
[`<R_ext/Applic.h>`](https://github.com/wch/r-source/blob/trunk/src/include/R_ext/Applic.h).

## Why is this useful?

Many `R` package authors implement critical parts in `C`, `Fortran` or
`C++` to improve performance. However, while `R` provides an [API for
`C`](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#The-R-API)
and it is possible to [mix `C` and
`C++`](https://isocpp.org/wiki/faq/mixing-c-and-cpp), using the `C`-API
in `C++` code can pose a higher burden for those more familiar with `R`
and `Rcpp` than `C++` or `C`.

Consider the following example, approximating the integral of the
identity function
![x \mapsto x](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;x%20%5Cmapsto%20x "x \mapsto x")
over the interval
![\[0, 1\]](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5B0%2C%201%5D "[0, 1]"):

``` cpp
// C++

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(integratecpp)]]

#include <algorithm> // std::transform

#include <Rcpp.h>
#include <R_ext/Applic.h>

// [[Rcpp::export(rng=false)]]
Rcpp::List integrate_identity() {
    // define integrand as lambda
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

    // NOTE: `Rdqags` requires a function pointer with signature
    // `void(*)(double *, int, void *)` and a void pointer
    // `void *` passed to the callback as the last argument
    const auto fn_callback = [](double *x, int n, void *ex) {
        auto& fn_integrand = *static_cast<decltype(&fn)>(ex);
        std::transform(&x[0], &x[n], &x[0], fn_integrand);
        return;
    };

    // NOTE: finite bounds can be integrated with the `C`-method `Rdqags`
    Rdqags(fn_callback, &fn, &lower, &upper, &epsabs, &epsrel, &result,
         &abserr, &neval, &ier, &limit, &lenw, &last, iwork.data(),
         work.data());

    return Rcpp::List::create(
        Rcpp::Named("value") = result,
        Rcpp::Named("absolute_error") = abserr,
        Rcpp::Named("subdivisions") = last,
        Rcpp::Named("neval") = neval
    );
}
```

``` r
# R

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
and is used to overwrite an array of double with corresponding function
evaluations. This is rather complicated and requires being more
familiarity with pointers. Additionally, this snippet is missing a
translation of the error code into a proper error message. To make it
worse, without guarding the callback functions from `C++` exceptions, we
introduce possible undefined behavior.

Using the wrapper solves these problems and simplifies numerical
integration in `Rcpp` considerably:

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

    const auto result = integratecpp::integrate(fn, 0., 1.);
    return Rcpp::List::create(
        Rcpp::Named("value") = result.value,
        Rcpp::Named("absolute_error") = result.absolute_error,
        Rcpp::Named("subdivisions") = result.subdivisions,
        Rcpp::Named("neval") = result.neval
    );
}

// [[Rcpp::export(rng=false)]]
Rcpp::List integrate_identity_error() {
    auto fn = [](const double x) {
        throw std::runtime_error("stop on purpose");
        return x;
    };

    const auto result = integratecpp::integrate(fn, 0., 1.);
    return Rcpp::List::create(
        Rcpp::Named("value") = result.value,
        Rcpp::Named("absolute_error") = result.absolute_error,
        Rcpp::Named("subdivisions") = result.subdivisions,
        Rcpp::Named("neval") = result.neval
    );
}
```

``` r
# R

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
#> <std::runtime_error in integrate_identity_error(): stop on purpose>
```

## Alternatives

There are alternatives to using `integratecpp` or `R`’s `C`-API for
numerical integration in `C++` code of `R` packages. Two examples are:

-   Linking to the [GNU scientific
    library](https://www.gnu.org/software/gsl/), possibly using
    [`RcppGSL`](https://github.com/eddelbuettel/rcppgsl).
-   Using [`RcppNumerical`](https://github.com/yixuan/RcppNumerical),
    which provides a similar approach to ours.

Both approaches provide a finer control over the specific integration
algorithms than `R` does. The following table provides a summary.

| **Approach**    | **Depends** | **Imports**  | **LinkingTo**       | **SystemRequirements** | **External dependency** | **Additional features** |
|:----------------|-------------|--------------|---------------------|------------------------|-------------------------|-------------------------|
| `integratecpp`  | `R >= 3.1`  | (`Rcpp`[^1]) | (`Rcpp`)            | `C++11`                |                         | ❌                      |
| `C`-API         |             |              |                     |                        |                         | ❌                      |
| `gsl`           |             | (`Rcpp`)     | (`Rcpp`, `RcppGSL`) |                        | `gsl`                   | ✅                      |
| `RcppNumerical` |             | `Rcpp`       | `Rcpp, RcppEigen`   |                        |                         | ✅                      |

What separates our approach are zero additional dependencies (if
vendored) and an intuitive pure `C++` API which does not rely on `Rcpp`
itself. Hence, as `Rdqags` and `Rdqagi` are not using longjumps, it can
be used in a pure `C++` back-end. A comparison of different numerical
integration approaches in `C++` is summarized in the article [“Comparing
numerical integration packages”](articles/comparison.html).

## Outlook

The current version of `integratecpp` has the following shortcommings
which could be adressed in future versions:

-   The current version of `integratecpp` imports and links to `Rcpp` to
    generate test functions which are not exported. Future versions
    might remove this dependency.

-   `R`’s `C`-API allows reusing workspace variables. This feature is
    currently not implemented by our wrapper, i.e., each call to
    `integratecpp::integrate(...)` or
    `integratecpp::integrator::operator()(...)` will create a
    `std::vector<int>` and a `std::vector<double>` of length `limit` and
    `4 * limit`, respectively. Future versions might make workspace
    variables class members of the `integratecpp::integrator` or allow
    to provide them externally.

-   The current version of `integratecpp` is licenced under `GPL (>=3)`
    due to its dependence and linking to `Rcpp`. The provided
    header-only library depends only on STL headers and
    [`<R_ext/Applic.h>`](https://github.com/wch/r-source/blob/trunk/src/include/R_ext/Applic.h).
    The latter is licensed under `LGPL (>= 2.1)` (see
    [`doc/COPYRIGHTS`](https://github.com/wch/r-source/blob/trunk/doc/COPYRIGHTS)).
    Hence, future versions might relicense the header-only library under
    `LGPL (>= 3)` for permissive linking to `integratecpp`.

Apart from internal improvements, `integratecpp` could provide
convinience functions à la [`usethis`](https://github.com/r-lib/usethis)
to integrate `integratecpp` into package developement. Also a function
to vendor the header into a package under development could be created.

Some of these enhancements can be found in various feature branches on
[github.com/hsloot/integratecpp](https://github.com/hsloot/integratecpp).

## Code of Conduct

Please note that the `integratecpp` project is released with a
[Contributor Code of
Conduct](https://contributor-covenant.org/version/2/0/CODE_OF_CONDUCT.html).
By contributing to this project, you agree to abide by its terms.

[^1]: The current version of `integratecpp` imports and links to `Rcpp`
    for internal testing. This depency might be removed in future
    versions and can be avoided if the header is vendored into the
    project.
