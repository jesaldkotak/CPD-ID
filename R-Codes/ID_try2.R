library(IDetect)

set.seed(2026)
setwd("C:/Users/Jesal Kotak/OneDrive - Imperial College London/ADIC MSc/Thesis Project/R-Codes")

T <- 2000.

############################# Piecewise constant mean signal ##############################

signal_mean <- c(rep(4,400), rep(0,800), rep(-3,400), rep(1,400)) #c => combine vectors, rep(x,n) => repeat x for n times
x_mean <- signal_mean + rnorm(T,sd = 1)

write.csv(x_mean,"x_2026.csv")

cpt_mean <- pcm_th(x_mean)
fit_mean <- est_signal(x_mean, cpt_mean, type = "mean")

cat("pcm_th")
print(cpt_mean)

plot(x_mean, type='l', main = "pcm_th", xlab="t",ylab="x")
lines(fit_mean, lwd = 2, col = "red")
abline(v=cpt_mean, lty = 2)

dev.copy(png, filename="IDetect_Analysis.png", width=800, height=600) # To view + save the plot
dev.off()

############################# Piecewise linear signal ##############################

# #signal_linear <- c(seq(from=1, to=600, by = 2), seq(from=599, to=0, by = -1), seq(from=1, to=1100, by = 1))
# signal_linear <- c(seq(from=1, to=100, by = 0.5), seq(from=99, to=0, by = -1), seq(from=1, to=100, by = 0.0625))
# print(length(signal_linear))
# x_linear <- signal_linear + rnorm(length(signal_linear),sd = 1)
# 
# cpt_linear <- cplm_th(x_linear)
# fit_linear <- est_signal(x_linear, cpt_linear, type = "slope")
# 
# cat("cplm_th")
# print(cpt_linear)
# 
# 
# plot(signal_linear, type='l', main = "cplm_th", xlab="t",ylab="x")
# lines(fit_linear, lwd = 1, lty = 3, col = "red")
# abline(v=cpt_linear, lty = 2)
# 
# dev.copy(png, filename="ID_linear_2.png", width=800, height=600) # To view + save the plot
# dev.off()
