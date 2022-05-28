#pragma once
//! \file integratecpp.h

#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <R_ext/Applic.h>

namespace integratecpp {

/*!
 * \brief A functor wrapping `Rdqags` and `Rdqagi` declared in
 *   `    <R_ext/Applic.h` and implemented in `src/appl/integrate.c`.
 *
 * - The functor is initialized by a configuration parameter with the maximal
 *   number of subdivisions, the required relative error, the required absolute
 *   error, and a dimensioning parameter for the working array.
 * - The operator is called with a Lambda-functor with signature convertible to
 *   `const double`, a lower bound, and an upper bound.
 * - The returned result consists of the approximated integral value, an
 *   estimated error, the final number of subdivisions, and the number of
 *   funcion evaluations.
 * - Issues during regarding the configuration parameter throw an exception,
 *   deriving from `integratecpp::integration_logic_error` and issues during the
 *   integration may throw exceptions deriving from
 *   `integratecpp::integration_runtime_error`. Both have accessors to the
 *   result-state at error which can be used for error handling.
 */
class integrator {
public:
  /*!
   * \brief Defines a data class for the integation results returned from
   *   `integratecpp::integrator::operator()()`.
   *
   * Contains the following data elements (compare `src/appl/integrate.c` in
   * R-source):
   * - `double value`: The approximation of the integral.
   * - `double abserr`: The estimate of the modules of the absolute error,
   *   which should be equal or larger than `abs(I-result)`.
   * - `int subdivisions`: The final number of subintervals produced in
   *   the subdivision process.
   * - `int neval`: The number of integrand evaluations.
   */
  class result_type {
  private:
    double value_ = 0.;
    double abserr_ = 0.;
    int subdivisions_ = 0;
    int neval_ = 0;

  public:
    result_type() = default;
    /*!
     * \brief The full constructor.
     *
     * \param value         a `double` with the approximated value.
     * \param abserr        a `double` with the estimated absolute error.
     * \param subdivisions  an `int` with the final number of subdivisions.
     * \param neval         an `int` with the number of neval.
     */
    explicit result_type(const double value, const double abserr,
                         const int subdivisions, const int neval);

    //! \brief Accessor to the approximated value.
    auto value() const noexcept -> decltype(value_);

    //! \brief Accessor to the estimated absolute error.
    auto abserr() const noexcept -> decltype(abserr_);

    //! \brief Accessor to the final number of subdivisions.
    auto subdivisions() const noexcept -> decltype(subdivisions_);

    //! \brief Accessor to the number of neval.
    auto neval() const noexcept -> decltype(neval_);
  };

  /*!
   * \brief Defines a data class for the integration configuration parameters
   *        used in `integratecpp::integrator::operator()()`.
   *
   * Contains the following data elements (see `src/appl/integrate.c` in
   * R-source):
   * - `int limit = 100`: The maximum number of subintervals in the partition of
   *   the given integration interval (lower, upper). If `limit >= 1` is not
   *   fulfilled, an `integratecpp::invalid_input_error` exception is thrown.
   * - `epsrel` (`double`) and `epsabs` (`double`): The requested relative and
   *   absolute accuracies. Both are equal by default to the fourth root of the
   *   Machine epsilon. If `epsabs <= 0` and
   *   `epsrel < max(50*rel.mach.acc.,0.5d-28)`, an
   *  `integratecpp::invalid_input_error` is thrown.
   * - `lenw` (`int`): A dimensioning parameter for the working array. Must
   *   equal at least four times `limit`, otherwise an invalid_input_error
   *   is thrown.
   */
  class config_type {
  private:
    int limit_ = 100;
    double epsrel_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);
    double epsabs_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);
    int lenw_ = 400;

  public:
    config_type() = default;

    /*!
     * \brief A partial constructor for `limit` and `epsrel`.
     *
     * \param limit   an `int` for the maximum number of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     *
     * \exception     throws integratecpp::invalid_input_error if limit < 1.
     */
    explicit config_type(const int limit, const double epsrel);

    /*!
     * A partial constructor for `limit`, `epsrel`, and `epsabs`.
     *
     * \param limit   an `int` for the maximum number of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     * \param epsabs  a `double` for the requested absolute accuracy.
     *
     * \exception     throws integratecpp::invalid_input_error if limit < 1.
     * \exception     throws integratecpp::invalid_input_error if epsabs <= 0
     *                and epsrel < max(50*rel.mach.acc.,0.5d-28).
     */
    explicit config_type(const int limit, const double epsrel,
                         const double epsabs);

