#pragma once

#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <R_ext/Applic.h>

namespace integratecpp {

//!  A Functor wrapping `Rdqags` and `Rdqagi` declared in `<R_ext/Applic.h`.
class integrator {
public:
  //! A class for the integation results
  class result_type {
  private:
    //! A `double` with the approximated value.
    double value_ = 0.;

    //! A `double` with the approximated absolute error.
    double abserr_ = 0.;

    //! An `int` with the no. of subdivisions.
    int subdivisions_ = 0;

    //! An `int` with the no. of neval.
    int neval_ = 0;

  public:
    result_type() = default;
    /*!
     * Full constructor
     * \param value         a `double` with the approximated value.
     * \param abserr        a `double` with the approximated absolute error.
     * \param subdivisions  an `int` with the no. of subdivisions.
     * \param neval         an `int` with the no. of neval.
     */
    result_type(const double value, const double abserr, const int subdivisions,
                const int neval);

    //! Accessor to the approximated value.
    auto value() const noexcept -> decltype(value_);

    //! Accessor to the approximated absolute error.
    auto abserr() const noexcept -> decltype(abserr_);

    //! Accessor to the no. of subdivisions.
    auto subdivisions() const noexcept -> decltype(subdivisions_);

    //! Accessor to the no. of neval.
    auto neval() const noexcept -> decltype(neval_);
  };

  //! A class for the integration configuration parameters
  class config_type {
  private:
    //! An `int` for the max. no. of subdivisions.
    int limit_ = 100;

    //! A `double` for the requested relative accuracy.
    double epsrel_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);

    //! A `double` for the requested absolute accuracy.
    double epsabs_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);

    //! An `int` with `lenw >= 4 * limit` for the dimensioning parameter.
    int lenw_ = 400;

  public:
    config_type() = default;

    /*!
     * Partial constructor
     * \param limit   an `int` for the max. no. of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     */
    config_type(const int limit, const double epsrel);

    /*!
     * Partial constructor
     * \param limit   an `int` for the max. no. of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     * \param epsrel  a `double` for the requested absolute accuracy.
     */
    config_type(const int limit, const double epsrel, const double epsabs);

    /*!
     * Full constructor
     * \param limit   an `int` for the max. no. of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     * \param epsabs  a `double` for the requested absolute accuracy.
     * \param lenw    an `int` with `lenw >= 4 * limit` for the dimensioning
     *                parameter.
     */
    config_type(const int limit, const double epsrel, const double epsabs,
                const int lenw);

    //! Accessor to the max. no. of subdivisions.
    auto limit() const noexcept -> decltype(limit_);

    //! Accessor to the requested relative accuracy.
    auto epsrel() const noexcept -> decltype(epsrel_);

    //! Accessor to the requested absolute accuracy.
    auto epsabs() const noexcept -> decltype(epsabs_);

    //! Accessor to the dimensioning parameter.
    auto lenw() const noexcept -> decltype(lenw_);
  };

private:
  //! A `config_type` with the integration configuration
  const config_type cfg_{};

public:
  integrator() = default;

  /*!
   * Full constructor using `config_param`
   * \param cfg  A `config_param`.
   */
  integrator(const config_type &cfg);

  integrator(const int limit, const double epsrel);

  integrator(const int limit, const double epsrel, const double epsabs);

  auto config() const noexcept -> decltype(cfg_);

  /*!
   * Full constructor
   * \param limit   an `int` for the max. no. of subdivisions.
   * \param lenw    an `int` with `lenw >= 4 * limit` for the dimensioning
   *                parameter.
   * \param epsabs  a `double` for the requested absolute accuracy.
   * \param epsrel  a `double` for the requested relative accuracy.
   *
   * \exception     std::invalid_argument
   */
  integrator(const int limit, const double epsrel, const double epsabs,
             const int lenw);

  /*!
   * Integrator Lambda-functor, using `Rdqags` if both bounds are are finite and
   * `Rdqagi` of at least one of the bounds is infinite.
   *
   * \param fn     a `Lambda_` functor with a (const) double argument.
   * \param lower  a `double` for the lower bound.
   * \param upper  a `double` for the upper bound. \return A `return_type` with
   *               the integration results.
   */
  template <typename Lambda_>
  result_type operator()(Lambda_ fn, const double lower,
                         const double upper) const;
};

/*!
 * A wrapper for the `integate`-functor.
 * \param fn     a `Lambda_` functor with a (const) double argument.
 * \param lower  a `double` for the lower bound.
 * \param upper  a `double` for the upper bound.
 * \param config  a `config_type` configuration parameter.
 *
 * \return A `return_type` with the integration results.
 */
