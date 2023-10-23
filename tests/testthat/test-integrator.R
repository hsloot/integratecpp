# Copyright (C) 2023 Henrik Sloot
#
# This file is part of integratecpp
#
# integratecpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# integratecpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# cSpell: ignore behaviour
# cSpell: words meanlog,sdlog

remove_call <- function(x) {
    x <- x[-match(x, "call")]
}

test_that("Parameter default initialization works as expected", {
    integrator <- Integrator()
    expect_equal(integrator$max_subdivisions, 100)
    expect_equal(integrator$relative_accuracy, .Machine$double.eps^0.25)
    expect_equal(integrator$absolute_accuracy, .Machine$double.eps^0.25)
    expect_equal(integrator$work_size, 400)
})

test_that("Parameter custom initialization works as expected", {
    integrator <- Integrator(
        max_subdivisions = 50,
        relative_accuracy = .Machine$double.eps^0.5, absolute_accuracy = 0,
        work_size = 800
    )
    expect_equal(integrator$max_subdivisions, 50)
    expect_equal(integrator$relative_accuracy, .Machine$double.eps^0.5)
    expect_equal(integrator$absolute_accuracy, 0)
    expect_equal(integrator$work_size, 800)
})

test_that("Parameter setting works as expected", {
    integrator <- Integrator()
    integrator$max_subdivisions <- 50
    integrator$relative_accuracy <- .Machine$double.eps^0.5
    integrator$absolute_accuracy <- 0
    integrator$work_size <- 800
    expect_equal(integrator$max_subdivisions, 50)
    expect_equal(integrator$relative_accuracy, .Machine$double.eps^0.5)
    expect_equal(integrator$absolute_accuracy, 0)
    expect_equal(integrator$work_size, 800)
})

test_that("Default settings for exponential distribution's expectation", {
    integrator <- Integrator()
    fn <- function(x, rate = 1) {
        x * dexp(x, rate = rate)
    }

    expect_equal(
        remove_call(integrator$integrate(fn, 0, Inf, rate = 0.5)),
        remove_call(stats::integrate(fn, 0, Inf, rate = 0.5))
    )

    expect_equal(
        remove_call(integrator$integrate(fn, 0, Inf, rate = 1)),
        remove_call(stats::integrate(fn, 0, Inf, rate = 1))
    )

    expect_equal(
        remove_call(integrator$integrate(fn, 0, Inf, rate = 2)),
        remove_call(stats::integrate(fn, 0, Inf, rate = 2))
    )
})

test_that("Default settings for beta distribution's expectation", {
    integrator <- Integrator()
    fn <- function(x, shape1, shape2) {
        x * dbeta(x, shape1 = shape1, shape2 = shape2)
    }

    expect_equal(
        remove_call(integrator$integrate(fn, 0, 1, shape1 = 1, shape2 = 1)),
        remove_call(stats::integrate(fn, 0, 1, shape1 = 1, shape2 = 1))
    )

    expect_equal(
        remove_call(integrator$integrate(fn, 0, 1, shape1 = 0.3, shape2 = 0.4)),
        remove_call(stats::integrate(fn, 0, 1, shape1 = 0.3, shape2 = 0.4))
    )

    expect_equal(
        remove_call(integrator$integrate(fn, 0, 1, shape1 = 1.5, shape2 = 2)),
        remove_call(stats::integrate(fn, 0, 1, shape1 = 1.5, shape2 = 2))
    )
})

test_that("Default settings for negative Weibull distribution's expectation", {
    integrator <- Integrator()
    fn <- function(x, shape, scale = 1) {
        x * dweibull(-x, shape = shape, scale = scale)
    }

    expect_equal(
        remove_call(integrator$integrate(fn, -Inf, 0, shape = 1, scale = 1)),
        remove_call(stats::integrate(fn, -Inf, 0, shape = 1, scale = 1))
    )

    expect_equal(
        remove_call(
            integrator$integrate(fn, -Inf, 0, shape = 0.3, scale = 0.4)
        ),
        remove_call(stats::integrate(fn, -Inf, 0, shape = 0.3, scale = 0.4))
    )

    expect_equal(
        remove_call(integrator$integrate(fn, -Inf, 0, shape = 1.5, scale = 2)),
        remove_call(stats::integrate(fn, -Inf, 0, shape = 1.5, scale = 2))
    )
})

