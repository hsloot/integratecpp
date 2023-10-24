
<!-- README.md is generated from README.Rmd. Please edit that file -->
<!-- cSpell: words Makevars -->

# integratecpp

<!-- badges: start -->
<!-- markdownlint-capture -->
<!-- markdownlint-disable MD045 -->

[![Codecov test
coverage](https://codecov.io/gh/hsloot/integratecpp/branch/main/graph/badge.svg)](https://app.codecov.io/gh/hsloot/integratecpp?branch=main)
[![](https://img.shields.io/badge/pkgdown-integratecpp-blue.svg)](https://hsloot.github.io/integratecpp/)
[![](https://img.shields.io/badge/Doxygen-integratecpp-blue.svg)](https://hsloot.github.io/integratecpp/html/index.html)
<!-- markdownlint-restore --> <!-- badges: end -->

The package `integratecpp` provides a header-only C++11 interface to R’s
C-API for numerical integration.

## Installation

You can install the development version of `integratecpp` like so:

``` r
# R
remotes::install_github("hsloot/integratecpp")
```

To include the header into your C++ source files for building with
`Rcpp`, use

``` cpp
// C++
// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(integratecpp)]]

#include <integratecpp.h>
// your code
```

If you intend including the header in source files of an R-package, you
should drop the `Rcpp` attributes and include the following lines in
your `DESCRIPTION` file:

``` deb-control
LinkingTo: integratecpp
SystemRequirements: C++11
```

Additionally, if you are using an `Makevars` or `Makevars.win` file,
[*WRE*](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#Using-C_002b_002b-code)
suggests adding the following:

``` makefile
CXX_STD = CXX11
```

Note that the header does includes only C++ standard library headers and
[`<R_ext/Applic.h>`](https://github.com/wch/r-source/blob/trunk/src/include/R_ext/Applic.h).

## How to use it?

Suppose we want to integrate the identity function over the unit
interval. First, we must include the required headers (and `Rcpp`
attributes, if `Rcpp` is used), for example:

``` cpp
#include <integratecpp.h>
```

Second, we have to define the integrand as an object that is invocable
by `const double` and returns `double`, for example a lambda-functor:

``` cpp
auto fn = [](const double x) {
    return x;
};
```

Third, we can use the integrate routine:

``` cpp
const auto result = integratecpp::integrate(fn, 0., 1.);
```

The last part can be enclosed in a try-catch block:

``` cpp
try {
  const auto result = integratecpp::integrate(fn, 0., 1.);
  // ...
} catch (const integratecpp::integration_logic_error &e) {
  // ...
} catch (const integratecpp::integration_runtime_error &e) {
  // ...
}
```

Optional configurations similar to those of `stats::integrate` are also
available if needed. For a more realistic example, see the vignette
[“Using `integratecpp`”](articles/integratecpp-usage.html).

## Why is this useful?

Many R package authors implement critical parts, including numerical
integration, in C, Fortran or C++ to improve performance. However, while
R provides an [API for
C](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#The-R-API)
and it is possible to [mix C and
C++](https://isocpp.org/wiki/faq/mixing-c-and-cpp), using the C-API in
C++ code can pose a higher burden for those more familiar with R and
`Rcpp` than C++ or C.

Using the C-API for the numerical integration routines requires adhering
to the interface of the functions `Rdqags` or `Rdqagi`. This can be done
by creating a callback functor taking a `void *` pointer to the original
function, which is then internally cast to the correct type and is used
to overwrite an array of doubles with corresponding function
evaluations. This is rather complicated and requires being more
familiarity with pointers. Additionally, it requires translating error
codes into a proper error message. To make it worse, not guarding
callback functions against C++ exceptions introduces possible undefined
behavior.

## Alternatives

There are alternatives to using `integratecpp` or R’s C-API for
numerical integration in C++ code of R packages. Two examples are:

- Linking to the [GNU scientific
  library](https://www.gnu.org/software/gsl/), possibly using
  [`RcppGSL`](https://github.com/eddelbuettel/rcppgsl).
- Using [`RcppNumerical`](https://github.com/yixuan/RcppNumerical),
  which provides a similar approach to ours.

Both approaches provide a finer control over the specific integration
algorithms than R does. The following table provides a summary.

<!-- markdownlint-capture -->
<!-- markdownlint-disable MD013 -->

| **Approach**    | **Depends** | **Imports**  | **LinkingTo**       | **SystemRequirements** | **External dependency** | **Additional features** |
|:----------------|-------------|--------------|---------------------|------------------------|-------------------------|-------------------------|
| `integratecpp`  | `R >= 3.1`  | (`Rcpp`[^1]) | (`Rcpp`)            | C++11                  |                         | ❌                      |
| C-API           |             |              |                     |                        |                         | ❌                      |
| `gsl`           |             | (`Rcpp`)     | (`Rcpp`, `RcppGSL`) |                        | `gsl`                   | ✅                      |
| `RcppNumerical` |             | `Rcpp`       | `Rcpp`, `RcppEigen` |                        |                         | ✅                      |

<!--markdownlint-restore -->

What separates our approach are zero additional dependencies (if
vendored) and an intuitive pure C++ API which does not rely on `Rcpp`
itself. Hence, as `Rdqags` and `Rdqagi` are not using longjumps
*themselves*, our approach can be used in a pure C++ back-end.[^2] A
comparison of different numerical integration approaches in C++ is
summarized in the article [“Comparing numerical integration
packages”](vignettes/web_only/comparison.Rmd).

## Outlook

The current version of `integratecpp` has the following shortcomings
which could be addressed in future versions:

- The current version of `integratecpp` imports and links to `Rcpp` to
  generate test functions which are not exported. Future versions might
  remove this dependency.

- R’s C-API allows reusing workspace variables. This feature is
  currently not implemented by our wrapper, i.e., each call to
  `integratecpp::integrate(...)` or
  `integratecpp::integrator::operator()(...)` will create a
  `std::vector<int>` and a `std::vector<double>` of length `limit` and
  `4 * limit`, respectively. Future versions might make workspace
  variables class members of the `integratecpp::integrator` or allow to
  provide them externally.

- The current version of `integratecpp` is licensed under `GPL (>=3)`
  due to its dependence and linking to `Rcpp`. The provided header-only
  library depends only on STL headers and
  [`<R_ext/Applic.h>`](https://github.com/wch/r-source/blob/trunk/src/include/R_ext/Applic.h).
  The latter is licensed under `LGPL (>= 2.1)` (see
  [`doc/COPYRIGHTS`](https://github.com/wch/r-source/blob/trunk/doc/COPYRIGHTS)).
  Hence, future versions might relicense the header-only library under
  `LGPL (>= 3)` for permissive linking to `integratecpp`.

Apart from internal improvements, `integratecpp` could provide
convenience functions à la [`usethis`](https://github.com/r-lib/usethis)
to integrate `integratecpp` into package development. Also a function to
vendor the header into a package under development could be created.

Some of these enhancements, possibly not up-to-date with the main
branch, can be found in various feature branches on
[github.com/hsloot/integratecpp](https://github.com/hsloot/integratecpp).

## Code of Conduct

Please note that the `integratecpp` project is released with a
[Contributor Code of Conduct](.github/CODE_OF_CONDUCT.md). By
contributing to this project, you agree to abide by its terms.

[^1]: The current version of `integratecpp` imports and links to `Rcpp`
    for internal testing. This dependency might be removed in future
    versions and can be avoided if the header is vendored into the
    project.

[^2]: Note that the provided integrator-function itself should not use
    longjumps (e.g., R functions provided through Rcpp) to exit the
    frame; if you want to use R functions with `Rcpp >= 1.0.10`,
    consider defining `RCPP_NO_UNWIND_PROTECT` before loading the Rcpp
    header file.
