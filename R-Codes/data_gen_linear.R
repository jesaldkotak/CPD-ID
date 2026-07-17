library(IDetect)
setwd("C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/R-Codes")

t <- 2000

set.seed(2026)

#signal_lin <- c( (0.2)*(1:500), (-0.2)*(501:1000)+200, (0.2)*(1001:1500)-200, (-0.2)*(1501:2000)+400)
#signal_lin <- c( (0.2)*(1:200), (-0.2)*(201:400)+80, (0.2)*(401:600)-80, (-0.2)*(601:800)+160, (0.2)*(801:1000)-160, (-0.2)*(1001:1200)+240, (0.2)*(1201:1400)-240, (-0.2)*(1401:1600)+320, (0.2)*(1601:1800)-320, (-0.2)*(1801:2000)+400)
#signal_lin <- (0.05)*(1:2000)
#signal_lin <- c((0.2)*(1:200), (0.4)*(201:400)-40, (0.0)*(401:600)+120, (0.1)*(601:800)+60, (-0.2)*(801:1000)+300, (0)*(1001:1200)+100, (0.2)*(1201:1400)-140, (-0.4)*(1401:1600)+700, (-0.2)*(1601:2000)+380)
signal_lin <- c(0.1*(1:10)+1, (0.2)*(11:200), (0.4)*(201:400)-40, 0.1*(401:416)+80, -0.1*(417:430)+163 , (0.0)*(431:600)+120, (0.1)*(601:800)+60, (-0.2)*(801:1000)+300, (0)*(1001:1200)+100, (0.2)*(1201:1400)-140, (-0.4)*(1401:1600)+700, (-0.2)*(1601:2000)+380)

x <- list()
for (i in 1:100) {
  x[[i]] <- signal_lin + rnorm(t,0,5)
}

plot(x[[100]], type='l', main = "R cplm_th", xlab="t",ylab="x")

df <- as.data.frame(x)
colnames(df) <- paste0("x", 1:100)

write.csv(df, "x_lin_2026_ds5.csv")

cpt_lin <- list()

for (i in 1:100) {
  cpt_lin[[i]] <- cplm_th(x[[i]], thr_const = 1.4, points=10)
}

max_len <- max(sapply(cpt_lin, length))

cpt_lin_pad <- lapply(cpt_lin, function(v) {
  length(v) <- max_len
  return(v)
})

df1 <- as.data.frame(cpt_lin_pad)
colnames(df1) <- paste0("cpt_lin", 1:100)

write.csv(df1, "r_lin_cpt_ds5_p10.csv", row.names = FALSE)
