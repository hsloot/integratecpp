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

#' A method for numerical integration
#'
#' @inheritParams stats::integrate
#' @param max_subdivisions the maximum number of subintervals.
#' @param relative_accuracy relative accuracy requested.
#' @param absolute_accuracy absolute accuracy requested.
#'
#' @return A list of class `integrate` with components `value`, `abs.error`,
#    `subdivision`, `message`, and `call`; see [stats::integrate()].
#'
#' @family test-helper
#'
#' @include RcppExports.R
#' @keywords internal
integrate <- function(f, lower, upper, ..., max_subdivisions = 100L,
                      relative_accuracy = .Machine$double.eps^0.25,
                      absolute_accuracy = relative_accuracy,
                      work_size = 4 * max_subdivisions,
                      stop.on.error = TRUE) { # nolint: object_name_linter
    out <- Rcpp__integrate(
        function(x) {
            f(x, ...)
        },
        lower, upper,
        max_subdivisions,
        relative_accuracy,
        absolute_accuracy,
        work_size
    )
    out$call <- match.call()
    class(out) <- "integrate"

    if (isTRUE(stop.on.error) && !isTRUE(out$message == "OK")) {
        stop(out$message)
    }

    out
}
