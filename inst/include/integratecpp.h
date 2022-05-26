#pragma once

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include <R_ext/Applic.h>

namespace integratecpp {

//!  A Functor wrapping `Rdqags` and `Rdqagi` declared in `<R_ext/Applic.h`.
class integrator {
public:
  //! A class for the integation results
  class result_type {
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
    double value() const noexcept;

    //! Accessor to the approximated absolute error.
    double abserr() const noexcept;

    //! Accessor to the no. of subdivisions.
    int subdivisions() const noexcept;

    //! Accessor to the no. of neval.
    int neval() const noexcept;

  private:
    const double value_ = 0.;
    const double abserr_ = 0.;
    const int subdivisions_ = 0;
    const int neval_ = 0;
  };

  //! A class for the integration configuration parameters
  class config_type {
  public:
    config_type() = default;
    /*!
     * Full constructor
     * \param limit   an `int` for the max. no. of subdivisions.
     * \param lenw    an `int` with `lenw >= 4 * limit` for the dimensioning
     *                parameter.
     * \param epsabs  a `double` for the requested absolute accuracy.
     * \param epsrel  a `double` for the requested relative accuracy.
     */
    config_type(const int limit, const int lenw, const double epsabs,
                const double epsrel);

    //! Accessor to the max. no. of subdivisions.
    int limit() const noexcept;

    //! Accessor to the dimensioning parameter.
    int lenw() const noexcept;

    //! Accessor to the requested absolute accuracy.
    double epsabs() const noexcept;

    //! Accessor to the requested relative accuracy.
    double epsrel() const noexcept;

  private:
    int limit_ = 100;
    int lenw_ = 400;
    double epsabs_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);
    double epsrel_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);
  };

  integrator() = default;

  /*!
   * Full constructor using `config_param`
   * \param cfg  A `config_param`.
   */
  integrator(const config_type &cfg);

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
  integrator(const int limit, const int lenw, const double epsabs,
             const double epsrel);

  /*!
   * Integrator Lambda-functor
   * \param fn     a `Lambda_` functor with a (const) double argument.
   * \param lower  a `double` for the lower bound.
   * \param upper  a `double` for the upper bound.
   * \return A `return_type` with the integration results.
   */
  template <typename Lambda_>
  result_type operator()(Lambda_ fn, const double lower,
                         const double upper) const;

private:
  const config_type cfg_{};
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

class integration_error : public std::exception {
public:
  using result_type = integrator::result_type;

  integration_error() = delete;
  integration_error(const result_type &result);

  const char *what() const noexcept override;

  result_type result() const noexcept;

private:
  const result_type result_;
};

class max_subdivision_error : public integration_error {
public:
  max_subdivision_error() = delete;
  max_subdivision_error(const result_type &result);

  const char *what() const noexcept override;
};

class roundoff_error : public integration_error {
public:
  roundoff_error() = delete;
  roundoff_error(const result_type &result);

  const char *what() const noexcept override;
};

class bad_integrand_error : public integration_error {
public:
  bad_integrand_error() = delete;
  bad_integrand_error(const result_type &result);

  const char *what() const noexcept override;
};

class extrapolation_roundoff_error : public integration_error {
public:
  extrapolation_roundoff_error() = delete;
  extrapolation_roundoff_error(const result_type &result);

  const char *what() const noexcept override;
};

class divergence_error : public integration_error {
public:
  divergence_error() = delete;
  divergence_error(const result_type &result);

  const char *what() const noexcept override;
};

struct invalid_input_error : public integration_error {
public:
  invalid_input_error() = delete;
  invalid_input_error(const result_type &result);

  const char *what() const noexcept override;
};

// -------------------------------------------------------------------------------------------------
// Implementations
// -------------------------------------------------------------------------------------------------

template <typename Lambda_>
inline integrator::result_type
integrator::operator()(Lambda_ fn, const double lower,
                       const double upper) const {
  if (std::isnan(lower) || std::isnan(upper))
    throw std::invalid_argument("lower and upper must not be NaN");

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
      throw max_subdivision_error(out);
    if (ier == 2)
      throw roundoff_error(out);
    if (ier == 3)
      throw bad_integrand_error(out);
    if (ier == 4)
      throw extrapolation_roundoff_error(out);
    if (ier == 5)
      throw divergence_error(out);
    if (ier == 6)
      throw invalid_input_error(out);
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

inline integrator::config_type::config_type(const int limit, const int lenw,
                                            const double epsabs,
                                            const double epsrel)
    : limit_{limit}, lenw_{lenw}, epsabs_{epsabs}, epsrel_{epsrel} {
  if (limit_ <= 0)
    throw std::invalid_argument("limit > 0 required");
  if (epsabs_ <= 0. &&
      epsrel_ <=
          std::max(50. * std::numeric_limits<double>::epsilon(), 0.5e-28)) {
    throw std::invalid_argument(
        "epsabs > 0 or epsrel > max(epsilon, 0.5e-28) required");
  }
  if (!(lenw_ >= 4 * limit_))
    throw std::invalid_argument("lenw >= 4 * limit required");
}

inline int integrator::config_type::limit() const noexcept { return limit_; }
inline int integrator::config_type::lenw() const noexcept { return lenw_; }
inline double integrator::config_type::epsabs() const noexcept {
  return epsabs_;
}
inline double integrator::config_type::epsrel() const noexcept {
  return epsrel_;
}

inline integrator::integrator(const config_type &cfg) : cfg_{cfg} {}
inline integrator::integrator(const int limit, const int lenw,
                              const double epsabs, const double epsrel)
    : cfg_{limit, lenw, epsabs, epsrel} {}

inline integration_error::integration_error(const result_type &result)
    : result_{result} {}

inline const char *integration_error::what() const noexcept {
  return "error reported by integration routine";
}

inline integrator::result_type integration_error::result() const noexcept {
  return result_;
}

inline max_subdivision_error::max_subdivision_error(const result_type &result)
    : integration_error{result} {}

inline const char *max_subdivision_error::what() const noexcept {
  return "maximum number of subdivisions reached";
}

inline roundoff_error::roundoff_error(const result_type &result)
    : integration_error{result} {}

inline const char *roundoff_error::what() const noexcept {
  return "roundoff error was detected";
}

inline bad_integrand_error::bad_integrand_error(const result_type &result)
    : integration_error{result} {}

inline const char *bad_integrand_error::what() const noexcept {
  return "extremely bad integrand behaviour";
}

inline extrapolation_roundoff_error::extrapolation_roundoff_error(
    const result_type &result)
    : integration_error{result} {}

inline const char *extrapolation_roundoff_error::what() const noexcept {
  return "roundoff error is detected in the extrapolation table";
}

inline divergence_error::divergence_error(const result_type &result)
    : integration_error{result} {}

inline const char *divergence_error::what() const noexcept {
  return "the integral is probably divergent";
}

inline invalid_input_error::invalid_input_error(const result_type &result)
    : integration_error{result} {}

inline const char *invalid_input_error::what() const noexcept {
  return "the input is invalid";
}

} // namespace integratecpp
