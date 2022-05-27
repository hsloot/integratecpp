#' @include RcppExports.R
#' @keywords internal
#' @noRd
integrate <- function(f, lower, upper, ..., subdivisions = 100L,
                      rel.tol = .Machine$double.eps^0.25, # nolint
                      abs.tol = rel.tol,
                      stop.on.error = TRUE # nolint
) {
    out <- Rcpp_integrate(
        function(x) f(x, ...), lower, upper, subdivisions, rel.tol, abs.tol
    )
    out$call <- match.call()
    class(out) <- "integrate"

    if (isTRUE(stop.on.error) && !isTRUE(out$message == "OK")) {
        stop(out$message)
    }

    out
}
