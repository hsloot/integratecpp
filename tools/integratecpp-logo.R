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

# cSpell: words ggplot,tibble,pkgdown,linewidth,darkseagreen

#' ---
#' title: Create logo and favicons for `integratecpp`
#' author: Henrik Sloot <henrik.sloot@gmail.com>
#' date: November 11, 2022
#' ---
NULL

#+ r setup
library(here)
library(tibble)
library(usethis)
library(ggplot2)
library(pkgdown)
library(hexSticker)


#+ r create-logo
ui_info("Creating logo for package {ui_code('integratecpp')}")

fn <- function(x) {
  x^-0.7
}

x <- seq(0, 1, length.out = 100)
x <- x[x > 0]
y <- fn(x)

df <- tibble(x = x, y = y)

p <- df |>
  ggplot(aes(x = x, y = y)) +
  geom_area(
    color = "black",
    fill = "azure4",
    alpha = 0.4
  ) +
  theme_classic() +
  theme_transparent() +
  theme(
    axis.title = element_blank(),
    axis.text = element_blank(),
    axis.ticks = element_line(linewidth = 0.25),
    axis.line = element_line(linewidth = 0.25)
  )

logo_path <- here("man", "figures", "logo.png")
sticker(
  p,
  package = "integratecpp",
  p_size = 20,
  s_x = 1,
  s_y = 0.8,
  s_width = 1.3,
  s_height = 0.7,
  h_fill = "darkseagreen3",
  h_color = "darkseagreen4",
  filename = logo_path
)

ui_done("Logo saved to {ui_path(logo_path)}")


#+ r build-favicons
ui_info("Building favicons for package {ui_code('integratecpp')}")

favicons_path <- here("pkgdown", "favicon")
build_favicons(here(), overwrite = TRUE)
ui_done("Favicons build in {ui_path(favicons_path)}")
