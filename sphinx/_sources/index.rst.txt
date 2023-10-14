.. examplelib documentation master file, created by
   sphinx-quickstart on Wed Apr 24 15:19:01 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to integratecpp's documentation!
========================================

.. toctree::
   :maxdepth: 3
   :caption: Contents:

* Back the the `main page <../index.html>`_.
* :ref:`genindex`

Integrator wrapper
==================

.. doxygenclass:: integratecpp::integrator
   :members:

Intergrate drop-in wrapper
==========================

.. doxygenfunction:: integratecpp::integrate

Library exceptions
==================

.. doxygenclass:: integratecpp::integration_runtime_error
   :members:

.. doxygenclass:: integratecpp::integration_logic_error
   :members:

.. doxygenclass:: integratecpp::max_subdivision_error
   :members:

.. doxygenclass:: integratecpp::roundoff_error
   :members:

.. doxygenclass:: integratecpp::bad_integrand_error
   :members:

.. doxygenclass:: integratecpp::extrapolation_roundoff_error
   :members:

.. doxygenclass:: integratecpp::divergence_error
   :members:

.. doxygenclass:: integratecpp::invalid_input_error
   :members:
