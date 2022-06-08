## REVIEW: consider moving file to `inst/Rsource`
##   and load specifically as test-helper.

#' A method for numerical integration
#'
#' @inherit stats::integrate
#' @param max_subdivisions the maximum number of subintervals.
#' @param relative_accuracy relative accuracy requested.
#' @param absolute_accuracy absolute accuracy requested.
#'
#' @include RcppExports.R
#' @keywords internal
integrate <- function(f, lower, upper, ..., max_subdivisions = 100L,
                      relative_accuracy = .Machine$double.eps^0.25,
                      absolute_accuracy = relative_accuracy,
                      work_size = 4 * max_subdivisions,
                      stop.on.error = TRUE # nolint
) {
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
