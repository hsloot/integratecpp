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
    } else {
        return(tryCatch(
            Rcpp__integrator__throw_if_invalid(object@pointer),
            error = function(cond) {
                return(as.character(cond))
            }
        ))
    }

    invisible(TRUE)
})

#' @describeIn Integrator-class
#'   Construct an object of class `Integrator`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod validObject
#' @keywords internal
setMethod("initialize", "Integrator", function(.Object, max_subdivisions = 100, relative_accuracy = .Machine$double.eps^0.25, absolute_accuracy = relative_accuracy, work_size = 4 * max_subdivisions) { # nolint
    .Object@pointer <- Rcpp__integrator__new(max_subdivisions, relative_accuracy, absolute_accuracy, work_size)
    validObject(.Object)

    .Object
})

#' @describeIn Integrator-class
#'   Either access configuration parameters
#'   `max_subdivisions`, `relative_accuracy`, `absolute_accuracy`, or `work_size` or get the
#'   integration routine with signature
#'   `function(f, lower, upper, ..., stop.on.error = TRUE)`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod
#'
#' @keywords internal
setMethod("$", "Integrator", function(x, name) {
    if (name %in% c("max_subdivisions", "relative_accuracy", "absolute_accuracy", "work_size")) {
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
#'   `max_subdivisions`, `relative_accuracy`, `absolute_accuracy`, or `work_size`.
#'
#' @include RcppExports.R
#' @importFrom methods setMethod validObject
#'
#' @keywords internal
setMethod("$<-", "Integrator", function(x, name, value) {
    if (name %in% c("max_subdivisions", "relative_accuracy", "absolute_accuracy", "work_size")) {
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
        cat(sprintf("- max_subdivisions: %s\n", format(object$max_subdivisions)))
        cat(sprintf("- relative_accuracy: %s\n", format(object$relative_accuracy, scientific = TRUE)))
        cat(sprintf("- absolute_accuracy: %s\n", format(object$absolute_accuracy, scientific = TRUE)))
        cat(sprintf("- work_size: %s\n", format(object$work_size)))
    } else {
        cat("\t (invalid or not initialized)\n")
    }

    invisible(NULL)
})

# nocov end
