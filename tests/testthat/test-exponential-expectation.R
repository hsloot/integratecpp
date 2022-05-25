test_that("Exponential expectation result matches that of R", {
    expect_equal(
        integrate_exponential_expectation(0.5)$value,
        integrate(function(x) x * dexp(x, 0.5), 0, Inf)$value)

    expect_equal(
        integrate_exponential_expectation(1)$value,
        integrate(function(x) x * dexp(x, 1), 0, Inf)$value)

        expect_equal(
        integrate_exponential_expectation(2)$value,
        integrate(function(x) x * dexp(x, 2), 0, Inf)$value)
})
