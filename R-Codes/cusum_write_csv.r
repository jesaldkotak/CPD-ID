library(IDetect)
set.seed(2026)

setwd("C:/Users/Jesal Kotak/OneDrive - Imperial College London/ADIC MSc/Thesis Project/R-Codes")
T <- 2000

signal_mean <- c(rep(4,400), rep(0,800), rep(-3,400), rep(1,400)) #c => combine vectors, rep(x,n) => repeat x for n times
x_mean <- signal_mean + rnorm(T,sd = 1)

ipcr <- cusum_function(x_mean)

#options(digits = 22)
#df_cusum <- data.frame(Index = 1:length(ipcr), R_CUSUM = ipcr)
#write.csv(df_cusum, "r_cusum_results.csv", row.names = FALSE)

df_cusum <- data.frame(
  Index = 1:length(ipcr),
  R_CUSUM = format(ipcr, digits = 15, scientific = FALSE)
)

# Write to CSV
write.csv(df_cusum, "r_cusum_results.csv", row.names = FALSE, quote = FALSE)

#write.csv(ipcr,"r_cusum_results.csv")