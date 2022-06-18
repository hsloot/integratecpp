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

## start: load testutils.integratecpp
lib_path_restore <- .libPaths()
local_lib <- tempfile("RLIB_")
dir.create(local_lib)
.libPaths(c(local_lib, .libPaths()))
if (requireNamespace("cli", quietly = TRUE)) {
    cli::cli_alert_info("Temporarily installing {.pkg testutils.integratecpp}")
} else {
    message("Temporarily installing `testutils.integratecpp`")
}
remotes::install_local("testutils.integratecpp", type = "source", quiet = TRUE,
    dependencies = FALSE, build = FALSE, upgrade = FALSE)
if (!"testutils.integratecpp" %in% installed.packages()[, "Package"]) {
    if (requireNamespace("cli", quietly = TRUE)) {
        cli::cli_alert_warning("Installation of {.pkg testutils.integratecpp} failed on {.Platform$OS.type}, trying again with argument {.code upgrade = TRUE}") # nolint
    } else {
        message(sprintf(
            "Installation of `testutils.integratecpp` failed on %s, trying again with argument `upgrade=TRUE`", # nolint
            .Platform$OS.type
        ))
    }
    ## TODO: This is required due to some weird bug in rcmdcheck or remotes;
    ## find a better solution
    remotes::install_local("testutils.integratecpp", type = "source", repos = NULL,
        quiet = TRUE, dependencies = TRUE, build = FALSE,
        upgrade = TRUE)
}

withr::defer({
    .libPaths(lib_path_restore)
    try({
        detach("testutils.integratecpp", unload = TRUE)
    }, silent = TRUE)
    package_dir <- file.path(local_lib, "testutils.integratecpp")
    if (dir.exists(package_dir)) unlink(package_dir)
    },
    teardown_env()
)
## end: load testutils.integratecpp
