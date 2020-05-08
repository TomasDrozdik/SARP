library('tidyverse')

args = commandArgs(trailingOnly=TRUE)

if (length(args) != 2) {
	error("Error: Rscript plot.R [input_filename.csv] [output_filename]")
}

input = args[1]
output = args[2]

data <- read_csv(input)
p <- ggplot(data, aes(x=sd, y=hop)) +
	geom_line() +
	xlab("sd") +
	ylab("distance [hops]")

ggsave(filename = output, plot=p)
