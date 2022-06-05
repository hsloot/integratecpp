#pragma once
//! \file integratecpp.h

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <R_ext/Applic.h>

// TODO: comment calls to `noexcept(<cond>)` if `<cond>` is known to be `true`
//       by `static_assert`.

namespace integratecpp {

/*!
 * \brief  A functor wrapping `Rdqags` and `Rdqagi` declared in
 *   `     <R_ext/Applic.h>` and implemented in `src/appl/integrate.c`.
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
   * \brief  Defines a data class for the integation results returned from
   *         `integratecpp::integrator::operator()()`.
   *
   * Contains the following data elements (compare `src/appl/integrate.c` in
   * R-source):
   * - `double value = 0.`:     The approximation of the integral.
   * - `double abserr = 0.`:    The estimate of the modules of the absolute
   *                            error, which should be equal or larger than
   *                            `abs(I-result)`.
   * - `int subdivisions = 0`:  The final number of subintervals produced in
   *                            the subdivision process.
   * - `int neval = 0`:         The number of integrand evaluations.
   */
  struct return_type {
    //! \brief The approximated value.
    double value;
    //! \brief The estimated absolute error.
    double abserr;
    //! \brief The final number of subdivisions.
    int subdivisions;
    //! \brief The number of function evaluations.
    int neval;

    return_type() noexcept = default;
    /*!
     * \brief  The full constructor.
     *
     * \param value         a `double` with the approximated value.
     * \param abserr        a `double` with the estimated absolute error.
     * \param subdivisions  an `int` with the final number of subdivisions.
     * \param neval         an `int` with the number of neval.
     */
    explicit constexpr return_type(const double value, const double abserr,
                                   const int subdivisions,
                                   const int neval) noexcept;
  };
  static_assert(std::is_nothrow_default_constructible<return_type>::value,
                "`integratecpp::integator::return_type` not nothrow "
                "default-constructible");
  static_assert(
      std::is_nothrow_copy_constructible<return_type>::value,
      "`integratecpp::integator::return_type` not nothrow copy-constructible");
  static_assert(
      std::is_nothrow_copy_assignable<return_type>::value,
      "`integratecpp::integator::return_type` not nothrow copy-assignable");
  static_assert(
      std::is_nothrow_move_constructible<return_type>::value,
      "`integratecpp::integator::return_type` not nothrow move-constructible");
  static_assert(
      std::is_nothrow_move_assignable<return_type>::value,
      "`integratecpp::integator::return_type` not nothrow move-assignable");
  static_assert(std::is_trivial<return_type>::value,
                "`integratecpp::integator::return_type` not trivial");
  static_assert(std::is_standard_layout<return_type>::value,
                "`integratecpp::integator::return_type` not standard layout");

  /*!
   * \brief  Defines a data class for the integration configuration parameters
   *         used in `integratecpp::integrator::operator()()`.
   *
   * Contains the following data elements (see `src/appl/integrate.c` in
   * R-source):
   * - `int limit = 100`:                    The maximum number of subintervals
   *                                         in the partition of the given
   *                                         integration interval (lower,
   *                                         upper).
   * - `double epsrel = rel.mach.acc.^.25`:  The requested relative accuracy.
   * - `double epsabs = rel.mach.acc.^.25`:  The requested absolute accuracy.
   * - `int lenw = 400`:                     A dimensioning parameter for the
   *                                         working array.
   *
   * Warning:  Objects of type `integratecpp::integrator::config_type` are
   *           allowed to have states which are invalid for
   *           `integatecpp::integrator::operator()()`. Use dedicated validity
   *           methods to assert an object's validity.
   */
  struct config_type {
    //! \brief The maximum number of subdivisions.
    int limit{100};
    //! \brief The requested relative accuracy.
    double epsrel{std::pow(std::numeric_limits<double>::epsilon(),
                           0.25)}; // = 0.0001220703125
    //! \brief The requested absolute accuracy.
    double epsabs{epsrel};
    //! \brief The dimensioning parameter of the working array.
    int lenw{400};