template <typename Lambda_>
integrator::result_type
integrate(Lambda_ fn, const double lower, const double upper,
          const integrator::config_type config = integrator::config_type{});

/*!
 * A class for all exceptions of the integration routine.
 */
class integration_error : public std::exception {
public:
  using result_type = integrator::result_type;

private:
  result_type result_;

public:
  explicit integration_error(const result_type &result);
  virtual ~integration_error() noexcept;

  integration_error() = delete;
  integration_error(const integration_error &other) noexcept;
  integration_error &operator=(const integration_error &other) noexcept;

  virtual result_type result() const noexcept;
};

class max_subdivision_error : public std::runtime_error,
                              public integration_error {
public:
  using integration_error::integration_error;
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;

  explicit max_subdivision_error(const std::string what,
                                 const result_type &result);
  explicit max_subdivision_error(const char *what, const result_type &result);
};

class roundoff_error : public std::runtime_error, public integration_error {
public:
  using integration_error::integration_error;
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;

  explicit roundoff_error(const char *what, const result_type &result);
  explicit roundoff_error(const std::string what, const result_type &result);
};

class bad_integrand_error : public std::runtime_error,
                            public integration_error {
public:
  using integration_error::integration_error;
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;

  explicit bad_integrand_error(const char *what, const result_type &result);
  explicit bad_integrand_error(const std::string what,
                               const result_type &result);
};

class extrapolation_roundoff_error : public std::runtime_error,
                                     public integration_error {
public:
  using integration_error::integration_error;
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;

  explicit extrapolation_roundoff_error(const char *what,
                                        const result_type &result);
  explicit extrapolation_roundoff_error(const std::string what,
                                        const result_type &result);
};

class divergence_error : public std::runtime_error, public integration_error {
public:
  using integration_error::integration_error;
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;

  explicit divergence_error(const char *what, const result_type &result);
  explicit divergence_error(const std::string what, const result_type &result);
};

struct invalid_input_error : public std::logic_error, public integration_error {
public:
  using integration_error::integration_error;
  using std::logic_error::logic_error;
  using std::logic_error::what;

  explicit invalid_input_error(const char *what, const result_type &result);
  explicit invalid_input_error(const std::string what,
                               const result_type &result);
};

// -------------------------------------------------------------------------------------------------
// # Implementations
// -------------------------------------------------------------------------------------------------

template <typename Lambda_>
inline integrator::result_type
integrator::operator()(Lambda_ fn, const double lower,
                       const double upper) const {
  if (std::isnan(lower) || std::isnan(upper))
    throw std::invalid_argument("a limit is NA or NaN");

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
                                  const integrator::config_type config) {
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
    throw std::invalid_argument("invalid parameter values");
  if (epsabs_ <= 0. &&
      epsrel_ <=
          std::max(50. * std::numeric_limits<double>::epsilon(), 0.5e-28)) {
    throw std::invalid_argument("invalid parameter values");
  }
  if (!(lenw_ >= 4 * limit_))
    throw std::invalid_argument("lenw >= 4 * limit required");
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

inline integrator::integrator(const config_type &cfg) : cfg_{cfg} {}
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

inline integration_error::integration_error(const result_type &result)
    : result_{result} {}

inline integration_error::~integration_error() noexcept {}

inline integration_error::integration_error(
    const integration_error &other) noexcept
    : result_{other.result_} {
  assert(this != &other);
}

inline integration_error &
integration_error::operator=(const integration_error &other) noexcept {
  if (this != &other) {
    result_ = other.result_;
  }
  return *this;
}

inline integrator::result_type integration_error::result() const noexcept {
  return result_;
}

inline max_subdivision_error::max_subdivision_error(const char *what,
                                                    const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline max_subdivision_error::max_subdivision_error(const std::string what,
                                                    const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline roundoff_error::roundoff_error(const char *what,
                                      const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline roundoff_error::roundoff_error(const std::string what,
                                      const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline bad_integrand_error::bad_integrand_error(const char *what,
                                                const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline bad_integrand_error::bad_integrand_error(const std::string what,
                                                const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline extrapolation_roundoff_error::extrapolation_roundoff_error(
    const char *what, const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline extrapolation_roundoff_error::extrapolation_roundoff_error(
    const std::string what, const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline divergence_error::divergence_error(const char *what,
                                          const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline divergence_error::divergence_error(const std::string what,
                                          const result_type &result)
    : std::runtime_error(what), integration_error(result) {}

inline invalid_input_error::invalid_input_error(const char *what,
                                                const result_type &result)
    : std::logic_error(what), integration_error(result) {}

inline invalid_input_error::invalid_input_error(const std::string what,
                                                const result_type &result)
    : std::logic_error(what), integration_error(result) {}

} // namespace integratecpp
