#cusum function
cusum_function <- function(x) {

  if (!(is.numeric(x))){

    stop("The input in `x' should be a numeric vector containing the data

         for which the CUSUM function will be calculated.")

  }

  n <- length(x)

  y <- cumsum(x)  #This is a vector of sum of individual elements, like a fibonacci seq

  res <- sqrt( ( (n - 1):1) / n / (1:(n - 1))) * y[1:(n - 1)] - sqrt( (1:(n - 1)) / n / ( (n - 1):1)) * (y[n] - y[1:(n - 1)]) #vector with cusum calculation for each element in x (for given interval)

  return(res)

}