test_that("Less max. subdivisions for gamma distribution's expectation", {
    integrator <- Integrator()
    fn <- function(x, shape, rate = 1) {
        x * dgamma(x, shape = shape, rate = rate)
    }

    integrator$max_subdivisions <- 50
    expect_equal(
        remove_call(integrator$integrate(
            fn, 0, Inf,
            shape = 0.3, rate = 0.5
        )),
        remove_call(stats::integrate(
            fn, 0, Inf,
            shape = 0.3, rate = 0.5,
            subdivisions = 50L
        ))
    )

    expect_equal(
        remove_call(integrator$integrate(
            fn, 0, Inf,
            shape = 0.7, rate = 1.5
        )),
        remove_call(stats::integrate(
            fn, 0, Inf,
            shape = 0.7, rate = 1.5,
            subdivisions = 50L
        ))
    )
})

test_that("More max. subdivisions for chi-square distribution's expectation", {
    integrator <- Integrator()
    fn <- function(x, df, ncp = 0) {
        x * dchisq(x, df = df, ncp = ncp)
    }

    integrator$work_size <- 800L
    integrator$max_subdivisions <- 200L
    expect_equal(
        remove_call(integrator$integrate(fn, 0, Inf, df = 3)),
        remove_call(
            stats::integrate(fn, 0, Inf, df = 3, subdivisions = 200L)
        )
    )

    expect_equal(
        remove_call(
            integrator$integrate(fn, 0, Inf, df = 5, ncp = 2)
        ),
        remove_call(
            stats::integrate(fn, 0, Inf, df = 5, ncp = 2, subdivisions = 200L)
        )
    )
})

test_that("Smaller required rel. tol. for normal distribution's variance", {
    integrator <- Integrator()
    fn <- function(x, mean = 0, sd = 1) {
        (x - mean)^2 * dnorm(x, mean = mean, sd = sd)
    }

    integrator$relative_accuracy <- .Machine$double.eps^0.125
    expect_equal(
        remove_call(integrator$integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 2
        )),
        remove_call(stats::integrate(
            fn, 0, Inf,
            mean = 0, sd = 2,
            rel.tol = .Machine$double.eps^0.125
        ))
    )

    expect_equal(
        remove_call(integrator$integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5
        )),
        remove_call(stats::integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5,
            rel.tol = .Machine$double.eps^0.125
        ))
    )
})

test_that("Higher required rel. tol. for normal distribution's variance", {
    integrator <- Integrator()
    fn <- function(x, mean = 0, sd = 1) {
        (x - mean)^2 * dnorm(x, mean = mean, sd = sd)
    }

    integrator$relative_accuracy <- .Machine$double.eps^0.5

    expect_equal(
        remove_call(integrator$integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 2
        )),
        remove_call(stats::integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 2,
            rel.tol = .Machine$double.eps^0.5
        ))
    )

    expect_equal(
        remove_call(integrator$integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5
        )),
        remove_call(stats::integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5,
            rel.tol = .Machine$double.eps^0.5
        ))
    )
})

test_that("Set required abs. tol. to zero for normal distribution's variance", {
    integrator <- Integrator()
    fn <- function(x, mean = 0, sd = 1) {
        (x - mean)^2 * dnorm(x, mean = mean, sd = sd)
    }

    integrator$absolute_accuracy <- 0

    expect_equal(
        remove_call(integrator$integrate(fn, -Inf, Inf, mean = 0, sd = 2)),
        remove_call(stats::integrate(fn, 0, Inf, mean = 0, sd = 2, abs.tol = 0))
    )

    expect_equal(
        remove_call(integrator$integrate(fn, -Inf, Inf, mean = 0, sd = 0.5)),
        remove_call(
            stats::integrate(fn, -Inf, Inf, mean = 0, sd = 0.5, abs.tol = 0)
        )
    )
})