    /*!
     * \brief The full constructor.
     *
     * \param limit   an `int` for the maximum number of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     * \param epsabs  a `double` for the requested absolute accuracy.
     * \param lenw    an `int` for the size of the working array.
     *
     * \exception     throws integratecpp::invalid_input_error if limit < 1.
     * \exception     throws integratecpp::invalid_input_error if epsabs <= 0
     *                and epsrel < max(50*rel.mach.acc.,0.5d-28).
     * \exception     throws integratecpp::invalid_input_error if lenw < 4 *
     *                limit.
     */
    explicit config_type(const int limit, const double epsrel,
                         const double epsabs, const int lenw);

    //! \brief Accessor to the maximum number of subdivisions.
    auto limit() const noexcept -> decltype(limit_);

    //! \brief Accessor to the requested relative accuracy.
    auto epsrel() const noexcept -> decltype(epsrel_);

    //! \brief Accessor to the requested absolute accuracy.
    auto epsabs() const noexcept -> decltype(epsabs_);

    //! \brief Accessor to the dimensioning parameter `lenw`.
    auto lenw() const noexcept -> decltype(lenw_);
  };

private:
  const config_type cfg_{};

public:
  integrator() = default;

  /*!
   * \brief A full constructor using `integratecpp::integrator::config_type`.
   *
   * \param cfg  a `integratecpp::integrator::config_type`.
   */
  explicit integrator(const config_type &cfg) noexcept;

  /*!
   * \brief A partial constructor using `limit` and `epsrel`.
   *
   * \param limit   an `int` for the maximum number of subdivisions.
   * \param epsrel  a `double` for the requested relative accuracy.
   *
   * \exception     throws integratecpp::invalid_input_error if limit < 1.
   */
  explicit integrator(const int limit, const double epsrel);

  /*!
   * \brief A partial constructor using `limit`, `epsrel`, and `epsabs`.
   *
   * \param limit   an `int` for the maximum number of subdivisions.
   * \param epsrel  a `double` for the requested relative accuracy.
   * \param epsabs  a `double` for the requested absolute accuracy.
   *
   * \exception     throws integratecpp::invalid_input_error if limit < 1.
   * \exception     throws integratecpp::invalid_input_error if epsabs <= 0
   *                and epsrel < max(50*rel.mach.acc.,0.5d-28).
   */
  explicit integrator(const int limit, const double epsrel,
                      const double epsabs);

  /*!
   * \brief A full constructor using `limit`, `epsrel`, `epsabs`, and `lenw`.
   *
   * \param limit   an `int` for the maximum number of subdivisions.
   * \param epsrel  a `double` for the requested relative accuracy.
   * \param epsabs  a `double` for the requested absolute accuracy.
   * \param lenw    an `int` for the size of the working array.
   *
   * \exception     throws integratecpp::invalid_input_error if limit < 1.
   * \exception     throws integratecpp::invalid_input_error if epsabs <= 0
   *                and epsrel < max(50*rel.mach.acc.,0.5d-28).
   * \exception     throws integratecpp::invalid_input_error if lenw < 4 *
   *                limit.
   */
  explicit integrator(const int limit, const double epsrel, const double epsabs,
                      const int lenw);

  //! \brief Accessor for the configuration parameters.
  auto config() const noexcept -> decltype(cfg_);

  /*!
   * \brief Approximates an integratal numerically for a Lambda-functor, lower,
   *        and upper bound, using `Rdqags` if both bounds are are finite and
   *        `Rdqagi` of at least one of the bounds is infinite.
   *
   * \param fn     a `Lambda_` functor compatible with a `const double`
   *               signature.
   * \param lower  a `double` for the lower bound.
   * \param upper  a `double` for the upper bound.
   *
   * \return       a `integratecpp::integrator::result_type` with the
   *               integration results.
   *
   * \exception    throws integratecpp::max_subdivision_error if the maximal
   *               number of subdivisions is reached without fulfilling required
   *               error conditions.
   * \exception    throws integratecpp::roundoff_error if a roundoff error is
   *               detected which prevents the requested accuracy from being
   *               achieved.
   * \exception    throws integratecpp::bad_integrand_error if extremely bad
   *               integrand behaviour is detected during integration.
   * \exception    throws integratecpp::extrapolation_roundoff_error if a
   *               roundoff error is detected in the extrapolation table.
   * \exception    throws integratecpp::divergence_error if the integral is
   *               deemed divergence (or slowly convergent).
   */
  template <typename Lambda_>
  result_type operator()(Lambda_ fn, const double lower,
                         const double upper) const;
};

