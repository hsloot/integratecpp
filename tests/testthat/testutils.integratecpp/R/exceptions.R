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

#' Throw exception and catch error message
#'
#' @param exception name of the exception to be thrown.
#' @param message error message passed to the exception constructor.
#'
#' @export
catch_what <- function(exception, message) {
    stopifnot(exception %in% c(
        "integration_logic_error",
        "integration_runtime_error",
        "max_subdivision_error",
        "roundoff_error",
        "bad_integrand_error",
        "extrapolation_roundoff_error",
        "divergence_error",
        "invalid_input_error"
    ))

    get(paste("Rcpp", exception, "catch_what", sep = "__"))(message)
}
