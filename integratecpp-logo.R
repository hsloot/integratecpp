library(tidyverse)
library(hexSticker)

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
            colour = "black",
            fill = "azure4",
            alpha = 0.4
        ) +
        theme_classic() +
        theme_transparent() +
        theme(
            axis.title = element_blank(),
            axis.text = element_blank(),
            axis.ticks = element_line(size = 0.25),
            axis.line = element_line(size = 0.25)
        )

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
    filename = "man/figures/logo.png"
)
