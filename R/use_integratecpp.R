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

#' Link to `integratecpp`
#'
#' Sets you package up for linking to `integratecpp` headers.
#'
#' @inheritParams usethis::use_package
#' @param name If supplied, creates and opens ⁠src/integrate.cpp.⁠
#'
#' @examples
#' \dontrun{
#' use_integratecpp()
#' use_integratecpp("gamma_inc")
#' use_integratecpp(min_version = TRUE)
#' }
#'
#' @importFrom utils head
#'
#' @export
use_integratecpp <- function(name = NULL, # nolint
                             min_version = NULL) {
    ## nothing works without usethis
    if (!requireNamespace("usethis", quietly = TRUE)) {
        stop("package `usethis` required")
    }

    ## `rprojroot`, `fs`, `desc`, and `rlang` required to emulate
    ## internal functions of `usethis`
    if (!requireNamespace("rprojroot", quietly = TRUE)) {
        usethis::ui_stop("package {usethis::ui_code(\"rprojroot\")} required")
    }
    if (!requireNamespace("fs", quietly = TRUE)) {
        usethis::ui_stop("package {usethis::ui_code(\"fs\")} required")
    }
    if (!requireNamespace("desc", quietly = TRUE)) {
        usethis::ui_stop("package {usethis::ui_code(\"desc\")} required")
    }
    if (!requireNamespace("rlang", quietly = TRUE)) {
        usethis::ui_stop("package {usethis::ui_code(\"rlang\")} required")
    }

    ## define method string
    who_is_asking <- function() "use_integratecpp()" # nolint

    ## verify that `use_integratecpp()` is executed for an `R` project
    ## corresponding to a package
    check_is_r_package <- function() {
        tryCatch(
            fs::is_dir(rprojroot::find_package_root_file(path = usethis::proj_path())),
            error = function(e) FALSE
        )
    }
    if (!check_is_r_package()) {
        usethis::ui_stop(
            "{usethis::ui_code(who_is_asking())} is designed to work within packages"
        )
    }

    ## verify that `use_integratecpp()` is executed for an `R` package
    ## using `roxygen2`
    check_uses_roxygen <- function() {
        desc::desc_has_fields("RoxygenNote", file = usethis::proj_get())
    }
    if (!check_uses_roxygen()) {
        usethis::ui_stop(
            "{usethis::ui_code(who_is_asking())} is designed to work within packages using {usethis::ui_code(\"roxygen2\")}." # nolint
        )
    }

    ## verify that package has source directory
    use_src_directory <- function() {
        if (fs::dir_exists(fs::path(usethis::proj_path(), "src"))) {
            return(invisible())
        } else {
            usethis::use_directory("src")
            usethis::use_git_ignore(c("*.o", "*.so", "*.dll"), "src")
            usethis::ui_todo(
                "Consider adding the following roxygen comment and run {usethis::ui_code(\"devtools::document()\")} to update {usethis::ui_path(\"NAMESPACE\")}." # nolint
            )
            usethis::ui_code_block(
                "@useDynLib {desc::desc_get_field(\"Package\")}, .registration = TRUE"
            )
        }
    }
    use_src_directory()

    ## verify that package links to `integratecpp`;
    ## use workaround for https://github.com/r-lib/usethis#1648
    check_linkingto_integratecpp <- function() {
        desc::desc_has_dep("integratecpp", type = "LinkingTo")
    }
    check_otherdeps_integratecpp <- function() {
        df_deps <- desc::desc_get_deps(usethis::proj_get())
        NROW(df_deps[df_deps[, "package"] == "integratecpp" &
            df_deps[, "type"] != "LinkingTo", , drop = FALSE]) > 0
    }
    if (check_linkingto_integratecpp() && !check_otherdeps_integratecpp()) {
        usethis::ui_warn(
            "Repeated calls to {usethis::ui_code(who_is_asking())} trigger a bug in {usethis::ui_code(\"usethis::use_package()\")}, see {usethis::ui_path(\"https://github.com/r-lib/usethis#1648\")}." # nolint
        )
        check_linkingto_integratecpp_noversion <- function() { # nolint
            df_deps <- desc::desc_get_deps(usethis::proj_get())
            NROW(df_deps[df_deps[, "package"] == "integratecpp" &
                df_deps[, "type"] == "LinkingTo" &
                df_deps[, "version"] == "*", , drop = FALSE]) == 1
        }
        if (check_linkingto_integratecpp_noversion()) {
            usethis::ui_info(
                "Removing {usethis::ui_value(\"integratecpp\")} from {usethis::ui_field(\"LinkingTo\")} field in DESCRIPTION." # nolint
            )
            desc::desc_del_dep("integratecpp", type = "LinkingTo")
            usethis::use_package("integratecpp", type = "LinkingTo", min_version = min_version)
        } else {
            usethis::ui_oops(
                "{usethis::ui_code(who_is_asking())} cannot update version of {usethis::ui_value(\"integratecpp\")} from {usethis::ui_field(\"LinkingTo\")} field in DESCRIPTION." # nolint
            )
            usethis::ui_todo(
                "Consider manually updating version of {usethis::ui_value(\"integratecpp\")} from {usethis::ui_field(\"LinkingTo\")} field in DESCRIPTION." # nolint
            )
        }

    } else {
        usethis::use_package("integratecpp", type = "LinkingTo", min_version = min_version)
    }

    ## verify package has SystemRequirement 'C++11'
    use_cpp_standard_11 <- function() {
        requirements <- desc::desc_get_field(
            "SystemRequirements",
            default = character(),
            file = usethis::proj_get()
        )
        if (length(requirements) == 0) {
            new_requirements <- "C++11"
        } else {
            pattern <- "(C\\+\\+[[:alnum:]]{2})"
            cxx_std <- regmatches(
                    requirements,
                    regexpr(
                        pattern, requirements
                    )
                )
            if (length(cxx_std) == 0) {
                new_requirements <- paste(c(requirements, "C++11"), collapse = ", ")
            } else {
                if (isTRUE(cxx_std %in% c("C++98", "C++03", "C++0x"))) {
                    new_requirements <- gsub(pattern, "C++11", requirements)
                } else {
                    new_requirements <- requirements
                }
            }
        }
        if (!isTRUE(requirements == new_requirements)) {
            usethis::ui_done(
                "Adding {usethis::ui_value(\"C++11\")} to {usethis::ui_field('SystemRequirements')} field in DESCRIPTION" # nolint
            )
            desc::desc_set("SystemRequirements", new_requirements)
        }
    }
    use_cpp_standard_11()

    ## copy template if one of the following conditions is met:
    ## either the `src` directory contains no non-hidden files
    ## or the name of the to-be-created file is explicitely provided
    has_non_hidden_files <- function(path) {
        length(fs::dir_ls(path, all = FALSE)) > 0
    }
    if (!has_non_hidden_files(fs::path(usethis::proj_path(), "src"))) {
        name <- if (is.null(name)) "integrate" else name
    }
    if (!is.null(name)) {
        usethis::use_template(
            "integrate.cpp", fs::path("src", paste0(name, ".cpp")),
            open = rlang::is_interactive(),
            package = "integratecpp"
        )
    }
}
