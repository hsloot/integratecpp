#' @include RcppExports.R
#' @keywords internal
#' @noRd
integrate <- function(f, lower, upper, ..., limit = 100L,
                      epsrel = .Machine$double.eps^0.25,
                      epsabs = epsrel,
                      stop_on_error = TRUE,
                      lenw = 4 * limit) {
    out <- Rcpp_integrate(
        function(x) f(x, ...), lower, upper, limit, epsrel, epsabs, lenw
    )
    out$call <- match.call()
    class(out) <- "integrate"

    if (isTRUE(stop_on_error) && !isTRUE(out$message == "OK")) {
        stop(out$message)
    }

    out
}
