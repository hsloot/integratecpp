remove_call <- function(x) {
    x <- x[-match(x, "call")]
}

test_that("Default settings for exponential distribtion's expectation", {
    fn <- function(x, rate = 1) {
        x * dexp(x, rate = rate)
    }

    expect_equal(
        remove_call(integrate(fn, 0, Inf, rate = 0.5)),
        remove_call(stats::integrate(fn, 0, Inf, rate = 0.5))
    )

    expect_equal(
        remove_call(integrate(fn, 0, Inf, rate = 1)),
        remove_call(stats::integrate(fn, 0, Inf, rate = 1))
    )

    expect_equal(
        remove_call(integrate(fn, 0, Inf, rate = 2)),
        remove_call(stats::integrate(fn, 0, Inf, rate = 2))
    )
})

test_that("Default settings for beta distribtion's expectation", {
    fn <- function(x, shape1, shape2) {
        x * dbeta(x, shape1 = shape1, shape2 = shape2)
    }

    expect_equal(
        remove_call(integrate(fn, 0, 1, shape1 = 1, shape2 = 1)),
        remove_call(stats::integrate(fn, 0, 1, shape1 = 1, shape2 = 1))
    )

    expect_equal(
        remove_call(integrate(fn, 0, 1, shape1 = 0.3, shape2 = 0.4)),
        remove_call(stats::integrate(fn, 0, 1, shape1 = 0.3, shape2 = 0.4))
    )

    expect_equal(
        remove_call(integrate(fn, 0, 1, shape1 = 1.5, shape2 = 2)),
        remove_call(stats::integrate(fn, 0, 1, shape1 = 1.5, shape2 = 2))
    )
})

test_that("Default settings for negative Weibull distribtion's expectation", {
    fn <- function(x, shape, scale = 1) {
        x * dweibull(-x, shape = shape, scale = scale)
    }

    expect_equal(
        remove_call(integrate(fn, -Inf, 0, shape = 1, scale = 1)),
        remove_call(stats::integrate(fn, -Inf, 0, shape = 1, scale = 1))
    )

    expect_equal(
        remove_call(integrate(fn, -Inf, 0, shape = 0.3, scale = 0.4)),
        remove_call(stats::integrate(fn, -Inf, 0, shape = 0.3, scale = 0.4))
    )

    expect_equal(
        remove_call(integrate(fn, -Inf, 0, shape = 1.5, scale = 2)),
        remove_call(stats::integrate(fn, -Inf, 0, shape = 1.5, scale = 2))
    )
})

test_that("Less max. subdivisions for gamma distribution's expectation", {
    fn <- function(x, shape, rate = 1) {
        x * dgamma(x, shape = shape, rate = rate)
    }

    expect_equal(
        remove_call(integrate(
            fn, 0, Inf,
            shape = 0.3, rate = 0.5,
            subdivisions = 50L
        )),
        remove_call(stats::integrate(
            fn, 0, Inf,
            shape = 0.3, rate = 0.5,
            subdivisions = 50L
        ))
    )

    expect_equal(
        remove_call(integrate(
            fn, 0, Inf,
            shape = 0.7, rate = 1.5,
            subdivisions = 50L
        )),
        remove_call(stats::integrate(
            fn, 0, Inf,
            shape = 0.7, rate = 1.5,
            subdivisions = 50L
        ))
    )
})

test_that("More max. subdivisions for chi-square distribution's expectation", {
    fn <- function(x, df, ncp = 0) {
        x * dchisq(x, df = df, ncp = ncp)
    }

    expect_equal(
        remove_call(integrate(fn, 0, Inf, df = 3, subdivisions = 200L)),
        remove_call(
            stats::integrate(fn, 0, Inf, df = 3, subdivisions = 200L)
        )
    )

    expect_equal(
        remove_call(
            integrate(fn, 0, Inf, df = 5, ncp = 2, subdivisions = 200L)
        ),
        remove_call(
            stats::integrate(fn, 0, Inf, df = 5, ncp = 2, subdivisions = 200L)
        )
    )
})