    // NOTE: default constructor of `config_type` is technically
    //       `noexcept(false)` since `std::pow` is `noexcept(false)` as it might
    //       throw. however, for the values used it should not throw. hence, we
    //       set it to `noexcept(true)`.
    config_type() noexcept = default;

    /*!
     * \brief  A partial constructor for `limit` and `epsrel`.
     *
     * \param limit   an `int` for the maximum number of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     *
     * \exception     throws integratecpp::invalid_input_error if limit < 1.
     */
    explicit constexpr config_type(const int limit,
                                   const double epsrel) noexcept;

    /*!
     * \brief  A partial constructor for `limit`, `epsrel`, and `epsabs`.
     *
     * \param limit   an `int` for the maximum number of subdivisions.
     * \param epsrel  a `double` for the requested relative accuracy.
     * \param epsabs  a `double` for the requested absolute accuracy.
     *
     * \exception     throws integratecpp::invalid_input_error if limit < 1.
     * \exception     throws integratecpp::invalid_input_error if epsabs <= 0
     *                and epsrel < max(50*rel.mach.acc.,0.5d-28).
     */
    explicit constexpr config_type(const int limit, const double epsrel,
                                   const double epsabs) noexcept;

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
    explicit constexpr config_type(const int limit, const double epsrel,
                                   const double epsabs,
                                   const int lenw) noexcept;

    //! \brief Indicates whether object is in a valid state for usage in
    //!        `integratecpp::integrator::operator()()`.
    bool is_valid() const noexcept;

    /*!
     * \brief   Asserts whether object is in a valid state for usage i n
     *          `integratecpp::integrator::operator()()`, possibly with an
     *          informative error message.
     *
     * \exception  throws integratecpp::invalid_input_error.
     */
    void assert_validity() const;
  };
  static_assert(std::is_nothrow_default_constructible<config_type>::value,
                "`integratecpp::integator::config_type` not nothrow "
                "default-constructible");
  static_assert(
      std::is_nothrow_copy_constructible<config_type>::value,
      "`integratecpp::integator::config_type` not nothrow copy-constructible");
  static_assert(
      std::is_nothrow_copy_assignable<config_type>::value,
      "`integratecpp::integator::config_type` not nothrow copy-assignable");
  static_assert(
      std::is_nothrow_move_constructible<config_type>::value,
      "`integratecpp::integator::config_type` not nothrow move-constructible");
  static_assert(
      std::is_nothrow_move_assignable<config_type>::value,
      "`integratecpp::integator::config_type` not nothrow move-assignable");
  // NOTE: `integratecpp::integrator::config_type` is not trivial as it has
  //        non-trivial member intializations.
  // static_assert(std::is_trivial<config_type>::value,
  //               "`integratecpp::integator::config_type` not trivial");
  static_assert(std::is_standard_layout<config_type>::value,
                "`integratecpp::integator::config_type` not standard layout");

private:
  config_type config_{};

public:
  integrator() noexcept(
      std::is_nothrow_default_constructible<config_type>::value) = default;

  /*!
   * \brief  A full constructor using `integratecpp::integrator::config_type`.
   *
   * \param config  a `integratecpp::integrator::config_type`.
   */
  explicit constexpr integrator(const config_type &config) noexcept(
      std::is_nothrow_copy_constructible<config_type>::value);

  /*!
   * \brief  A partial constructor using `limit` and `epsrel`.
   *
   * \param limit   an `int` for the maximum number of subdivisions.
   * \param epsrel  a `double` for the requested relative accuracy.
   *
   * \exception     throws integratecpp::invalid_input_error if limit < 1.
   */
  explicit constexpr integrator(const int limit, const double epsrel) noexcept;

