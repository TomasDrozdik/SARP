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
	theme(text=element_text(size=8),
	      panel.background = element_blank(),
     	  plot.background = element_blank())

ggsave(filename='presentation_readdress_square5x5_update.pdf', bg='transparent', units='cm', width=9.5, height=6.5)

ggplot(square, aes(x=added_nodes, y=reach)) +
	geom_boxplot() +
	xlab("added nodes") +
	ylab("delivered packets") +
	theme_bw() +
	theme(text=element_text(size=8),
	      panel.background = element_blank(),
     	  plot.background = element_blank())

ggsave(filename='presentation_readdress_square5x5_delivery.pdf', bg='transparent', units='cm', width=9.5, height=6.5)


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
	theme(text=element_text(size=8),
	      panel.background = element_blank(),
     	  plot.background = element_blank())

ggsave(filename='presentation_readdress_cube4x4x4_update.pdf', bg='transparent', units='cm', width=9.5, height=6.5)

ggplot(cube, aes(x=added_nodes, y=reach)) +
	geom_boxplot() +
	xlab("added nodes") +
	ylab("delivered packets") +
	theme_bw() +
	theme(text=element_text(size=8),
	      panel.background = element_blank(),
     	  plot.background = element_blank())

ggsave(filename='presentation_readdress_cube4x4x4_delivery.pdf', bg='transparent', units='cm', width=9.5, height=6.5)

