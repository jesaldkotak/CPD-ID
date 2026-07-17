cumsum_lin <- function (x) 
{
    if (!(is.numeric(x))) {
        stop("The input in `x' should be a numeric vector.")
    }
    res <- numeric()
    n <- length(x)
    if (n <= 2) {
        res <- 0
    }
    else {
        b <- 2:(n - 1)
        y1 <- cumsum(x * (1:n))
        y <- cumsum(x)
        a <- sqrt(6/((n - 1) * n * (n + 1) * (2 - 2 * b^2 + 2 * b * n - 1 + 2 * b - n)))
        be <- sqrt(((n - b + 1) * (n - b))/((b - 1) * b))
        res[1] <- 0
        res[b] <- a * be * ((2 * b + n - 1) * y1[b] - (n + 1) * b * y[b]) - (a/be) * ((3 * n - 2 * b + 1) * (y1[n] -  y1[b]) - (n + 1) * (2 * n - b) * (y[n] - y[b]))
    }
    return(res)
}