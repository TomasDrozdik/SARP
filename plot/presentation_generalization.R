library('tidyverse')

ggplot(data.frame(x=c(0, 10)), aes(x=x)) +
	stat_function(fun=dnorm, args=list(4.63, 1.94), aes(colour='parent'), size=1) +
	stat_function(fun=dnorm, args=list(5.75, 1.13), aes(colour='child 1'), size=1) +
	stat_function(fun=dnorm, args=list(3.5, 0.56), aes(colour= 'child 2'), size=1) +
	scale_colour_brewer(palette='Set1') +
	scale_x_continuous(name='hop-metrics distance') +
	scale_y_continuous(name='Probability density function') +
	guides(colour=guide_legend(title="")) +
	theme_bw() +
	theme(legend.position="bottom",
		  legend.background=element_blank(),
		  legend.box.background=element_blank(),
	      text=element_text(size=8),
	      panel.background = element_blank(),
     	  plot.background = element_blank())


ggsave(filename='presentation_generalization.pdf', bg='transparent', units='cm', width=9.5, height=6.5)

