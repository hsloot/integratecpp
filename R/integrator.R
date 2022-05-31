## REVIEW: consider moving file to `inst/Rsource`
##   and load specifically as test-helper.

#' A class for numerical integration
#'
#' @slot pointer An external pointer to a C++ object.
#'
#' @importFrom methods setClass
#' @keywords internal
Integrator <- setClass("Integrator", slots = c("pointer" = "externalptr")) # nolint

#' @importFrom methods setValidity new
#' @keywords internal
#' @noRd
setValidity("Integrator", function(object) {
    if (identical(object@pointer, new("externalptr"))) {
        return("dangling pointer")
    }

    invisible(TRUE)
})

#' @describeIn Integrator-class
#'   Construct an object of class `Integrator`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod validObject
#' @keywords internal
setMethod("initialize", "Integrator", function(.Object, limit = 100, epsrel = .Machine$double.eps^0.25, epsabs = epsrel, lenw = 4 * limit) { # nolint
    .Object@pointer <- Rcpp__integrator__new(limit, epsrel, epsabs, lenw)
    validObject(.Object)

    .Object
})

#' @describeIn Integrator-class
#'   Either access configuration parameters
#'   `limit`, `epsrel`, `epsabs`, or `lenw` or get the
#'   integration routine with signature
#'   `function(f, lower, upper, ..., stop.on.error = TRUE)`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod
#'
#' @keywords internal
setMethod("$", "Integrator", function(x, name) {
    if (name %in% c("limit", "epsrel", "epsabs", "lenw")) {
        get(paste("Rcpp__integrator__get", name, sep = "_"))(x@pointer)
    } else if (name == "integrate") {
        function(f, lower, upper, ..., stop_on_error = TRUE) { # nolint
            out <- Rcpp__integrator__integrate(
                x@pointer,
                function(y) f(y, ...), lower, upper
            )
            out$call <- match.call()
            class(out) <- "integrate"

            if (isTRUE(stop_on_error) && !isTRUE(out$message == "OK")) {
                stop(out$message)
            }

            out
        }
    } else {
        stop("not implemented") # nocov
    }
})

#' @describeIn Integrator-class
#'   Set any of the configuration parameters
#'   `limit`, `epsrel`, `epsabs`, or `lenw`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod validObject
#'
#' @keywords internal
setMethod("$<-", "Integrator", function(x, name, value) {
    if (name %in% c("limit", "epsrel", "epsabs", "lenw")) {
        get(paste("Rcpp__integrator__set", name, sep = "_"))(x@pointer, value)
        validObject(x)

        x
    } else {
        stop("not implemented") # nocov
    }
})

# nocov start

#' @describeIn Integrator-class
#'   print an object of class `Integrator`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod classLabel validObject
#'
#' @keywords internal
#' @noRd
setMethod("show", "Integrator", function(object) {
    cat(sprintf("An object of class %s\n", classLabel(class(object))))
    if (isTRUE(validObject(object, test = TRUE))) {
        cat(sprintf("%s\n", format(object@pointer)))
        cat(sprintf("- limit: %s\n", format(object$limit)))
        cat(sprintf("- epsrel: %s\n", format(object$epsrel, scientific = TRUE)))
        cat(sprintf("- epsabs: %s\n", format(object$epsabs, scientific = TRUE)))
        cat(sprintf("- lenw: %s\n", format(object$lenw)))
    } else {
        cat("\t (invalid or not initialized)\n")
    }

    invisible(NULL)
})

# nocov end