/*!
 * \brief A drop-in replacement of `integratecpp::integrator` for numerical
 *        integration. Approximates an integratal numerically for a
 *        Lambda-functor, lower, and upper bound, using `Rdqags` if both bounds
 *        are are finite and `Rdqagi` of at least one of the bounds is infinite.
 *
 * \param fn      a `Lambda_` functor compatible with a `const double`
 *                signature.
 * \param lower   a `double` for the lower bound.
 * \param upper   a `double` for the upper bound.
 * \param config  a `const` reference to a
 *                `integratecpp::integrator::config_type` configuration
 *                parameter.
 *
 * \return       a `integratecpp::integrator::result_type` with the
 *               integration results.
 *
 * \exception    throws integratecpp::max_subdivision_error if the maximal
 *               number of subdivisions is reached without fulfilling required
 *               error conditions.
 * \exception    throws integratecpp::roundoff_error if a roundoff error is
 *               detected which prevents the requested accuracy from being
 *               achieved.
 * \exception    throws integratecpp::bad_integrand_error if extremely bad
 *               integrand behaviour is detected during integration.
 * \exception    throws integratecpp::extrapolation_roundoff_error if a
 *               roundoff error is detected in the extrapolation table.
 * \exception    throws integratecpp::divergence_error if the integral is
 *               deemed divergence (or slowly convergent).
 */
template <typename Lambda_>
integrator::result_type
integrate(Lambda_ fn, const double lower, const double upper,
          const integrator::config_type &config = integrator::config_type{});

/*!
 * \brief Defines a type of object to be thrown as exception. It reports errors
 *        that occur during the integration routine of
 *        `integratecpp::integator::operator()()` or `integratecpp::integrate()`
 *        and are due to events beyond the scope of the program and not easily
 *        predicted.
 */
class integration_runtime_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;

  /*!
   * \brief A full constructor, specifying the error message with `std::string`.
   *
   * @param  what     a `std::string` containing the error message.
   * @param  result   a `integratecpp::integrator::result_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_runtime_error(const std::string what,
                                     const integrator::result_type &result);

  /*!
   * \brief A full constructor, specifying the error message with
   *        `const char
   * *`.
   *
   * @param  what     a `const char *` containing a pointer a a const char array
   *                  with the error message.
   * @param  result   a `integratecpp::integrator::result_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_runtime_error(const char *what,
                                     const integrator::result_type &result);

  //! \brief Accessor the the result at the time or error.
  virtual integrator::result_type result() const noexcept;

private:
  integrator::result_type result_{};
};

/*!
 * \brief Defines a type of object to be thrown as exception.  It reports errors
 *        that occur during the integration routine of
 *        `integratecpp::integator::operator()()` or `integratecpp::integrate()`
 *        and that are a consequence of faulty logic within the program such as
 *        violating logical preconditions or class invariants and may be
 *        preventable.
 */
class integration_logic_error : public std::logic_error {

public:
  using std::logic_error::logic_error;

