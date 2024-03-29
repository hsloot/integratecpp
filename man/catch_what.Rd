% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/exceptions.R
\name{catch_what}
\alias{catch_what}
\title{Throw exception and catch error message}
\usage{
catch_what(exception, message)
}
\arguments{
\item{exception}{name of the exception to be thrown.}

\item{message}{error message passed to the exception constructor.}
}
\value{
A String with \code{message} if \code{exception} is the name of an
implemented exception class.
}
\description{
Throw exception and catch error message
}
\seealso{
Other test-helper: 
\code{\link{Integrator-class}},
\code{\link{integrate}}
}
\concept{test-helper}
\keyword{internal}
