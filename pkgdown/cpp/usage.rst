Usage
=====

Suppose you want to reimplement an R function :code:`foo` in C++ that requires
numerical integration of a function :code:`bar` over the interval from zero to
one, already implemented in C++.  Using the integratecpp drop-in
wrapper-function, this can be implemented as:

.. code-block:: cpp

   #include <integratecpp.h>

   // declare and define unary function bar

   // [[Rcpp::export]]
   double foo() {
       using integratecpp::integrate;
       try {
           const auto result = integrate(bar, 0., 1.);
           return result;
       } catch (const integratecpp::integration_logic_error &e) {
           Rcpp::stop(e.what());
       } catch (const integratecpp::integration_runtime_error &e) {
           Rcpp::stop(e.what());
       }
   }

You can also use non-default options:

.. code-block:: cpp

   #include <integratecpp.h>

   // declare and define unary function fn

   // [[Rcpp::export]]
   double foo() {
       using integratecpp::integrate;
       using integratecpp::integrator::config_type;
       try {
           const auto result = integrate(bar, 0., 1., config_type{100, 0.125});
           return result;
       } catch (const integratecpp::integration_logic_error &e) {
           Rcpp::stop(e.what());
       } catch (const integratecpp::integration_runtime_error &e) {
           Rcpp::stop(e.what());
       }
   }

If you need to integrate a lot of different functions, e.g., :code:`bar` and
:code:`baz` it might be preferable to use the integrator wrapper-class:

.. code-block:: cpp

   #include <integratecpp.h>

   // declare and define unary function bar and baz

   // [[Rcpp::export]]
   double foo() {
       using integratecpp::integrator;
       using integratecpp::integrator::config_type;
       try {
           auto custom_integrator = integrator{config_type{100, 0.125}};
           const auto result_bar = custom_integrator(bar, 0., 1.);
           const auto result_baz = custom_integrator(baz, 0., 1.);
           return result_bar * result_baz;
       } catch (const integratecpp::integration_logic_error &e) {
           Rcpp::stop(e.what());
       } catch (const integratecpp::integration_runtime_error &e) {
           Rcpp::stop(e.what());
       }
   }
