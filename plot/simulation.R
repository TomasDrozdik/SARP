library('tidyverse')

args = commandArgs(trailingOnly=TRUE)

if (length(args) != 2) {
	error("Error: Rscript simulation.R [input_filename.csv] [output_filename]")
}

input = args[1]
output = args[2]

d <- read_csv(input)
ggplot(d, aes(x = compact_treshold, y = routing_periods)) +
	geom_point() +
	xlab("COMPACT_THRESHOLD") +
	ylab("update periods") +
	theme_bw() +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))
ggsave(filename = paste0(output, '_update.pdf'))

ggplot(d, aes(x = compact_treshold, y = delivered_packets)) +
	geom_point() +
	xlab("COMPACT_THRESHOLD") +
	ylab("delivered packets") +
	theme_bw() +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))
ggsave(filename = paste0(output, '_delivery.pdf'))

ggplot(d, aes(x = compact_treshold, y = routing_table_entries)) +
	geom_point() +
	xlab("COMPACT_THRESHOLD") +
	ylab("routing entries") +
	theme_bw() +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))
ggsave(filename = paste0(output, '_size.pdf'))

