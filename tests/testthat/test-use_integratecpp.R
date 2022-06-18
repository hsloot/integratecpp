# Copyright (C) 2022 Henrik Sloot
#
# This file is part of integratecpp
#
# integratecpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# integratecpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

skip_if_not_installed("usethis")
skip_if_not_installed("fs")
skip_if_not_installed("desc")
skip_if_not_installed("rlang")

test_that("`use_integratecpp()` fails if `usethis::proj_get()` is not a package", {
    pkg <- create_local_project()

    expect_snapshot(use_integratecpp(), error = TRUE, transform = function(lines) {
      gsub(
        "(?<=No root directory found in )(.*?)(?= or its parent directories.)",
        "<temporary-dir>",
        lines,
        perl = TRUE
      )
    })
})

test_that("`use_integratecpp()` fails if `usethis` is not available", {
  skip_if_not_installed("mockery")
  pkg <- create_local_package()

  mockery::stub(use_integratecpp, "requireNamespace", function(package, ...) {
    if (isTRUE(package == "usethis")) FALSE else base::requireNamespace(package, ...)
  })

  expect_snapshot(use_integratecpp(), error = TRUE)
})

test_that("`use_integratecpp()` fails if `rprojroot` is not available", {
  skip_if_not_installed("mockery")
  pkg <- create_local_package()

  mockery::stub(use_integratecpp, "requireNamespace", function(package, ...) {
    if (isTRUE(package == "rprojroot")) FALSE else base::requireNamespace(package, ...)
  })

  expect_snapshot(use_integratecpp(), error = TRUE)
})

test_that("`use_integratecpp()` fails if `fs` is not available", {
  skip_if_not_installed("mockery")
  pkg <- create_local_package()

  mockery::stub(use_integratecpp, "requireNamespace", function(package, ...) {
    if (isTRUE(package == "fs")) FALSE else base::requireNamespace(package, ...)
  })

  expect_snapshot(use_integratecpp(), error = TRUE)
})

test_that("`use_integratecpp()` fails if `desc` is not available", {
  skip_if_not_installed("mockery")
  pkg <- create_local_package()

  mockery::stub(use_integratecpp, "requireNamespace", function(package, ...) {
    if (isTRUE(package == "desc")) FALSE else base::requireNamespace(package, ...)
  })

  expect_snapshot(use_integratecpp(), error = TRUE)
})

test_that("`use_integratecpp()` fails if `rlang` is not available", {
  skip_if_not_installed("mockery")
  pkg <- create_local_package()

  mockery::stub(use_integratecpp, "requireNamespace", function(package, ...) {
    if (isTRUE(package == "rlang")) FALSE else base::requireNamespace(package, ...)
  })

  expect_snapshot(use_integratecpp(), error = TRUE)
})

test_that("`use_integratecpp()` fails if package not using `roxygen2`", {
  skip_if_not_installed("mockery")
  pkg <- create_local_package()

  tryCatch(
      desc::desc_del(c("Roxygen", "RoxygenNote"), usethis::proj_path()),
      error = function(e) invisible()
  )

  expect_snapshot(use_integratecpp(), error = TRUE)
})

test_that("`use_integratecpp()` works as expected on bare package", {
  pkg <- create_local_package()

  if (!desc::desc_has_fields("RoxygenNote")) {
    skip("`create_local_package()` post-conditions not fulfilled")
  }

  ## verify that call is successful
  expect_error(use_integratecpp(), NA)

  ## verify dependency to `integratecpp` of type `"LinkingTo"` exists
  expect_true(desc::desc_has_dep("integratecpp", "LinkingTo"))

  ## verify `SystemRequirements` contains `"C++11"`
  expect_true(desc::desc_has_fields("SystemRequirements"))
  expect_match(desc::desc_get_field("SystemRequirements", default = character()), "C\\+\\+11")

  ## verify `./src` directory exists
  expect_true(fs::dir_exists(fs::path(usethis::proj_path(), "src")))

  ## verify `./src/integrate.cpp` exists
  skip_if_not(expect_true(fs::dir_exists(fs::path(usethis::proj_path(), "src"))))
  expect_true(fs::file_exists(fs::path(usethis::proj_path(), "src", "integrate.cpp")))

  ## verify `./src/integrate.cpp` matches template
  skip_if_not(fs::file_exists(fs::path(usethis::proj_path(), "src", "integrate.cpp")))
  skip_if_not(fs::file_exists(system.file("templates", "integrate.cpp", package = "integratecpp")))
  expect_equal(
    readLines(fs::path(usethis::proj_path(), "src", "integrate.cpp")),
    readLines(system.file("templates", "integrate.cpp", package = "integratecpp"))
  )
})

