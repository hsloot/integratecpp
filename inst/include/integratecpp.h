#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>

#include <R_ext/Applic.h>
#include <vector>

namespace integratecpp {

class integrator {
public:
  struct result_type {
    const double value;
    const double abserr;
    const int subdivisions;
    const int neval;
  };

  struct config_type {
    config_type() = default;
    config_type(const int limit, const int lenw, const double epsabs,
                const double epsrel);

    const int limit = 100;
    const int lenw = 400;
    const double epsabs =
        std::pow(std::numeric_limits<double>::epsilon(), 0.25);
    const double epsrel =
        std::pow(std::numeric_limits<double>::epsilon(), 0.25);
  };

  integrator() = default;
  integrator(const config_type &cfg);
  integrator(const int limit, const int lenw, const double epsabs,
             const double epsrel);

  template <typename Lambda_>
  auto operator()(Lambda_ fn, const double lower, const double upper) const;

private:
  const config_type cfg_{};
};

inline integrator::config_type::config_type(const int limit, const int lenw,
                                            const double epsabs,
                                            const double epsrel)
    : limit{limit}, lenw{lenw}, epsabs{epsabs}, epsrel{epsrel} {
  if (limit <= 0)
    throw std::invalid_argument("limit > 0 required");
  if (epsabs <= 0. &&
      epsrel <=
          std::max(50. * std::numeric_limits<double>::epsilon(), 0.5e-28)) {
    throw std::invalid_argument(
        "epsabs > 0 or epsrel > max(epsilon, 0.5e-28) required");
  }
  if (!(lenw >= 4 * limit))
    throw std::invalid_argument("lenw >= 4 * limit required");
}

struct integration_error : public std::exception {
  using result_type = integrator::result_type;

  integration_error() = delete;
  integration_error(const result_type &result) : result{result} {}
  const char *what() const noexcept override {
    return "error reported by integration routine";
  }

  const result_type result;
};

struct max_subdivision_error : public integration_error {
  max_subdivision_error(const result_type &result)
      : integration_error{result} {}
  const char *what() const noexcept override {
    return "maximum number of subdivisions reached";
  }
};

struct roundoff_error : public integration_error {
  roundoff_error(const result_type &result) : integration_error{result} {}
  const char *what() const noexcept override {
    return "roundoff error was detected";
  }
};

struct bad_integrand_error : public integration_error {
  bad_integrand_error(const result_type &result) : integration_error{result} {}
  const char *what() const noexcept override {
    return "extremely bad integrand behaviour";
  }
};

struct extrapolation_roundoff_error : public integration_error {
  extrapolation_roundoff_error(const result_type &result)
      : integration_error{result} {}
  const char *what() const noexcept override {
    return "roundoff error is detected in the extrapolation table";
  }
};

struct divergence_error : public integration_error {
  divergence_error(const result_type &result) : integration_error{result} {}
  const char *what() const noexcept override {
    return "the integral is probably divergent";
  }
};

struct invalid_input_error : public integration_error {
  invalid_input_error(const result_type &result) : integration_error{result} {}
  const char *what() const noexcept override { return "the input is invalid"; }
};

inline integrator::integrator(const config_type &cfg) : cfg_{cfg} {}
inline integrator::integrator(const int limit, const int lenw,
                              const double epsabs, const double epsrel)
    : cfg_{limit, lenw, epsabs, epsrel} {}

template <typename Lambda_>
inline auto integrator::operator()(Lambda_ fn, const double lower,
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

  auto result = 0.;
  auto abserr = 0.;
  auto ier = 0;
  auto neval = 0;
  auto last = 0;
  auto iwork = std::vector<int>(cfg_.limit);
  auto work = std::vector<double>(cfg_.lenw);

  if (std::isfinite(lower) && std::isfinite(upper)) {
    Rdqags(fn_callback, &fn, const_cast<double *>(&lower),
           const_cast<double *>(&upper), const_cast<double *>(&cfg_.epsabs),
           const_cast<double *>(&cfg_.epsrel), &result, &abserr, &neval, &ier,
           const_cast<int *>(&cfg_.limit), const_cast<int *>(&cfg_.lenw), &last,
           iwork.data(), work.data());
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
    Rdqagi(fn_callback, &fn, &bound, &inf, const_cast<double *>(&cfg_.epsabs),
           const_cast<double *>(&cfg_.epsrel), &result, &abserr, &neval, &ier,
           const_cast<int *>(&cfg_.limit), const_cast<int *>(&cfg_.lenw), &last,
           iwork.data(), work.data());
  }
  const auto out = result_type{result, abserr, last, neval};
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