  /*!
   * \brief  A partial constructor using `limit`, `epsrel`, and `epsabs`.
   *
   * \param limit   an `int` for the maximum number of subdivisions.
   * \param epsrel  a `double` for the requested relative accuracy.
   * \param epsabs  a `double` for the requested absolute accuracy.
   *
   * \exception     throws integratecpp::invalid_input_error if limit < 1.
   * \exception     throws integratecpp::invalid_input_error if epsabs <= 0
   *                and epsrel < max(50*rel.mach.acc.,0.5d-28).
   */
  explicit constexpr integrator(const int limit, const double epsrel,
                                const double epsabs) noexcept;

  /*!
   * \brief  A full constructor using `limit`, `epsrel`, `epsabs`, and `lenw`.
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
  explicit constexpr integrator(const int limit, const double epsrel,
                                const double epsabs, const int lenw) noexcept;

  //! \brief Accessor for the configuration parameters.
  constexpr auto config() const
      noexcept(std::is_nothrow_copy_assignable<config_type>::value)
          -> decltype(config_);

  //! \brief Setter for the configuration parameters.
  void config(const config_type &config) noexcept;

  //! \brief Accessor to the maximum number of subdivisions.
  constexpr auto limit() const noexcept -> decltype(config_.limit);

  //! \brief Setter to the maximum number of subdivisions.
  void limit(const int limit) noexcept;

  //! \brief Accessor to the requested relative accuracy.
  constexpr auto epsrel() const noexcept -> decltype(config_.epsrel);

  //! \brief Setter to the requested relative accuracy.
  void epsrel(const double epsrel) noexcept;

  //! \brief Accessor to the requested absolute accuracy.
  constexpr auto epsabs() const noexcept -> decltype(config_.epsabs);

  //! \brief Setter to the requested absolute accuracy.
  void epsabs(const double epsabs) noexcept;

  //! \brief Accessor to the dimensioning parameter of the working array.
  constexpr auto lenw() const noexcept -> decltype(config_.lenw);

  //! \brief Setter to the dimensioning parameter of the working array.
  void lenw(const int lenw) noexcept;

  //! \brief Indicates whether object is in a valid state for usage in
  //!        `integratecpp::integrator::operator()()`.
  bool is_valid() const noexcept;

  /*!
   * \brief   Asserts whether object is in a valid state for usage i n
   *          `integratecpp::integrator::operator()()`, possibly with an
   *          informative error message.
   *
   * \exception  throws integratecpp::invalid_input_error.
   */
  void assert_validity() const;

