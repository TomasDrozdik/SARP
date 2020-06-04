library('tidyverse')

linear <- read_csv('data/linear100.csv')
linear$type <- 'linear100'
square <- read_csv('data/square10x10.csv')
square$type <- 'square10x10'
cube <- read_csv('data/cube5x5x4.csv')
cube$type <- 'cube5x5x4'
big_cube <- read_csv('data/cube10x10x10.csv')
big_cube$type <- 'cube10x10x10'

d <- rbind(linear, square)
d <- rbind(d, cube)
d <- rbind(d, big_cube)

d$entries_per_neighbor <- d$routing_table_entries / d$node_count

ggplot(data=d, aes(x=compact_treshold, y=routing_periods, colour=type)) +
	geom_point() +
	scale_colour_brewer(palette='Set1') +
	xlab("COMPACT_THRESHOLD") +
	ylab("update periods") +
	guides(colour=guide_legend(title="")) +
	theme_bw() +
	theme(legend.position='top')
ggsave(filename='grid_comparison_update.pdf', units='cm', width=12, height=8)

ggplot(data=d, aes(x=compact_treshold, y=delivered_packets, colour=type)) +
	geom_point() +
	scale_colour_brewer(palette='Set1') +
	facet_wrap(type~.) +
	xlab("COMPACT_THRESHOLD") +
	ylab("delivered packets") +
	guides(colour=guide_legend(title="")) +
	theme_bw() +
	theme(text=element_text(size=8), legend.position='none')
ggsave(filename='grid_comparison_delivery.pdf', units='cm', width=12, height=8)

ggplot(data=d, aes(x=compact_treshold, y=entries_per_neighbor, colour=type)) +
	geom_point() +
	scale_colour_brewer(palette='Set1') +
	xlab("COMPACT_THRESHOLD") +
	ylab("routing entries per node") +
	guides(colour=guide_legend(title="")) +
	theme_bw() +
	theme(legend.position='top')
ggsave(filename='grid_comparison_size.pdf', units='cm', width=12, height=8)

