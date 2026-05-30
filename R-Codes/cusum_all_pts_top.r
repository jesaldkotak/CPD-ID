library(IDetect)
setwd("C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/R-Codes")
source("pcm_th_wcomments.r")

t <- 2000
set.seed(2026)

current_file <- "cusum_val_2026_p10_t11_0cp_30.csv"

header <- data.frame(Direction=character(), Exp_Interval=integer(), Start_Index=integer(), End_Index=integer(), Candidate_Index=integer(), Max_CUSUM=numeric(), Order=integer())
write.table(header, file = current_file, sep=",", col.names=TRUE, row.names=FALSE, quote=FALSE)

signal_mean <- c(rep(5,2000))
x <- list()
for (i in 1:100) {
  x[[i]] <- signal_mean + rnorm(t,0,1)
}
cp_order <- 0
cpt_mean <- pcm_th_wcomments(x[[30]], thr_const = 1.1, points=10, log_file = current_file)

#IDetect:::cusum_one(x[[3]],271,395,282)
