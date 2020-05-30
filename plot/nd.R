library('tidyverse')

p <- ggplot(data.frame(x = c(0, 10)), aes(x = x)) +
	stat_function(fun = dnorm, args = list(4.63, 1.94), aes(colour = 'parent: mean=4.6 variance=1.9'), size = 1) +
	stat_function(fun = dnorm, args = list(5.75, 1.13), aes(colour = 'child:  mean=5.8 variance=1.1'), size = 1) +
	stat_function(fun = dnorm, args = list(3.5, 0.56), aes(colour =  'child:  mean=3.5 variance=0.5'), size = 1) +
	scale_colour_brewer(palette = 'Set1') +
	scale_x_continuous(name = 'x') +
	scale_y_continuous(name = 'Probability density function') +
	guides(colour=guide_legend(title="")) +
	theme_bw() +
	theme(text = element_text(size = 13), legend.position = c(0.7, 0.8), legend.text=element_text(size=13))

ggsave(filename = 'normal_distribution.pdf', plot = p)

