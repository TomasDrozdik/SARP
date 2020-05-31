library('tidyverse')

args = commandArgs(trailingOnly=TRUE)

linear <- read_csv('data/linear100.csv')
square <- read_csv('data/square10x10.csv')
cube <- read_csv('data/cube5x5x4.csv')

jitter <- position_jitter(width = 0, height = 0)

update <- ggplot() +
	geom_point(data = linear, position = jitter, aes(x = compact_treshold, y = routing_periods, colour = 'linear')) +
	geom_point(data = square, position = jitter, aes(x = compact_treshold, y = routing_periods, colour = 'square')) +
	geom_point(data = cube, position = jitter, aes(x = compact_treshold, y = routing_periods, colour = 'cube')) +
	xlab("COMPACT_THRESHOLD") +
	ylab("update periods") +
	scale_colour_brewer(palette = 'Set1') +
	theme_bw() +
	guides(colour=guide_legend(title="")) +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))

ggsave(filename = 'grid_comparison_update.pdf', plot=update)

linear_filter_lower <- filter(linear, routing_periods / (routing_update_period / 1000) < (traffic_time_min / 1000))
square_filter_lower <- filter(square, routing_periods / (routing_update_period / 1000) < (traffic_time_min / 1000))
cube_filter_lower <- filter(cube, routing_periods / (routing_update_period / 1000) < (traffic_time_min / 1000))

delivery_converged <- ggplot() +
	geom_point(data = linear_filter_lower, position = jitter, aes(x = compact_treshold, y = delivered_packets, colour = 'linear')) +
	geom_point(data = square_filter_lower, position = jitter, aes(x = compact_treshold, y = delivered_packets, colour = 'square')) +
	geom_point(data = cube_filter_lower, position = jitter, aes(x = compact_treshold, y = delivered_packets, colour = 'cube')) +
	xlab("COMPACT_THRESHOLD") +
	ylab("delivered packets") +
	theme_bw() +
	guides(colour=guide_legend(title="")) +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))

ggsave(filename = 'grid_comparison_delivery_converged.pdf', plot=delivery_converged)

linear_filter_upper <- filter(linear, routing_periods / (routing_update_period / 1000) > (traffic_time_max / 1000))
square_filter_upper <- filter(square, routing_periods / (routing_update_period / 1000) > (traffic_time_max / 1000))
cube_filter_upper <- filter(cube, routing_periods / (routing_update_period / 1000) > (traffic_time_max / 1000))

delivery_not_converged <- ggplot() +
	geom_point(data = linear_filter_upper, position = jitter, aes(x = compact_treshold, y = delivered_packets, colour = 'linear')) +
	geom_point(data = square_filter_upper, position = jitter, aes(x = compact_treshold, y = delivered_packets, colour = 'square')) +
	geom_point(data = cube_filter_upper, position = jitter, aes(x = compact_treshold, y = delivered_packets, colour = 'cube')) +
	xlab("COMPACT_THRESHOLD") +
	ylab("delivered packets") +
	theme_bw() +
	guides(colour=guide_legend(title="")) +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))

ggsave(filename = 'grid_comparison_delivery_non_converged.pdf', plot=delivery_not_converged)

size <- ggplot() +
	geom_point(data = linear, position = jitter, aes(x = compact_treshold, y = routing_table_entries, colour = 'linear')) +
	geom_point(data = square, position = jitter, aes(x = compact_treshold, y = routing_table_entries, colour = 'square')) +
	geom_point(data = cube, position = jitter, aes(x = compact_treshold, y = routing_table_entries, colour = 'cube')) +
	xlab("COMPACT_THRESHOLD") +
	ylab("routing entries") +
	theme_bw() +
	guides(colour=guide_legend(title="")) +
	theme(text = element_text(size = 12), legend.text=element_text(size=12))

ggsave(filename = 'grid_comparison_size.pdf', plot=size)

