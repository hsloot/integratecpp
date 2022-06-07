#pragma once
//! \file integratecpp.h

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <exception>
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
   * - `double absolute_error = 0.`:
   *                            The estimate of the modules of the absolute
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
    double absolute_error;
    //! \brief The final number of subdivisions.
    int subdivisions;
    //! \brief The number of function evaluations.
    int neval;

    return_type() noexcept = default;
    /*!
     * \brief  The full constructor.
     *
     * \param value           a `double` with the approximated value.
     * \param absolute_error  a `double` with the estimated absolute error.
     * \param subdivisions    an `int` with the final number of subdivisions.
     * \param neval           an `int` with the number of neval.
     */
    explicit constexpr return_type(const double value,
                                   const double absolute_error,
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
   * - `int max_subdivisions = 100`:    The maximum number of subintervals in
   *                                    the partition of the given integration
   *                                    interval (lower, upper).
   * - `double relative_accuracy = rel.mach.acc.^.25`:
   *                                    The requested relative accuracy.
   * - `double absolute_accuracy = rel.mach.acc.^.25`:
   *                                    The requested absolute accuracy.
   * - `int work_size = 400`:           A dimensioning parameter for the working
   *                                    array.
   *
   * Warning:  Objects of type `integratecpp::integrator::config_type` are
   *           allowed to have states which are invalid for
   *           `integatecpp::integrator::operator()()`. Use dedicated validity
   *           methods to assert an object's validity.
   */
  struct config_type {
    //! \brief The maximum number of subdivisions.
    int max_subdivisions{100};
    //! \brief The requested relative accuracy.
    double relative_accuracy{std::pow(std::numeric_limits<double>::epsilon(),
                                      0.25)}; // = 0.0001220703125
    //! \brief The requested absolute accuracy.
    double absolute_accuracy{relative_accuracy};
    //! \brief The dimensioning parameter of the working array.
    int work_size{400};

    // NOTE: default constructor of `config_type` is technically
    //       `noexcept(false)` since `std::pow` is `noexcept(false)` as it might
    //       throw. however, for the values used it should not throw. hence, we
    //       set it to `noexcept(true)`.
    config_type() noexcept = default;

    /*!
     * \brief  A partial constructor for `max_subdivisions` and
     *         `relative_accuracy`.
     *
     * \param max_subdivisions   an `int` for the maximum number of
     *                           subdivisions.
     * \param relative_accuracy  a `double` for the requested relative accuracy.
     *
     * \exception     throws integratecpp::invalid_input_error if
     *                max_subdivisions < 1.
     */
    explicit constexpr config_type(const int max_subdivisions,
                                   const double relative_accuracy) noexcept;

    /*!
     * \brief  A partial constructor for `max_subdivisions`,
     *         `relative_accuracy`, and `absolute_accuracy`.
     *
     * \param max_subdivisions   an `int` for the maximum number of
     *                           subdivisions.
     * \param relative_accuracy  a `double` for the requested relative accuracy.
     * \param absolute_accuracy  a `double` for the requested absolute accuracy.
     *
     * \exception     throws integratecpp::invalid_input_error if
     *                max_subdivisions < 1.
     * \exception     throws integratecpp::invalid_input_error if
     *                absolute_accuracy <= 0 and relative_accuracy <
     *                max(50*rel.mach.acc.,0.5d-28).
     */
    explicit constexpr config_type(const int max_subdivisions,
                                   const double relative_accuracy,
                                   const double absolute_accuracy) noexcept;

    /*!
     * \brief The full constructor.
     *
     * \param max_subdivisions   an `int` for the maximum number of
     *                           subdivisions.
     * \param relative_accuracy  a `double` for the requested relative accuracy.
     * \param absolute_accuracy  a `double` for the requested absolute accuracy.
     * \param work_size          an `int` for the size of the working array.
     *
     * \exception     throws integratecpp::invalid_input_error if
     *                max_subdivisions < 1.
     * \exception     throws integratecpp::invalid_input_error if
     *                absolute_accuracy <= 0 and relative_accuracy <
     *                max(50*rel.mach.acc.,0.5d-28).
     * \exception     throws integratecpp::invalid_input_error if
     *                work_size < 4 * max_subdivisions.
     */
    explicit constexpr config_type(const int max_subdivisions,
                                   const double relative_accuracy,
                                   const double absolute_accuracy,
                                   const int work_size) noexcept;

    //! \brief  Indicates whether object is in a valid state for usage in
    //!         `integratecpp::integrator::operator()()`.
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
   * \brief  A partial constructor using `max_subdivisions` and
   *         `relative_accuracy`.
   *
   * \param max_subdivisions   an `int` for the maximum number of subdivisions.
   * \param relative_accuracy  a `double` for the requested relative accuracy.
   *
   * \exception     throws integratecpp::invalid_input_error if
   *                max_subdivisions < 1.
   */
  explicit constexpr integrator(const int max_subdivisions,
                                const double relative_accuracy) noexcept;

  /*!
   * \brief  A partial constructor using `max_subdivisions`,
   *         `relative_accuracy`, and `absolute_accuracy`.
   *
   * \param max_subdivisions   an `int` for the maximum number of subdivisions.
   * \param relative_accuracy  a `double` for the requested relative accuracy.
   * \param absolute_accuracy  a `double` for the requested absolute accuracy.
   *
   * \exception     throws integratecpp::invalid_input_error if
   *                max_subdivisions < 1.
   * \exception     throws integratecpp::invalid_input_error if
   *                absolute_accuracy <= 0 and
   *                relative_accuracy < max(50*rel.mach.acc.,0.5d-28).
   */
  explicit constexpr integrator(const int max_subdivisions,
                                const double relative_accuracy,
                                const double absolute_accuracy) noexcept;

  /*!
   * \brief  A full constructor using `max_subdivisions`, `relative_accuracy`,
   *         `absolute_accuracy`, and `work_size`.
   *
   * \param max_subdivisions   an `int` for the maximum number of subdivisions.
   * \param relative_accuracy  a `double` for the requested relative accuracy.
   * \param absolute_accuracy  a `double` for the requested absolute accuracy.
   * \param work_size          an `int` for the size of the working array.
   *
   * \exception     throws integratecpp::invalid_input_error if
   *                max_subdivisions < 1.
   * \exception     throws integratecpp::invalid_input_error if
   *                absolute_accuracy <= 0 and
   *                relative_accuracy < max(50*rel.mach.acc.,0.5d-28).
   * \exception     throws integratecpp::invalid_input_error if
   *                work_size < 4 * max_subdivisions.
   */
  explicit constexpr integrator(const int max_subdivisions,
                                const double relative_accuracy,
                                const double absolute_accuracy,
                                const int work_size) noexcept;

  //! \brief Accessor for the configuration parameters.
  constexpr auto config() const
      noexcept(std::is_nothrow_copy_assignable<config_type>::value)
          -> decltype(config_);

  //! \brief Setter for the configuration parameters.
  void config(const config_type &config) noexcept;

  //! \brief Accessor to the maximum number of subdivisions.
  constexpr auto max_subdivisions() const noexcept
      -> decltype(config_.max_subdivisions);

  //! \brief Setter to the maximum number of subdivisions.
  void max_subdivisions(const int max_subdivisions) noexcept;

  //! \brief Accessor to the requested relative accuracy.
  constexpr auto relative_accuracy() const noexcept
      -> decltype(config_.relative_accuracy);

  //! \brief Setter to the requested relative accuracy.
  void relative_accuracy(const double relative_accuracy) noexcept;

  //! \brief Accessor to the requested absolute accuracy.
  constexpr auto absolute_accuracy() const noexcept
      -> decltype(config_.absolute_accuracy);

  //! \brief Setter to the requested absolute accuracy.
  void absolute_accuracy(const double absolute_accuracy) noexcept;

  //! \brief Accessor to the dimensioning parameter of the working array.
  constexpr auto work_size() const noexcept -> decltype(config_.work_size);

  //! \brief Setter to the dimensioning parameter of the working array.
  void work_size(const int work_size) noexcept;

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
   * \param lower  a `double` for the lower bound.
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
private:
  integrator::return_type result_{};

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
private:
  integrator::return_type result_{};

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
};

/*!
 * \brief  Defines a type of object to be thrown as exception. It reports errors
 *         that occur during the integration routine of
 *         `integratecpp::integator::operator()()` or
 *         `integratecpp::integrate()` if the maximum number of subdivisions
 *         allowed has been achieved.
 *
 * One can allow more subdivisions by increasing the value of `max_subdivisions`
 * (and taking the according dimension adjustments into account). However, if
 * this yields no improvement it is advised to analyze the integrand in order to
 * determine the integration difficulties. If the position of a local difficulty
 * can be determined (e.g. singularity, discontinuity within the interval) one
 * will probably gain from splitting up the interval at this point and calling
 * the integrator on the subranges. If possible, an appropriate special-purpose
 * integrator should be used, which is designed for handling the type of
 * difficulty involved.
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
 * This could be because (`absolute_accuracy <= 0` and
 * `relative_accuracy < max(50*rel.mach.acc.,0.5d-28)`) or
 * `max_subdivisions < 1` or `work_size < max_subdivisions*4`.
 * Result, `absolute_error`, `neval`, last are set to zero.
 */
struct invalid_input_error : public integration_logic_error {
public:
  using integration_logic_error::integration_logic_error;
};

// -------------------------------------------------------------------------------------------------
// Implementations of internal type_traits in integratecpp::type_traits
// -------------------------------------------------------------------------------------------------

namespace type_traits {

#if __cplusplus >= 201703L
template <typename Fn, typename... Args>
using is_invocable = std::is_invocable<Fn, Args...>;
template <typename R, typename Fn, typename... Args>
using is_invocable_r = std::is_invocable_r<R, Fn, Args...>;
#else
template <typename Fn, typename... Args>
struct is_invocable
    : std::is_constructible<
          std::function<void(Args...)>,
          std::reference_wrapper<typename std::remove_reference<Fn>::type>> {};
template <typename R, typename Fn, typename... Args>
struct is_invocable_r
    : std::is_constructible<
          std::function<R(Args...)>,
          std::reference_wrapper<typename std::remove_reference<Fn>::type>> {};
#endif

} // namespace type_traits

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrator::operator()(...)
// -------------------------------------------------------------------------------------------------

template <typename UnaryRealFunction_>
inline integrator::return_type integrator::operator()(UnaryRealFunction_ &&fn,
                                                      double lower,
                                                      double upper) const {
  static_assert(
      type_traits::is_invocable_r<
          double, typename std::remove_reference<UnaryRealFunction_>::type,
          const double>::value,
      "UnaryRealFunction_ is not invocable with `const double` and return "
      "value `double`");

  // NOTE: check validity of `this->config_` and arguments `lower` and `upper`
  const auto assert_validity = [this](const double lower, const double upper) {
    this->assert_validity();
    if (std::isnan(lower) || std::isnan(upper)) {
      throw invalid_input_error("the input is invalid");
    } else {
      return;
    }
  };
  assert_validity(lower, upper);

  // NOTE: create local copies for input variables (as `Rdqag[si]` interface
  // requires pointers to non-const variables)
  auto limit = config_.max_subdivisions;
  auto epsrel = config_.relative_accuracy;
  auto epsabs = config_.absolute_accuracy;
  auto lenw = config_.work_size;

  // NOTE: create output and local references
  auto out = return_type{};
  auto &result = out.value;
  auto &abserr = out.absolute_error;
  auto &last = out.subdivisions;
  auto &neval = out.neval;

  // NOTE: create variable for error code of `Rdqag[si]`
  auto ier = 0;

  // NOTE: create working array and index array
  auto iwork = std::vector<int>(limit);
  auto work = std::vector<double>(lenw);

  // NOTE: create non-capturing callback Lambda (which can be implicitely
  // converted to a fnct.-pointer of signature `integr_fn` aka
  // `void(double *, int, void *)`).
  // the actual integrand function is passed through the `void *` in the last
  // argument alongside with a `std::unique_ptr` to capture exceptions during
  // function evaluations.
  const auto integrand_callback = [](double *x, int n, void *ex) {
    using iterator = double *;
    using const_iterator = const double *;
    const auto cbegin = [](const double *x) {
      return static_cast<const_iterator>(&x[0]);
    };
    const auto cend = [](const double *x, const int n) {
      return static_cast<const_iterator>(&x[n]);
    };
    const auto begin = [](double *x) { return static_cast<iterator>(&x[0]); };
    using ex_t =
        std::pair<typename std::remove_reference<UnaryRealFunction_>::type,
                  std::exception_ptr>;

    auto &fn_integrand = (*static_cast<ex_t *>(ex)).first;
    auto &e_ptr = (*static_cast<ex_t *>(ex)).second;

    // NOTE: `guarded_transform` is a wrapper arround `std::transform`,
    // catching all exceptions appart `std::bad_alloc` and storing them in the
    // provided `std::unique_ptr`.
    // an additional check is performed whether all results are finite.
    // in case of errors, all function values are set to zero.
    const auto guarded_transform =
        [](const_iterator first, const_iterator last, iterator d_first,
           typename std::remove_reference<UnaryRealFunction_>::type &fn,
           std::exception_ptr &e_ptr) {
          const auto cleanup = [](iterator first, std::size_t size) {
            try {
              std::fill_n(first, size, 0.);
            } catch (...) {
            }
          };
          try {
            std::transform(first, last, d_first, fn);
          } catch (const std::bad_alloc &e) {
            // NOTE: memory allocation issues inside std::transform must not be
            // ignored
            std::rethrow_exception(std::current_exception());
          } catch (const std::exception &e) {
            cleanup(d_first, std::distance(first, last));
            e_ptr = std::current_exception();
          } catch (...) {
            cleanup(d_first, std::distance(first, last));
            e_ptr = std::make_exception_ptr(
                integration_runtime_error("Unknown error"));
          }

          if (!std::all_of(d_first, d_first + std::distance(first, last),
                           [](const double x) { return std::isfinite(x); })) {
            cleanup(d_first, std::distance(first, last));
            e_ptr = std::make_exception_ptr(
                integration_runtime_error("non-finite function value"));
          }
        };
    guarded_transform(cbegin(x), cend(x, n), begin(x), fn_integrand, e_ptr);
  };
  auto ex = std::make_pair(std::forward<UnaryRealFunction_>(fn),
                           std::exception_ptr());

  if (std::isfinite(lower) && std::isfinite(upper)) {
    Rdqags(integrand_callback, &ex, &lower, &upper, &epsabs, &epsrel, &result,
           &abserr, &neval, &ier, &limit, &lenw, &last, iwork.data(),
           work.data());
  } else {
    // NOTE: boundary information requires a transformation for `Rdqagi`.
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
    auto bounds_info = translate_bounds(lower, upper);
    auto bound = std::move(bounds_info.first);
    auto inf = std::move(bounds_info.second);

    Rdqagi(integrand_callback, &ex, &bound, &inf, &epsabs, &epsrel, &result,
           &abserr, &neval, &ier, &limit, &lenw, &last, iwork.data(),
           work.data());
  }

  // NOTE: translate error codes from `Rdqag[is]` and evaluation errors from
  // `fn` to suitable exceptions
  const auto translate_error = [](const int error_code,
                                  std::exception_ptr e_ptr,
                                  const return_type result) {
    if (e_ptr) {
      std::rethrow_exception(e_ptr);
    }
    if (error_code > 0) {
      // NOTE: invalid argument errors should be caught during
      // initialization
      assert(error_code < 6);
      if (error_code == 1) {
        throw max_subdivision_error("maximum number of subdivisions reached",
                                    result);

      } else if (error_code == 2) {
        throw roundoff_error("roundoff error was detected", result);
      } else if (error_code == 3) {
        throw bad_integrand_error("extremely bad integrand behaviour", result);
      } else if (error_code == 4) {
        throw extrapolation_roundoff_error(
            "roundoff error is detected in the extrapolation table", result);
      } else if (error_code == 5) {
        throw divergence_error("the integral is probably divergent", result);
      } else {
        throw std::logic_error( // # nocov
            "invalid argument errors should be caught during "
            "initialization"); // # nocov
      }
    }
    return;
  };
  translate_error(ier, ex.second, out);

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

inline constexpr integrator::return_type::return_type(
    const double value, const double absolute_error, const int subdivisions,
    const int neval) noexcept
    : value{value}, absolute_error{absolute_error},
      subdivisions{subdivisions}, neval{neval} {}

// -------------------------------------------------------------------------------------------------
// Implementations of integratecpp::integrator::config_type
// -------------------------------------------------------------------------------------------------

inline constexpr integrator::config_type::config_type(
    const int max_subdivisions, const double relative_accuracy) noexcept
    : config_type{max_subdivisions, relative_accuracy, relative_accuracy,
                  4 * max_subdivisions} {}

inline constexpr integrator::config_type::config_type(
    const int max_subdivisions, const double relative_accuracy,
    const double absolute_accuracy) noexcept
    : config_type{max_subdivisions, relative_accuracy, absolute_accuracy,
                  4 * max_subdivisions} {}

inline constexpr integrator::config_type::config_type(
    const int max_subdivisions, const double relative_accuracy,
    const double absolute_accuracy, const int work_size) noexcept
    : max_subdivisions{max_subdivisions}, relative_accuracy{relative_accuracy},
      absolute_accuracy{absolute_accuracy}, work_size{work_size} {}

inline bool integrator::config_type::is_valid() const noexcept {
  if (max_subdivisions <= 0) {
    return false;
  } else if (absolute_accuracy <= 0. &&
             relative_accuracy <
                 std::max(50. * std::numeric_limits<double>::epsilon(),
                          0.5e-28)) {
    return false;
  } else if (work_size < 4 * max_subdivisions) {
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
inline constexpr integrator::integrator(const int max_subdivisions,
                                        const double relative_accuracy) noexcept
    : config_{max_subdivisions, relative_accuracy} {}
inline constexpr integrator::integrator(const int max_subdivisions,
                                        const double relative_accuracy,
                                        const double absolute_accuracy) noexcept
    : config_{max_subdivisions, relative_accuracy, absolute_accuracy} {}
inline constexpr integrator::integrator(const int max_subdivisions,
                                        const double relative_accuracy,
                                        const double absolute_accuracy,
                                        const int work_size) noexcept
    : config_{max_subdivisions, relative_accuracy, absolute_accuracy,
              work_size} {}

inline constexpr auto integrator::config() const noexcept -> decltype(config_) {
  return config_;
}
inline void integrator::config(const config_type &config) noexcept {
  config_ = config;
}

inline constexpr auto integrator::max_subdivisions() const noexcept
    -> decltype(config_.max_subdivisions) {
  return config_.max_subdivisions;
}
inline void integrator::max_subdivisions(const int max_subdivisions) noexcept {
  config_.max_subdivisions = max_subdivisions;
}

inline constexpr auto integrator::relative_accuracy() const noexcept
    -> decltype(config_.relative_accuracy) {
  return config_.relative_accuracy;
}
inline void
integrator::relative_accuracy(const double relative_accuracy) noexcept {
  config_.relative_accuracy = relative_accuracy;
}

inline constexpr auto integrator::absolute_accuracy() const noexcept
    -> decltype(config_.absolute_accuracy) {
  return config_.absolute_accuracy;
}
inline void
integrator::absolute_accuracy(const double absolute_accuracy) noexcept {
  config_.absolute_accuracy = absolute_accuracy;
}

inline constexpr auto integrator::work_size() const noexcept
    -> decltype(config_.work_size) {
  return config_.work_size;
}
inline void integrator::work_size(const int work_size) noexcept {
  config_.work_size = work_size;
}

inline bool integrator::is_valid() const noexcept { return config_.is_valid(); }

inline void integrator::assert_validity() const { config_.assert_validity(); }

// -------------------------------------------------------------------------------------------------
// Implementations of exception classes
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
