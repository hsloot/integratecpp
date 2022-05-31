## REVIEW: consider moving file to `inst/Rsource`
##   and load specifically as test-helper.

#' Throw exception and catch error message
#'
#' @param exception name of the exception to be thrown.
#' @param message error message passed to the exception constructor.
#'
#' @keywords internal
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
