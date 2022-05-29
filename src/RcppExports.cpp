// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "../inst/include/integratecpp.h"
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// integration_logic_error
std::string integration_logic_error(std::string what);
RcppExport SEXP _integratecpp_integration_logic_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(integration_logic_error(what));
    return rcpp_result_gen;
END_RCPP
}
// integration_runtime_error
std::string integration_runtime_error(std::string what);
RcppExport SEXP _integratecpp_integration_runtime_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(integration_runtime_error(what));
    return rcpp_result_gen;
END_RCPP
}
// max_subdivision_error
std::string max_subdivision_error(std::string what);
RcppExport SEXP _integratecpp_max_subdivision_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(max_subdivision_error(what));
    return rcpp_result_gen;
END_RCPP
}
// roundoff_error
std::string roundoff_error(std::string what);
RcppExport SEXP _integratecpp_roundoff_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(roundoff_error(what));
    return rcpp_result_gen;
END_RCPP
}
// bad_integrand_error
std::string bad_integrand_error(std::string what);
RcppExport SEXP _integratecpp_bad_integrand_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(bad_integrand_error(what));
    return rcpp_result_gen;
END_RCPP
}
// extrapolation_roundoff_error
std::string extrapolation_roundoff_error(std::string what);
RcppExport SEXP _integratecpp_extrapolation_roundoff_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(extrapolation_roundoff_error(what));
    return rcpp_result_gen;
END_RCPP
}
// divergence_error
std::string divergence_error(std::string what);
RcppExport SEXP _integratecpp_divergence_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(divergence_error(what));
    return rcpp_result_gen;
END_RCPP
}
// invalid_input_error
std::string invalid_input_error(std::string what);
RcppExport SEXP _integratecpp_invalid_input_error(SEXP whatSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type what(whatSEXP);
    rcpp_result_gen = Rcpp::wrap(invalid_input_error(what));
    return rcpp_result_gen;
END_RCPP
}
// Rcpp_integrate
Rcpp::List Rcpp_integrate(Rcpp::Function fn, const double lower, const double upper, const int subdivisions, const double epsrel, const double epsabs, const int lenw);
RcppExport SEXP _integratecpp_Rcpp_integrate(SEXP fnSEXP, SEXP lowerSEXP, SEXP upperSEXP, SEXP subdivisionsSEXP, SEXP epsrelSEXP, SEXP epsabsSEXP, SEXP lenwSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< Rcpp::Function >::type fn(fnSEXP);
    Rcpp::traits::input_parameter< const double >::type lower(lowerSEXP);
    Rcpp::traits::input_parameter< const double >::type upper(upperSEXP);
    Rcpp::traits::input_parameter< const int >::type subdivisions(subdivisionsSEXP);
    Rcpp::traits::input_parameter< const double >::type epsrel(epsrelSEXP);
    Rcpp::traits::input_parameter< const double >::type epsabs(epsabsSEXP);
    Rcpp::traits::input_parameter< const int >::type lenw(lenwSEXP);
    rcpp_result_gen = Rcpp::wrap(Rcpp_integrate(fn, lower, upper, subdivisions, epsrel, epsabs, lenw));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_integratecpp_integration_logic_error", (DL_FUNC) &_integratecpp_integration_logic_error, 1},
    {"_integratecpp_integration_runtime_error", (DL_FUNC) &_integratecpp_integration_runtime_error, 1},
    {"_integratecpp_max_subdivision_error", (DL_FUNC) &_integratecpp_max_subdivision_error, 1},
    {"_integratecpp_roundoff_error", (DL_FUNC) &_integratecpp_roundoff_error, 1},
    {"_integratecpp_bad_integrand_error", (DL_FUNC) &_integratecpp_bad_integrand_error, 1},
    {"_integratecpp_extrapolation_roundoff_error", (DL_FUNC) &_integratecpp_extrapolation_roundoff_error, 1},
    {"_integratecpp_divergence_error", (DL_FUNC) &_integratecpp_divergence_error, 1},
    {"_integratecpp_invalid_input_error", (DL_FUNC) &_integratecpp_invalid_input_error, 1},
    {"_integratecpp_Rcpp_integrate", (DL_FUNC) &_integratecpp_Rcpp_integrate, 7},
    {NULL, NULL, 0}
};

RcppExport void R_init_integratecpp(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