  /*!
   * \brief  Approximates an integratal numerically for a Lambda-functor, lower,
   *         and upper bound, using `Rdqags` if both bounds are are finite and
   *         `Rdqagi` of at least one of the bounds is infinite.
   *
   * \param fn     a `UnaryRealFunction_` functor compatible with a
   *               `const double` signature.
   *               \param lower  a `double` for the lower bound.
   * \param upper  a `double` for the upper bound.
   *
   * \return       a `integratecpp::integrator::return_type` with the
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
  template <typename UnaryRealFunction_>
  return_type operator()(UnaryRealFunction_ &&fn, const double lower,
                         const double upper) const;
};
static_assert(std::is_nothrow_default_constructible<integrator>::value,
              "`integratecpp::integator::integrator` not nothrow "
              "default-constructible");
static_assert(std::is_nothrow_copy_constructible<integrator>::value,
              "`integratecpp::integator` not nothrow copy-constructible");
static_assert(std::is_nothrow_copy_assignable<integrator>::value,
              "`integratecpp::integator` not nothrow copy-assignable");
static_assert(std::is_nothrow_move_constructible<integrator>::value,
              "`integratecpp::integator` not nothrow move-constructible");
static_assert(std::is_nothrow_move_assignable<integrator>::value,
              "`integratecpp::integator` not nothrow move-assignable");
// NOTE: `integratecpp::integrator` is not trivial as it has
//        non-trivial member intializations.
// static_assert(std::is_trivial<integrator>::value,
//               "`integratecpp::integator` not trivial");
static_assert(std::is_standard_layout<integrator>::value,
              "`integratecpp::integator` not standard layout");

/*!
 * \brief  A drop-in replacement of `integratecpp::integrator` for numerical
 *         integration. Approximates an integratal numerically for a
 *         Lambda-functor, lower, and upper bound, using `Rdqags` if both bounds
 *         are are finite and `Rdqagi` of at least one of the bounds is
 *         infinite.
 *
 * \param fn      a `UnaryRealFunction_` functor compatible with a `const
 *                double` signature.
 * \param lower   a `double` for the lower bound.
 * \param upper   a `double` for the upper bound.
 * \param config  a `const` reference to a
 *                `integratecpp::integrator::config_type` configuration
 *                parameter.
 *
 * \return        a `integratecpp::integrator::return_type` with the
 *                integration results.
 *
 * \exception     throws integratecpp::max_subdivision_error if the maximal
 *                number of subdivisions is reached without fulfilling required
 *                error conditions.
 * \exception     throws integratecpp::roundoff_error if a roundoff error is
 *                detected which prevents the requested accuracy from being
 *                achieved.
 * \exception     throws integratecpp::bad_integrand_error if extremely bad
 *                integrand behaviour is detected during integration.
 * \exception     throws integratecpp::extrapolation_roundoff_error if a
 *                roundoff error is detected in the extrapolation table.
 * \exception     throws integratecpp::divergence_error if the integral is
 *                deemed divergence (or slowly convergent).
 */
template <typename UnaryRealFunction_>
integrator::return_type
integrate(UnaryRealFunction_ &&fn, const double lower, const double upper,
          const integrator::config_type config = integrator::config_type{});

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` and are due to events beyond the scope of
 *         the program and not easily predicted.
 */
class integration_runtime_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;

  /*!
   * \brief  A full constructor, specifying the error message with
   *         `std::string`.
   *
   * \param  what     a `std::string` containing the error message.
   * \param  result   a `integratecpp::integrator::return_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_runtime_error(const std::string what,
                                     const integrator::return_type &result);

  /*!
   * \brief  A full constructor, specifying the error message with
   *         `const char *`.
   *
   * \param  what     a `const char *` containing a pointer a a const char array
   *                  with the error message.
   * \param  result   a `integratecpp::integrator::return_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_runtime_error(const char *what,
                                     const integrator::return_type &result);

  //! \brief Accessor the the result at the time or error.
  virtual integrator::return_type result() const noexcept;

private:
  integrator::return_type result_{};
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` and that are a consequence of faulty
 *         logic within the program such as violating logical preconditions or
 *         class invariants and may be preventable.
 */
class integration_logic_error : public std::logic_error {

public:
  using std::logic_error::logic_error;

  /*!
   * \brief  A full constructor, specifying the error message with
   *         `std::string`.
   *
   * \param  what     a `std::string` containing the error message.
   * \param  result   a `integratecpp::integrator::return_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_logic_error(const std::string what,
                                   const integrator::return_type &result);

  /*!
   * \brief  A full constructor, specifying the error message with
   *         `const char *`.
   *
   * \param  what     a `const char *` containing a pointer a a const char array
   *                  with the error message.
   * \param  result   a `integratecpp::integrator::return_type` with
   *                  the integration results at the time of error.
   */
  explicit integration_logic_error(const char *what,
                                   const integrator::return_type &result);

