
setwd("C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/R-Codes")
source("cumsum_lin.r")
source("cplm_th_wcomments.r")

t <- 2000
set.seed(2026)

current_file <- "cumsum_val_2026_p10_t14_35.csv"

header <- data.frame(Direction=character(), Exp_Interval=integer(), Start_Index=integer(), End_Index=integer(), Candidate_Index=integer(), Max_CUSUM=numeric(), Order=integer())
write.table(header, file = current_file, sep=",", col.names=TRUE, row.names=FALSE, quote=FALSE)

signal_lin <- c( (0.2)*(1:500), (-0.2)*(501:1000)+200, (0.2)*(1001:1500)-200, (-0.2)*(1501:2000)+400)

x <- list()
for (i in 1:100) {
  x[[i]] <- signal_lin + rnorm(t,0,5)
}

cp_order <- 0
cpt_mean <- cplm_th_wcomments(x[[35]], thr_const = 1.4, points=10, log_file = current_file)
