library('tidyverse')

continuous <- read_csv('data/ztest_sd_hop.csv')
tresholds <- read_csv('data/ztest_sd_hop_tresholds.csv')
p <- ggplot(continuous, aes(x=sd, y=hop)) +
	geom_point() +
	geom_point(data = tresholds, color='red') +
	xlim(0.01, 0.2) +
	ylim(1, 37) +
	xlab("sd") +
	ylab("distance [hops]")

ggsave(filename = 'ztest_sd_hop_neihgbors.png', plot=p)
