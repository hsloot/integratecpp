# Copyright (c) 2020 usethis authors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## see https://github.com/r-lib/usethis/blob/252d8bef5b5a55b2d4ad44e220c6fedd80cd973b/tests/testthat/helper.R#L6-L68 # nolint
## modifications to original work: add explicit namespaces to functions which would be
## in the namespace of `usethis` but are not in the namespace of this project
session_temp_proj <- usethis:::proj_find(fs::path_temp())
if (!is.null(session_temp_proj)) {
  Rproj_files <- fs::dir_ls(session_temp_proj, glob = "*.Rproj") # nolint
  usethis::ui_line(c(
    "Rproj file(s) found at or above session temp dir:",
    paste0("* ", Rproj_files),
    "Expect this to cause spurious test failures."
  ))
}

create_local_package <- function(dir = fs::file_temp(pattern = "testpkg"),
                                 env = parent.frame(),
                                 rstudio = FALSE) {
  create_local_thing(dir, env, rstudio, "package")
}

create_local_project <- function(dir = fs::file_temp(pattern = "testproj"),
                                 env = parent.frame(),
                                 rstudio = FALSE) {
  create_local_thing(dir, env, rstudio, "project")
}

create_local_thing <- function(dir = fs::file_temp(pattern = pattern),
                               env = parent.frame(),
                               rstudio = FALSE,
                               thing = c("package", "project")) {
  thing <- match.arg(thing)
  if (fs::dir_exists(dir)) {
    usethis::ui_stop("Target {usethis::ui_code('dir')} {usethis::ui_path(dir)} already exists.")
  }

  old_project <- usethis:::proj_get_() # this could be `NULL`, i.e. no active project
  old_wd <- getwd()          # not necessarily same as `old_project`

  withr::defer(
    {
      usethis::ui_done("Deleting temporary project: {usethis::ui_path(dir)}")
      fs::dir_delete(dir)
    },
    envir = env
  )
  usethis::ui_silence(
    switch(
      thing,
      package = usethis::create_package(dir, rstudio = rstudio, open = FALSE, check_name = FALSE),
      project = usethis::create_project(dir, rstudio = rstudio, open = FALSE)
    )
  )

  withr::defer(usethis::proj_set(old_project, force = TRUE), envir = env)
  usethis::proj_set(dir)

  withr::defer(
    {
      usethis::ui_done("Restoring original working directory: {usethis::ui_path(old_wd)}")
      setwd(old_wd)
    },
    envir = env
  )
  setwd(usethis::proj_get())

  invisible(usethis::proj_get())
}
