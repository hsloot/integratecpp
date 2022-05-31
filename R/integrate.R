## REVIEW: consider moving file to `inst/Rsource`
##   and load specifically as test-helper.

#' A method for numerical integration
#'
#' @inherit stats::integrate
#' @param limit the maximum number of subintervals.
#' @param epsrel relative accuracy requested.
#' @param epsabs absolute accuracy requested.
#'
#' @include RcppExports.R
#' @keywords internal
integrate <- function(f, lower, upper, ..., limit = 100L,
                      epsrel = .Machine$double.eps^0.25,
                      epsabs = epsrel,
                      lenw = 4 * limit,
                      stop.on.error = TRUE # nolint
) {
    out <- Rcpp__integrate(
        function(x) f(x, ...), lower, upper, limit, epsrel, epsabs, lenw
    )
    out$call <- match.call()
    class(out) <- "integrate"

    if (isTRUE(stop.on.error) && !isTRUE(out$message == "OK")) {
        stop(out$message)
    }

    out
}
