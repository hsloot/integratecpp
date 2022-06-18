# Copyright (C) 2022 Henrik Sloot
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

skip_if_not_installed("testutils.integratecpp")
catch_what <- testutils.integratecpp::catch_what


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
