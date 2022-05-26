#pragma once

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include <R_ext/Applic.h>
#include <vector>

namespace integratecpp {

class integrator {
public:
  class result_type {
  public:
    result_type() = default;
    result_type(const double value, const double abserr, const int subdivisions,
                const int neval);

    double value() const noexcept { return value_; }
    double abserr() const noexcept { return abserr_; }
    int subdivisions() const noexcept { return subdivisions_; }
    int neval() const noexcept { return neval_; }

  private:
    const double value_ = 0.;
    const double abserr_ = 0.;
    const int subdivisions_ = 0;
    const int neval_ = 0;
  };

  class config_type {
  public:
    config_type() {}
    config_type(const int limit, const int lenw, const double epsabs,
                const double epsrel);

    int limit() const noexcept { return limit_; }
    int lenw() const noexcept { return lenw_; }
    double epsabs() const noexcept { return epsabs_; }
    double epsrel() const noexcept { return epsrel_; }

  private:
    int limit_ = 100;
    int lenw_ = 400;
    double epsabs_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);
    double epsrel_ = std::pow(std::numeric_limits<double>::epsilon(), 0.25);
  };

  integrator() = default;
  integrator(const config_type &cfg);
  integrator(const int limit, const int lenw, const double epsabs,
             const double epsrel);

  template <typename Lambda_>
  result_type operator()(Lambda_ fn, const double lower,
                         const double upper) const;

private:
  const config_type cfg_{};
};

inline integrator::result_type::result_type(const double value,
                                            const double abserr,
                                            const int subdivisions,
                                            const int neval)
    : value_{value}, abserr_{abserr}, subdivisions_{subdivisions}, neval_{
                                                                       neval} {}

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

class integration_error : public std::exception {
public:
  using result_type = integrator::result_type;

  integration_error() = delete;
  integration_error(const result_type &result) : result_{result} {}

  const char *what() const noexcept override {
    return "error reported by integration routine";
  }

  result_type result() const noexcept;

private:
  const result_type result_;
};

inline integrator::result_type integration_error::result() const noexcept {
  return result_;
}

class max_subdivision_error : public integration_error {
public:
  max_subdivision_error() = delete;
  max_subdivision_error(const result_type &result)
      : integration_error{result} {}

  const char *what() const noexcept override {
    return "maximum number of subdivisions reached";
  }
};

class roundoff_error : public integration_error {
public:
  roundoff_error() = delete;
  roundoff_error(const result_type &result) : integration_error{result} {}

  const char *what() const noexcept override {
    return "roundoff error was detected";
  }
};

class bad_integrand_error : public integration_error {
public:
  bad_integrand_error() = delete;
  bad_integrand_error(const result_type &result) : integration_error{result} {}

  const char *what() const noexcept override {
    return "extremely bad integrand behaviour";
  }
};

class extrapolation_roundoff_error : public integration_error {
public:
  extrapolation_roundoff_error() = delete;
  extrapolation_roundoff_error(const result_type &result)
      : integration_error{result} {}

  const char *what() const noexcept override {
    return "roundoff error is detected in the extrapolation table";
  }
};

class divergence_error : public integration_error {
public:
  divergence_error() = delete;
  divergence_error(const result_type &result) : integration_error{result} {}

  const char *what() const noexcept override {
    return "the integral is probably divergent";
  }
};

struct invalid_input_error : public integration_error {
public:
  invalid_input_error() = delete;
  invalid_input_error(const result_type &result) : integration_error{result} {}

  const char *what() const noexcept override { return "the input is invalid"; }
};

inline integrator::integrator(const config_type &cfg) : cfg_{cfg} {}
inline integrator::integrator(const int limit, const int lenw,
                              const double epsabs, const double epsrel)
    : cfg_{limit, lenw, epsabs, epsrel} {}

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

} // namespace integratecpp
