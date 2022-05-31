test_that("`integration_logic_error` works as expected", {
    expect_equal(
        catch_what("integration_logic_error", ""),
        ""
    )

    message <- "a logic error occured during integration"
    expect_equal(
        catch_what("integration_logic_error", message),
        message
    )
})

test_that("`integration_runtime_error` works as expected", {
    expect_equal(
        catch_what("integration_runtime_error", ""),
        ""
    )

    message <- "a runtime error occured during integration"
    expect_equal(
        catch_what("integration_runtime_error", message),
        message
    )
})

test_that("`max_subdivision_error` works as expected", {
    expect_equal(
        catch_what("max_subdivision_error", ""),
        ""
    )

    message <- "maximum number of subdivisions reached"
    expect_equal(
        catch_what("max_subdivision_error", message),
        message
    )

    message <- sprintf("modified - %s", message)

    expect_equal(
        catch_what("max_subdivision_error", message),
        message
    )
})

test_that("`roundoff_error` works as expected", {
    expect_equal(
        catch_what("roundoff_error", ""),
        ""
    )

    message <- "roundoff error was detected"
    expect_equal(
        catch_what("roundoff_error", message),
        message
    )

    message <- sprintf("modified - %s", message)

    expect_equal(
        catch_what("roundoff_error", message),
        message
    )
})

test_that("`bad_integrand_error` works as expected", {
    expect_equal(
        catch_what("bad_integrand_error", ""),
        ""
    )

    message <- "extremely bad integrand behaviour"
    expect_equal(
        catch_what("bad_integrand_error", message),
        message
    )

    message <- sprintf("modified - %s", message)

    expect_equal(
        catch_what("bad_integrand_error", message),
        message
    )
})

test_that("`extrapolation_roundoff_error` works as expected", {
    expect_equal(
        catch_what("extrapolation_roundoff_error", ""),
        ""
    )

    message <- "roundoff error is detected in the extrapolation table"
    expect_equal(
        catch_what("extrapolation_roundoff_error", message),
        message
    )

    message <- sprintf("modified - %s", message)

    expect_equal(
        catch_what("extrapolation_roundoff_error", message),
        message
    )
})

test_that("`divergence_error` works as expected", {
    expect_equal(
        catch_what("divergence_error", ""),
        ""
    )

    message <- "the integral is probably divergent"
    expect_equal(
        catch_what("divergence_error", message),
        message
    )

    message <- sprintf("modified - %s", message)

    expect_equal(
        catch_what("divergence_error", message),
        message
    )
})

test_that("`invalid_input_error` works as expected", {
    expect_equal(
        catch_what("invalid_input_error", ""),
        ""
    )

    message <- "the input is invalid"
    expect_equal(
        catch_what("invalid_input_error", message),
        message
    )

    message <- sprintf("modified - %s", message)

    expect_equal(
        catch_what("invalid_input_error", message),
        message
    )
})