test_that("Smaller required rel. tol. for normal distribution's variance", {
    fn <- function(x, mean = 0, sd = 1) {
        (x - mean)^2 * dnorm(x, mean = mean, sd = sd)
    }

    expect_equal(
        remove_call(integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 2,
            rel.tol = .Machine$double.eps^0.125
        )),
        remove_call(stats::integrate(
            fn, 0, Inf,
            mean = 0, sd = 2,
            rel.tol = .Machine$double.eps^0.125
        ))
    )

    expect_equal(
        remove_call(integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5,
            rel.tol = .Machine$double.eps^0.125
        )),
        remove_call(stats::integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5,
            rel.tol = .Machine$double.eps^0.125
        ))
    )
})

test_that("Higher required rel. tol. for normal distribution's variance", {
    fn <- function(x, mean = 0, sd = 1) {
        (x - mean)^2 * dnorm(x, mean = mean, sd = sd)
    }

    expect_equal(
        remove_call(integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 2,
            rel.tol = .Machine$double.eps^0.5
        )),
        remove_call(stats::integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 2,
            rel.tol = .Machine$double.eps^0.5
        ))
    )

    expect_equal(
        remove_call(integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5,
            rel.tol = .Machine$double.eps^0.5
        )),
        remove_call(stats::integrate(
            fn, -Inf, Inf,
            mean = 0, sd = 0.5,
            rel.tol = .Machine$double.eps^0.5
        ))
    )
})

test_that("Set required abs. tol. to zero for normal distribution's variance", {
    fn <- function(x, mean = 0, sd = 1) {
        (x - mean)^2 * dnorm(x, mean = mean, sd = sd)
    }

    expect_equal(
        remove_call(integrate(fn, -Inf, Inf, mean = 0, sd = 2, abs.tol = 0)),
        remove_call(stats::integrate(fn, 0, Inf, mean = 0, sd = 2, abs.tol = 0))
    )

    expect_equal(
        remove_call(integrate(fn, -Inf, Inf, mean = 0, sd = 0.5, abs.tol = 0)),
        remove_call(
            stats::integrate(fn, -Inf, Inf, mean = 0, sd = 0.5, abs.tol = 0)
        )
    )
})

test_that("`limit == 0` produces `invalid_input_error`", {
    fn <- function(x, rate = 1) (x - 1 / rate)^2 * dexp(x, rate = rate)

    expect_error(
        integrate(fn, 0, Inf, rate = 1, subdivisions = 0),
        "the input is invalid"
    )
})

test_that("`is.na(lower) || is.na(upper)` produces `invalid_input_error`", {
    fn <- function(x, rate = 1) (x - 1 / rate)^2 * dexp(x, rate = rate)

    expect_error(
        integrate(fn, 0, NA, rate = 1),
        "the input is invalid"
    )
    expect_error(
        integrate(fn, NA, Inf, rate = 1),
        "the input is invalid"
    )
})

test_that("`eps.abs <= 0 && eps.rel < max(50*.Machine$double.eps, 0.5e-28)` produces `invalid_input_error`", {
    fn <- function(x, rate = 1) (x - 1 / rate)^2 * dexp(x, rate = rate)

    expect_error(
        integrate(fn, 0, Inf, rate = 1, abs.tol = 0, rel.tol = 0.5 * max(50 * .Machine$double.eps, 0.5e-28)),
        "the input is invalid"
    )
})

test_that("`lenw < 4 * limits` produces `invalid_input_error`", {
    fn <- function(x, rate = 1) (x - 1 / rate)^2 * dexp(x, rate = rate)

    expect_error(
        integrate(fn, 0, Inf, rate = 1, subdivisions = 100, lenw = 399),
        "the input is invalid"
    )
})
