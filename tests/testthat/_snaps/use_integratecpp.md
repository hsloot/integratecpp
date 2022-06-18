# `use_integratecpp()` fails if `usethis::proj_get()` is not a package

    Code
      use_integratecpp()
    Error <usethis_error>
      `use_integratecpp()` is designed to work within packages

# `use_integratecpp()` fails if `usethis` is not available

    Code
      use_integratecpp()
    Error <simpleError>
      package `usethis` required

# `use_integratecpp()` fails if `rprojroot` is not available

    Code
      use_integratecpp()
    Error <usethis_error>
      package `rprojroot` required

# `use_integratecpp()` fails if `fs` is not available

    Code
      use_integratecpp()
    Error <usethis_error>
      package `fs` required

# `use_integratecpp()` fails if `desc` is not available

    Code
      use_integratecpp()
    Error <usethis_error>
      package `desc` required

# `use_integratecpp()` fails if `rlang` is not available

    Code
      use_integratecpp()
    Error <usethis_error>
      package `rlang` required

# `use_integratecpp()` fails if package not using `roxygen2`

    Code
      use_integratecpp()
    Error <usethis_error>
      `use_integratecpp()` is designed to work within packages using `roxygen2`.

# output of `use_integratecpp()` is as expected

    Code
      use_integratecpp()
    Message <rlang_message>
      v Creating 'src/'
      v Adding '*.o', '*.so', '*.dll' to 'src/.gitignore'
      * Consider adding the following roxygen comment and run `devtools::document()` to update 'NAMESPACE'.
        @useDynLib <pkg-name>, .registration = TRUE
      v Adding 'integratecpp' to LinkingTo field in DESCRIPTION
      * Possible includes are:
        #include <integratecpp.h>
      v Adding 'C++11' to SystemRequirements field in DESCRIPTION
      v Writing 'src/integrate.cpp'

# output of running `use_integratecpp()` twice is as expected

    Code
      use_integratecpp()
    Warning <simpleWarning>
      Repeated calls to `use_integratecpp()` trigger a bug in `usethis::use_package()`, see 'https:/github.com/r-lib/usethis#1648'.
    Message <rlang_message>
      i Removing 'integratecpp' from LinkingTo field in DESCRIPTION.
      v Adding 'integratecpp' to LinkingTo field in DESCRIPTION
      * Possible includes are:
        #include <integratecpp.h>

# output of running `use_integratecpp()` twice with version is as expected

    Code
      use_integratecpp(min_version = TRUE)
    Warning <simpleWarning>
      Repeated calls to `use_integratecpp()` trigger a bug in `usethis::use_package()`, see 'https:/github.com/r-lib/usethis#1648'.
    Message <rlang_message>
      x `use_integratecpp()` cannot update version of 'integratecpp' from LinkingTo field in DESCRIPTION.
      * Consider manually updating version of 'integratecpp' from LinkingTo field in DESCRIPTION.

# output of running `use_integratecpp()` with other `SystemRequirements` is as expected

    Code
      use_integratecpp()
    Message <rlang_message>
      v Creating 'src/'
      v Adding '*.o', '*.so', '*.dll' to 'src/.gitignore'
      * Consider adding the following roxygen comment and run `devtools::document()` to update 'NAMESPACE'.
        @useDynLib <pkg-name>, .registration = TRUE
      v Adding 'integratecpp' to LinkingTo field in DESCRIPTION
      * Possible includes are:
        #include <integratecpp.h>
      v Adding 'C++11' to SystemRequirements field in DESCRIPTION
      v Writing 'src/integrate.cpp'

# output of running `use_integratecpp()` with `SystemRequirements < C++11` is as expected

    Code
      use_integratecpp()
    Message <rlang_message>
      v Creating 'src/'
      v Adding '*.o', '*.so', '*.dll' to 'src/.gitignore'
      * Consider adding the following roxygen comment and run `devtools::document()` to update 'NAMESPACE'.
        @useDynLib <pkg-name>, .registration = TRUE
      v Adding 'integratecpp' to LinkingTo field in DESCRIPTION
      * Possible includes are:
        #include <integratecpp.h>
      v Adding 'C++11' to SystemRequirements field in DESCRIPTION
      v Writing 'src/integrate.cpp'