  //! \brief Accessor the the result at the time or error.
  virtual integrator::return_type result() const noexcept;

private:
  integrator::return_type result_{};
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if the maximum number of subdivisions
 *         allowed has been achieved.
 *
 * One can allow more subdivisions by increasing the value of limit (and
 * taking the according dimension adjustments into account).
 * However, if this yields no improvement it is advised to analyze the integrand
 * in order to determine the integration difficulties.
 * If the position of a local difficulty can be determined (e.g. singularity,
 * discontinuity within the interval) one will probably gain from splitting up
 * the interval at this point and calling the integrator on the subranges.
 * If possible, an appropriate special-purpose integrator should be used, which
 * is designed for handling the type of difficulty involved.
 */
class max_subdivision_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if the occurrence of roundoff error is
 *         detected, which prevents the requested tolerance from being achieved.
 *
 * The error may be under-estimated.
 */
class roundoff_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if extremely bad integrand behaviour
 *         occurs at some points of the integration interval.
 */
class bad_integrand_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if the algorithm does not converge.
 *         roundoff error is detected in the extrapolation table.
 *
 * It is assumed that the requested tolerance cannot be achieved, and that the
 * returned result is the best which can be obtained.
 */
class extrapolation_roundoff_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if the integral is probably divergent, or
 *         slowly convergent.
 *
 * It must be noted that divergence can occur with any other value of ier.
 */
class divergence_error : public integration_runtime_error {
public:
  using integration_runtime_error::integration_runtime_error;
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if the integral is probably divergent, or
 *         slowly convergent. if the input is invalid.
 *
 * This could be because (`epsabs <= 0` and
 * `epsrel < max(50*rel.mach.acc.,0.5d-28)`) or `limit < 1` or `lenw <
 * limit*4`. Result, `abserr`, `neval`, last are set to zero.
 */
struct invalid_input_error : public integration_logic_error {
public:
  using integration_logic_error::integration_logic_error;
};

// -------------------------------------------------------------------------------------------------
// Implementations of aux-functions in integratecpp::details
// -------------------------------------------------------------------------------------------------

namespace details {

template <typename InputIt_, typename OutputIt_, typename UnaryRealFunction_>
inline void
guarded_transform(InputIt_ first, InputIt_ last, OutputIt_ d_first,
                  UnaryRealFunction_ &&fn,
                  std::unique_ptr<integration_runtime_error> &e_ptr) {
  const auto cleanup = [](OutputIt_ first, std::size_t size) {
    try {
      std::fill_n(first, size, 0.);
    } catch (...) {
    }
  };
  try {
    std::transform(first, last, d_first, std::forward<UnaryRealFunction_>(fn));
  } catch (const std::bad_alloc &e) {
    // memory allocation issues inside std::transform must not be ignored
    std::throw_with_nested(e);
  } catch (const std::exception &e) {
    cleanup(d_first, std::distance(first, last));
    e_ptr.reset(new integration_runtime_error(e.what()));
  } catch (...) {
    cleanup(d_first, std::distance(first, last));
    e_ptr.reset(new integration_runtime_error("Unknown error"));
  }
}

template <typename UnaryRealFunction_>
inline void integrand_callback(double *x, int n, void *ex) {
#if __cplusplus >= 201703L
  static_assert(
      std::is_invocable_r<double, UnaryRealFunction_, const double>::value,
      "UnaryRealFunction_ is not invocable with `const double` and return "
      "value `double`");
#else
  static_assert(
      std::is_constructible<
          std::function<double(const double)>,
          std::reference_wrapper<
              typename std::remove_reference<UnaryRealFunction_>::type>>::value,
      "`UnaryRealFunction_` is not convertible to `std::function<double(const "
      "double)>`");
#endif
  using iterator = double *;
  using const_iterator = const double *;
  const auto cbegin = [](const double *x) {
    return static_cast<const_iterator>(&x[0]);
  };
  const auto cend = [](const double *x, const int n) {
    return static_cast<const_iterator>(&x[n]);
  };
  const auto begin = [](double *x) { return static_cast<iterator>(&x[0]); };
  // auto end = [](double *x, const int n) {
  //   return static_cast<iterator>(&x[n]);
  // };
  using ex_t =
      std::pair<typename std::remove_reference<UnaryRealFunction_>::type,
                std::unique_ptr<integration_runtime_error>>;

  const auto &fn_integrand = (*static_cast<ex_t *>(ex)).first;
  auto &e_ptr = (*static_cast<ex_t *>(ex)).second;
  details::guarded_transform(cbegin(x), cend(x, n), begin(x), fn_integrand,
                             e_ptr);
}

} // namespace details

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrator::operator()(...)
// -------------------------------------------------------------------------------------------------

template <typename UnaryRealFunction_>
inline integrator::return_type
integrator::operator()(UnaryRealFunction_ &&fn, const double lower,
                       const double upper) const {
  const auto assert_validity = [this](const double lower, const double upper) {
    this->assert_validity();
    if (std::isnan(lower) || std::isnan(upper))
      throw invalid_input_error("the input is invalid");
    return;
  };
  assert_validity(lower, upper);

#if __cplusplus >= 201703L
  auto [limit, epsrel, epsabs, lenw] = config_;
#else
  auto limit = config_.limit;
  auto epsrel = config_.epsrel;
  auto epsabs = config_.epsabs;
  auto lenw = config_.lenw;
#endif

  auto out = return_type{};
#if __cplusplus >= 201703L
  auto &[value, abserr, subdivisions, neval] = out;
#else
  auto &value = out.value;
  auto &abserr = out.abserr;
  auto &subdivisions = out.subdivisions;
  auto &neval = out.neval;
#endif

  auto ier = 0;
  auto iwork = std::vector<int>(config_.limit);
  auto work = std::vector<double>(config_.lenw);

  auto ex = std::make_pair(std::forward<UnaryRealFunction_>(fn),
                           std::unique_ptr<integration_runtime_error>());

  if (std::isfinite(lower) && std::isfinite(upper)) {
    Rdqags(details::integrand_callback<
               typename std::remove_reference<UnaryRealFunction_>::type>,
           &ex, const_cast<double *>(&lower), const_cast<double *>(&upper),
           &epsabs, &epsrel, &value, &abserr, &neval, &ier, &limit, &lenw,
           &subdivisions, iwork.data(), work.data());
  } else {
    const auto translate_bounds = [](const double lower, const double upper) {
      int inf;
      double bound;
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
      return std::make_pair(bound, inf);
    };
#if __cplusplus >= 201703L
    auto [bound, inf] = translate_bounds(lower, upper);
#else
    auto bounds_info = translate_bounds(lower, upper);
    auto bound = std::move(bounds_info.first);
    auto inf = std::move(bounds_info.second);
#endif

    Rdqagi(details::integrand_callback<
               typename std::remove_reference<UnaryRealFunction_>::type>,
           &ex, &bound, &inf, &epsabs, &epsrel, &value, &abserr, &neval, &ier,
           &limit, &lenw, &subdivisions, iwork.data(), work.data());
  }
  const auto translate_error =
      [&out](const int ier, std::unique_ptr<integration_runtime_error> &e_ptr) {
        if (e_ptr.get() != nullptr) {
          std::throw_with_nested(*e_ptr);
        }
        if (ier > 0) {
          // invalid argument errors should be caught during initialization
          assert(ier < 6);
          if (ier == 1) {
            throw max_subdivision_error(
                "maximum number of subdivisions reached", out);

          } else if (ier == 2) {
            throw roundoff_error("roundoff error was detected", out);
          } else if (ier == 3) {
            throw bad_integrand_error("extremely bad integrand behaviour", out);
          } else if (ier == 4) {
            throw extrapolation_roundoff_error(
                "roundoff error is detected in the extrapolation table", out);
          } else if (ier == 5) {
            throw divergence_error("the integral is probably divergent", out);
          } else {
            throw std::logic_error( // # nocov
                "invalid argument errors should be caught during "
                "initialization"); // # nocov
          }
        }
        return;
      };
  translate_error(ier, ex.second);

  return out;
};

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrate::(...)
// -------------------------------------------------------------------------------------------------

template <typename UnaryRealFunction_>
inline integrator::return_type integrate(UnaryRealFunction_ &&fn,
                                         const double lower, const double upper,
                                         const integrator::config_type config) {
  return integrator{config}(std::forward<UnaryRealFunction_>(fn), lower, upper);
}

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrator::return_type
// -------------------------------------------------------------------------------------------------

inline constexpr integrator::return_type::return_type(const double value,
                                                      const double abserr,
                                                      const int subdivisions,
                                                      const int neval) noexcept
    : value{value}, abserr{abserr}, subdivisions{subdivisions}, neval{neval} {}

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrator::config_type
// -------------------------------------------------------------------------------------------------

inline constexpr integrator::config_type::config_type(
    const int limit, const double epsrel) noexcept
    : config_type{limit, epsrel, epsrel, 4 * limit} {}

inline constexpr integrator::config_type::config_type(
    const int limit, const double epsrel, const double epsabs) noexcept
    : config_type{limit, epsrel, epsabs, 4 * limit} {}

inline constexpr integrator::config_type::config_type(const int limit,
                                                      const double epsrel,
                                                      const double epsabs,
                                                      const int lenw) noexcept
    : limit{limit}, epsrel{epsrel}, epsabs{epsabs}, lenw{lenw} {}

inline bool integrator::config_type::is_valid() const noexcept {
  if (limit <= 0) {
    return false;
  } else if (epsabs <= 0. &&
             epsrel < std::max(50. * std::numeric_limits<double>::epsilon(),
                               0.5e-28)) {
    return false;
  } else if (lenw < 4 * limit) {
    return false;
  } else {
    return true;
  }
}

inline void integrator::config_type::assert_validity() const {
  if (!is_valid())
    throw invalid_input_error("the input is invalid");
}

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrator
// -------------------------------------------------------------------------------------------------

inline constexpr integrator::integrator(const config_type &config) noexcept
    : config_{config} {}
inline constexpr integrator::integrator(const int limit,
                                        const double epsrel) noexcept
    : config_{limit, epsrel} {}
inline constexpr integrator::integrator(const int limit, const double epsrel,
                                        const double epsabs) noexcept
    : config_{limit, epsrel, epsabs} {}
inline constexpr integrator::integrator(const int limit, const double epsrel,
                                        const double epsabs,
                                        const int lenw) noexcept
    : config_{limit, epsrel, epsabs, lenw} {}

inline constexpr auto integrator::config() const noexcept -> decltype(config_) {
  return config_;
}
inline void integrator::config(const config_type &config) noexcept {
  config_ = config;
}

inline constexpr auto integrator::limit() const noexcept
    -> decltype(config_.limit) {
  return config_.limit;
}
inline void integrator::limit(const int limit) noexcept {
  config_.limit = limit;
}

inline constexpr auto integrator::epsrel() const noexcept
    -> decltype(config_.epsrel) {
  return config_.epsrel;
}
inline void integrator::epsrel(const double epsrel) noexcept {
  config_.epsrel = epsrel;
}

inline constexpr auto integrator::epsabs() const noexcept
    -> decltype(config_.epsabs) {
  return config_.epsabs;
}
inline void integrator::epsabs(const double epsabs) noexcept {
  config_.epsabs = epsabs;
}

inline constexpr auto integrator::lenw() const noexcept
    -> decltype(config_.lenw) {
  return config_.lenw;
}
inline void integrator::lenw(const int lenw) noexcept { config_.lenw = lenw; }

inline bool integrator::is_valid() const noexcept { return config_.is_valid(); }

inline void integrator::assert_validity() const { config_.assert_validity(); }

// -------------------------------------------------------------------------------------------------
// ## Implementations of exception classes
// -------------------------------------------------------------------------------------------------

inline integration_runtime_error::integration_runtime_error(
    const char *what, const integrator::return_type &result)
    : std::runtime_error{what}, result_{result} {}

inline integration_runtime_error::integration_runtime_error(
    const std::string what, const integrator::return_type &result)
    : std::runtime_error{what}, result_{result} {}

inline integrator::return_type
integration_runtime_error::result() const noexcept {
  return result_;
}

inline integration_logic_error::integration_logic_error(
    const char *what, const integrator::return_type &result)
    : std::logic_error{what}, result_{result} {}

inline integration_logic_error::integration_logic_error(
    const std::string what, const integrator::return_type &result)
    : std::logic_error{what}, result_{result} {}

inline integrator::return_type
integration_logic_error::result() const noexcept {
  return result_;
}

} // namespace integratecpp
