Installation
============

This header-only library is meant to be used in a shared library that is linked
against the R framework, e.g., in an R package or with Rcpp.

Manual include
--------------

You may copy the standalone header file into a suitable include directory.

R package library
-----------------

You may install the :code:`integratecpp` R package Via

.. code-block:: r

   remotes::install_github("hsloot/integratecpp")

Packages
~~~~~~~~

Include the following in your :code:`DESCRIPTION` file:

.. code-block:: debcontrol

   LinkingTo: integratecpp

Rcpp
~~~~

Use Rcpp's plugin attributes:

.. code-block:: cpp

   // [[Rcpp::plugins(cpp11)]]
   // [[Rcpp::depends(integratecpp)]]

   #include <integratecpp.h>
   // your code