test_that("`use_integratecpp()` works as expected with `Rcpp`", {
    skip_if_not_installed("Rcpp")

    pkg <- create_local_package()

    if (!desc::desc_has_fields("RoxygenNote")) {
        skip("`create_local_package()` post-conditions not fulfilled")
    }

    usethis::use_rcpp()
    if (!desc::desc_has_dep("Rcpp", "LinkingTo") ||
        !desc::desc_has_dep("Rcpp", "Imports")) {
        skip("`use_rcpp()` post-conditions not fulfilled")
    }

    ## verify that call is successful
    expect_error(use_integratecpp(), NA)

    ## verify dependency to `integratecpp` of type `"LinkingTo"` exists
    expect_true(desc::desc_has_dep("integratecpp", "LinkingTo"))

    ## verify `SystemRequirements` contains `"C++11"`
    expect_true(desc::desc_has_fields("SystemRequirements"))
    expect_match(desc::desc_get_field("SystemRequirements", default = character()), "C\\+\\+11")
})

test_that("output of `use_integratecpp()` is as expected", {
  pkg <- create_local_package()

  ## snapshot output
  withr::local_options(list(usethis.quiet = FALSE))
  expect_snapshot(use_integratecpp(), transform = function(lines) {
    gsub("(testpkg[[:alnum:]]+)", "<pkg-name>", lines)
  })
})

test_that("output of running `use_integratecpp()` twice is as expected", {
  pkg <- create_local_package()

  use_integratecpp()
  ## snapshot output
  withr::local_options(list(usethis.quiet = FALSE))
  expect_snapshot(use_integratecpp())
})

test_that("output of running `use_integratecpp()` twice with version is as expected", {
  pkg <- create_local_package()

  use_integratecpp(min_version = TRUE)
  ## snapshot output
  withr::local_options(list(usethis.quiet = FALSE))
  expect_snapshot(use_integratecpp(min_version = TRUE))
})

test_that("output of running `use_integratecpp()` with other `SystemRequirements` is as expected", {
  pkg <- create_local_package()

  desc::desc_set("SystemRequirements", "GNU make")
  ## snapshot output
  withr::local_options(list(usethis.quiet = FALSE))
  expect_snapshot(use_integratecpp(), transform = function(lines) {
    gsub("(testpkg[[:alnum:]]+)", "<pkg-name>", lines)
  })

  expect_equal(desc::desc_get_field("SystemRequirements"), "GNU make, C++11")
})

test_that("output of running `use_integratecpp()` with `SystemRequirements < C++11` is as expected", { # nolint
  pkg <- create_local_package()

  desc::desc_set("SystemRequirements", "C++98")
  ## snapshot output
  withr::local_options(list(usethis.quiet = FALSE))
  expect_snapshot(use_integratecpp(), transform = function(lines) {
    gsub("(testpkg[[:alnum:]]+)", "<pkg-name>", lines)
  })

  expect_equal(desc::desc_get_field("SystemRequirements"), "C++11")
})

test_that("`use_integratecpp()` works as expected with `cpp11`", {
    skip_if_not_installed("cpp11")
    pkg <- create_local_package()

    if (!desc::desc_has_fields("RoxygenNote")) {
        skip("`create_local_package()` post-conditions not fulfilled")
    }

    usethis::use_cpp11()
    if (!desc::desc_has_dep("cpp11", "LinkingTo") ||
        !desc::desc_has_fields("SystemRequirements") ||
        !grepl("C\\+\\+11", desc::desc_get_field("SystemRequirements", default = character()))) {
        skip("`use_cpp11()` post-conditions not fulfilled")
    }

    ## verify that call is successful
    expect_error(use_integratecpp(), NA)

    ## verify dependency to `integratecpp` of type `"LinkingTo"` exists
    expect_true(desc::desc_has_dep("integratecpp", "LinkingTo"))

    ## verify `SystemRequirements` contains `"C++11"`
    expect_true(desc::desc_has_fields("SystemRequirements"))
    expect_match(desc::desc_get_field("SystemRequirements", default = character()), "C\\+\\+11")
})
