test_that("`max_subdivision_error` works as expected", {
    message <- "maximum number of subdivisions reached"
    expect_equal(
        max_subdivision_error(message),
        message
    )

    message <- sprintf("modified – %s", message)

    expect_equal(
        max_subdivision_error(message),
        message
    )
})

test_that("`roundoff_error` works as expected", {
    message <- "roundoff error was detected"
    expect_equal(
        roundoff_error(message),
        message
    )

    message <- sprintf("modified – %s", message)

    expect_equal(
        roundoff_error(message),
        message
    )
})

test_that("`bad_integrand_error` works as expected", {
    message <- "extremely bad integrand behaviour"
    expect_equal(
        bad_integrand_error(message),
        message
    )

    message <- sprintf("modified – %s", message)

    expect_equal(
        bad_integrand_error(message),
        message
    )
})

test_that("`extrapolation_roundoff_error` works as expected", {
    message <- "roundoff error is detected in the extrapolation table"
    expect_equal(
        extrapolation_roundoff_error(message),
        message
    )

    message <- sprintf("modified – %s", message)

    expect_equal(
        extrapolation_roundoff_error(message),
        message
    )
})

test_that("`divergence_error` works as expected", {
    message <- "the integral is probably divergent"
    expect_equal(
        divergence_error(message),
        message
    )

    message <- sprintf("modified – %s", message)

    expect_equal(
        divergence_error(message),
        message
    )
})

test_that("`invalid_input_error` works as expected", {
    message <- "the input is invalid"
    expect_equal(
        invalid_input_error(message),
        message
    )

    message <- sprintf("modified – %s", message)

    expect_equal(
        invalid_input_error(message),
        message
    )
})