  /*!
   * \brief A full constructor, specifying the error message with `std::string`.
   *
   * @param  what     a `std::string` containing the error message.
   * @param  result   a `integratecpp::integrator::result_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_logic_error(const std::string what,
                                   const integrator::result_type &result);

  /*!
   * \brief A full constructor, specifying the error message with
   *        `const char *`.
   *
   * @param  what     a `const char *` containing a pointer a a const char array
   *                  with the error message.
   * @param  result   a `integratecpp::integrator::result_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_logic_error(const char *what,
                                   const integrator::result_type &result);

  //! \brief Accessor the the result at the time or error.
  virtual integrator::result_type result() const noexcept;

private:
  integrator::result_type result_{};
};

/*!
 * A class for the exception if more subdivisions than the maximum are
 * required.
 *
 * From <scr/appl/integate.c>:
 * maximum number of subdivisions allowed has been achieved. one can allow
 * more subdivisions by increasing the value of limit (and taking the
 * according dimension adjustments into account). however, if this yields no
 * improvement it is advised to analyze the integrand in order to determine
 * the integration difficulties. if the position of a local difficulty can be
 * determined (e.g. singularity, discontinuity within the interval) one will
 * probably gain from splitting up the interval at this point and calling the
 * integrator on the subranges. if possible, an appropriate special-purpose
 * integrator should be used, which is designed for handling the type of
 * difficulty involved.
 */
class max_subdivision_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * A class for the exception if a roundoff error occurs.
 *
 * From <scr/appl/integate.c>:
 * the occurrence of roundoff error is detected, which prevents the requested
 * tolerance from being achieved. the error may be under-estimated.
 */
class roundoff_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * A class for the exception if bad integrand behaviour occurs
 *
 * From <scr/appl/integate.c>:
 * extremely bad integrand behaviour occurs at some points of the integration
 * interval.
 */
class bad_integrand_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * A class for the exception if roundoff errors in the extrapolation table
 * occur
 *
 * From <scr/appl/integate.c>:
 * the algorithm does not converge. roundoff error is detected in the
 * extrapolation table. it is assumed that the requested tolerance cannot be
 * achieved, and that the returned result is the best which can be obtained.
 */
class extrapolation_roundoff_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * A class for the exception if divergence is detected
 *
 * From <scr/appl/integate.c>:
 * the integral is probably divergent, or slowly convergent. it must be noted
 * that divergence can occur with any other value of ier.
 */
class divergence_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * A class for the exception if inputs are invalid
 *
 * From <scr/appl/integate.c>:
 * the input is invalid, because (epsabs <= 0 and epsrel <
 * max(50*rel.mach.acc.,0.5d-28)) or limit < 1 or leniw < limit*4. result,
 * abserr, neval, last are set to zero. exept when limit or leniw is invalid,
 * iwork(1), work(limit*2+1) and work(limit*3+1) are set to zero, work(1) is
 * set to a and work(limit+1) to b.
 */
struct invalid_input_error : public integration_logic_error {
public:
  using integration_logic_error::integration_logic_error;
};

// -------------------------------------------------------------------------------------------------
// # Implementations
// -------------------------------------------------------------------------------------------------

template <typename Lambda_>
inline integrator::result_type
integrator::operator()(Lambda_ fn, const double lower,
                       const double upper) const {
  if (std::isnan(lower) || std::isnan(upper))
    throw invalid_input_error("the input is invalid");

  auto fn_callback = [](double *x, int n, void *ex) {
    auto fn_ptr = static_cast<decltype(&fn)>(ex);
    for (auto i = 0; i < n; ++i) {
      x[i] = (*fn_ptr)(x[i]);
    }
    return;
  };

  auto epsabs = cfg_.epsabs();
  auto epsrel = cfg_.epsrel();
  auto limit = cfg_.limit();
  auto lenw = cfg_.lenw();

  auto result = 0.;
  auto abserr = 0.;
  auto ier = 0;
  auto neval = 0;
  auto last = 0;
  auto iwork = std::vector<int>(cfg_.limit());
  auto work = std::vector<double>(cfg_.lenw());

  if (std::isfinite(lower) && std::isfinite(upper)) {
    Rdqags(fn_callback, &fn, const_cast<double *>(&lower),
           const_cast<double *>(&upper), &epsabs, &epsrel, &result, &abserr,
           &neval, &ier, &limit, &lenw, &last, iwork.data(), work.data());
  } else {
    auto inf = 0;
    auto bound = 0.;
    if (std::isfinite(lower)) {
      inf = 1;
      bound = lower;
    } else if (std::isfinite(upper)) {
      inf = -1;
      bound = upper;
    } else {
      inf = 2;
      bound = 0.;
    }
    Rdqagi(fn_callback, &fn, &bound, &inf, &epsabs, &epsrel, &result, &abserr,
           &neval, &ier, &limit, &lenw, &last, iwork.data(), work.data());
  }
  auto out = result_type{result, abserr, last, neval};
  if (ier > 0) {
    if (ier == 1)
      throw max_subdivision_error("maximum number of subdivisions reached",
                                  out);
    if (ier == 2)
      throw roundoff_error("roundoff error was detected", out);
    if (ier == 3)
      throw bad_integrand_error("extremely bad integrand behaviour", out);
    if (ier == 4)
      throw extrapolation_roundoff_error(
          "roundoff error is detected in the extrapolation table", out);
    if (ier == 5)
      throw divergence_error("the integral is probably divergent", out);
    if (ier == 6)
      throw invalid_input_error("the input is invalid", out);
  }

  return out;
};

template <typename Lambda_>
integrator::result_type integrate(Lambda_ fn, const double lower,
                                  const double upper,
                                  const integrator::config_type &config) {
  return integrator{config}(fn, lower, upper);
}

inline integrator::result_type::result_type(const double value,
                                            const double abserr,
                                            const int subdivisions,
                                            const int neval)
    : value_{value}, abserr_{abserr}, subdivisions_{subdivisions}, neval_{
                                                                       neval} {}

inline double integrator::result_type::value() const noexcept { return value_; }
inline double integrator::result_type::abserr() const noexcept {
  return abserr_;
}
inline int integrator::result_type::subdivisions() const noexcept {
  return subdivisions_;
}
inline int integrator::result_type::neval() const noexcept { return neval_; }

inline integrator::config_type::config_type(const int limit,
                                            const double epsrel)
    : config_type{limit, epsrel, epsrel, 4 * limit} {}

inline integrator::config_type::config_type(const int limit,
                                            const double epsrel,
                                            const double epsabs)
    : config_type{limit, epsrel, epsabs, 4 * limit} {}

inline integrator::config_type::config_type(const int limit,
                                            const double epsrel,
                                            const double epsabs, const int lenw)
    : limit_{limit}, epsrel_{epsrel}, epsabs_{epsabs}, lenw_{lenw} {
  if (limit_ <= 0)
    throw invalid_input_error("the input is invalid");
  if (epsabs_ <= 0. &&
      epsrel_ <
          std::max(50. * std::numeric_limits<double>::epsilon(), 0.5e-28)) {
    throw invalid_input_error("the input is invalid");
  }
  if (!(lenw_ >= 4 * limit_))
    throw invalid_input_error("the input is invalid");
}

inline auto integrator::config_type::limit() const noexcept
    -> decltype(limit_) {
  return limit_;
}
inline auto integrator::config_type::epsrel() const noexcept
    -> decltype(epsrel_) {
  return epsrel_;
}
inline auto integrator::config_type::epsabs() const noexcept
    -> decltype(epsabs_) {
  return epsabs_;
}
inline auto integrator::config_type::lenw() const noexcept -> decltype(lenw_) {
  return lenw_;
}

inline integrator::integrator(const config_type &cfg) noexcept : cfg_{cfg} {}
inline integrator::integrator(const int limit, const double epsrel)
    : cfg_{limit, epsrel} {}
inline integrator::integrator(const int limit, const double epsrel,
                              const double epsabs)
    : cfg_{limit, epsrel, epsabs} {}
inline integrator::integrator(const int limit, const double epsrel,
                              const double epsabs, const int lenw)
    : cfg_{limit, epsrel, epsabs, lenw} {}

inline auto integrator::config() const noexcept -> decltype(cfg_) {
  return cfg_;
}

// -------------------------------------------------------------------------------------------------
// ## Implementations of exception classes
// -------------------------------------------------------------------------------------------------

inline integration_runtime_error::integration_runtime_error(
    const char *what, const integrator::result_type &result)
    : std::runtime_error{what}, result_{result} {}

inline integration_runtime_error::integration_runtime_error(
    const std::string what, const integrator::result_type &result)
    : std::runtime_error{what}, result_{result} {}

inline integrator::result_type
integration_runtime_error::result() const noexcept {
  return result_;
}

inline integration_logic_error::integration_logic_error(
    const char *what, const integrator::result_type &result)
    : std::logic_error{what}, result_{result} {}

inline integration_logic_error::integration_logic_error(
    const std::string what, const integrator::result_type &result)
    : std::logic_error{what}, result_{result} {}

inline integrator::result_type
integration_logic_error::result() const noexcept {
  return result_;
}

} // namespace integratecpp