test_that("`max_subdivisions == 0` produces `invalid_input_error`", {
    expect_error(
        Integrator(max_subdivisions = 0),
        "the input is invalid"
    )
})

test_that("`is.na(lower) || is.na(upper)` produces `invalid_input_error`", {
    integrator <- Integrator()
    fn <- function(x, rate = 1) (x - 1 / rate)^2 * dexp(x, rate = rate)

    expect_error(
        integrator$integrate(fn, 0, NA, rate = 1),
        "the input is invalid"
    )
    expect_error(
        integrator$integrate(fn, NA, Inf, rate = 1),
        "the input is invalid"
    )
})

test_that("`eps.abs <= 0 && eps.rel < max(50*.Machine$double.eps, 0.5e-28)` produces `invalid_input_error`", { # nolint
    expect_error(
        Integrator(
            absolute_accuracy = 0,
            relative_accuracy = 0.5 * max(50 * .Machine$double.eps, 0.5e-28)
        ),
        "the input is invalid"
    )
})

test_that("`work_size < 4 * max_subdivisions` produces `invalid_input_error`", {
    expect_error(
        Integrator(max_subdivisions = 100, work_size = 399),
        "the input is invalid"
    )
})

test_that("`max_subdivisions_error` is thrown", {
    integrator <- Integrator()
    expect_error(
        integrator$integrate(function(x) sin(1 / x), 0, 1),
        "maximum number of subdivisions reached"
    )
})

## See
## Piessens, Robert, et al. "Quadpack."
## A Subroutine Package for Automatic Integration Springer-Verlag (1983).
##
## "[...] terminate early in cases where the exact integral is zero and a
## pure relative accuracy is requested (absolute_accuracy=O)."
test_that("`roundoff_error` is thrown", {
    integrator <- Integrator()
    integrator$absolute_accuracy <- 0
    expect_error(
        integrator$integrate(function(x) x * dnorm(x), -1, 1),
        "roundoff error was detected"
    )
})

## See
## https://stat.ethz.ch/pipermail/r-help/2013-December/364560.html
test_that("`bad_integrand_error` is thrown", {
    integrator <- Integrator()
    expect_error(
        integrator$integrate(function(x) {
            ## nolint start
            -((100000 * x)^(-1) - (1000 * x)^(-1)) *
                1 / (0.20 * sqrt(2 * pi)) *
                exp(-0.5 * (x - 0.10) / (0.20))^2
            ## nolint end
        }, 0, Inf),
        "extremely bad integrand behaviour"
    )
})

test_that("`extrapolation_roundoff_error` is thrown", {
    integrator <- Integrator()
    expect_error(
        integrator$integrate(
            function(x) {
                (x - 2000) * dlnorm(x, meanlog = 9.0167, sdlog = 1.6247)
            },
            2000, Inf
        ),
        "roundoff error is detected in the extrapolation table"
    )
})

test_that("`divergence_error` is thrown", {
    integrator <- Integrator()
    expect_error(
        integrator$integrate(function(x) x^-0.9999, 0, 1),
        "the integral is probably divergent"
    )
})

test_that("function evaluation error is thrown", {
    integrator <- Integrator()
    expect_error(
        integrator$integrate(function(x) stop("stop on purpose"), 0, 1),
        "Evaluation error: stop on purpose."
    )
})

test_that("non-finite values error is thrown", {
    integrator <- Integrator()
    expect_error(
        integrator$integrate(
            function(x) {
                ifelse(x < 0.3, 1 / (1 - 0.3), ifelse(x > 0.7, 1 / (1 - 0.7), Inf)) # nolint
            },
            0, 1
        ),
        "non-finite function value"
    )
})
