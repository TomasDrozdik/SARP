library('tidyverse')

args=commandArgs(trailingOnly=TRUE)

d <- read_csv('data/linear100_update_threshold.csv')
d$entries_per_neighbor <- d$routing_table_entries / d$node_count

lg <- guide_colourbar(barheight=0.5)

ggplot(d, aes(x=compact_treshold, y=routing_periods, color=update_treshold)) +
	geom_point() +
	xlab("COMPACT_THRESHOLD") +
	ylab("update periods") +
	facet_wrap(update_treshold~.) +
	scale_color_continuous(name='UPDATE_THRESHOLD  ', n=3, guide=lg) +
	theme_bw() +
	theme(legend.position='top')
ggsave(filename='linear100_update_threshold_update.pdf', units='cm', width=12, height=8)

ggplot(d, aes(x=compact_treshold, y=delivered_packets, color=update_treshold)) +
	geom_point() +
	xlab("COMPACT_THRESHOLD") +
	ylab("delivered packets") +
	facet_wrap(update_treshold~.) +
	scale_color_continuous(name='UPDATE_THRESHOLD  ', n=3, guide=lg) +
	theme_bw() +
	theme(legend.position='top')
ggsave(filename='linear100_update_threshold_delivery.pdf', units='cm', width=12, height=8)

ggplot(d, aes(x=compact_treshold, y=entries_per_neighbor, color=update_treshold)) +
	geom_point() +
	xlab("COMPACT_THRESHOLD") +
	ylab("routing entries per node") +
	facet_wrap(update_treshold~.) +
	scale_color_continuous(name='UPDATE_THRESHOLD  ', n=3, guide=lg) +
	theme_bw() +
	theme(legend.position='top')
ggsave(filename='linear100_update_threshold_size.pdf', units='cm', width=12, height=8)

