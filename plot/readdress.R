library('tidyverse')

# Square 5x5 graphs

square <- read_csv('data/readdress_square5x5.csv')

# Compute reach as a delivered packets / added nodes
square$reach <- square$delivered_packets / square$added_nodes
square$added_nodes <- as.factor(square$added_nodes)

ggplot(square, aes(x=added_nodes, y=routing_periods)) +
	geom_boxplot() +
	xlab("added nodes") +
	ylab("update periods") +
	theme_bw() +
ggsave(filename='readdress_square5x5_update.pdf', units='cm', width=12, height=8)

ggplot(square, aes(x=added_nodes, y=reach)) +
	geom_boxplot() +
	xlab("added nodes") +
	ylab("delivered packets") +
	theme_bw() +
ggsave(filename='readdress_square5x5_delivery.pdf', units='cm', width=12, height=8)


# Cube 4x4x4 graphs

cube <- read_csv('data/readdress_cube4x4x4.csv')

# Compute reach as a delivered packets / added nodes
cube$reach <- cube$delivered_packets / cube$added_nodes
cube$added_nodes <- as.factor(cube$added_nodes)

ggplot(cube, aes(x=added_nodes, y=routing_periods)) +
	geom_boxplot() +
	xlab("added nodes") +
	ylab("update periods") +
	theme_bw() +
ggsave(filename='readdress_cube4x4x4_update.pdf', units='cm', width=12, height=8)

ggplot(cube, aes(x=added_nodes, y=reach)) +
	geom_boxplot() +
	xlab("added nodes") +
	ylab("delivered packets") +
	theme_bw() +
ggsave(filename='readdress_cube4x4x4_delivery.pdf', units='cm', width=12, height=8)

