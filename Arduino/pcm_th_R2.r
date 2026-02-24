function (x, sigma = stats::mad(diff(x)/sqrt(2)), thr_const = 1, 
    thr_fin = sigma * thr_const * sqrt(2 * log(length(x))), s = 1, 
    e = length(x), points = 3, k_l = 1, k_r = 1) 
{
    if (!(is.numeric(x))) {
        stop("The input in `x' should be a numeric vector containing the data in\n           which you would like to find change-points.")
    }
    if ((thr_const <= 0) || (points <= 0)) {
        stop("The threshold constant as well as the `points' argument that represents the\n         magnitude of the expansion for the intervals should be positive numbers.")
    }
    if (abs(points - round(points)) > .Machine$double.eps^0.5) {
        warning("The input for `points' should be a positive integer. If it is a positive real\n            number then the integer part of the given number is used as the value of `points'.")
    }
    points <- as.integer(points)
    l <- length(x)
    r_e_points <- seq(points, l, points)
    l_e_points <- seq(l - points + 1, 1, -points)
    chp <- 0
    if (e - s <= 1) {
        cpt <- 0
    }
    else {
        pos_r <- numeric()
        CUSUM_r <- numeric()
        pos_l <- numeric()
        CUSUM_l <- numeric()
        moving_points <- s_e_points(r_e_points, l_e_points, s, 
            e)
        right_points <- moving_points[[1]]
        left_points <- moving_points[[2]]
        lur <- length(left_points)
        rur <- length(right_points)
        if (k_r < k_l) {
            while ((chp == 0) & (k_r < min(k_l, rur))) {
                x_temp_r <- x[s:right_points[k_r]]
                ipcr <- cusum_function(x_temp_r)
                pos_r[k_r] <- which.max(abs(ipcr)) + s - 1
                CUSUM_r[k_r] <- abs(ipcr[pos_r[k_r] - s + 1])
                if (CUSUM_r[k_r] > thr_fin) {
                  chp <- pos_r[k_r]
                }
                else {
                  k_r <- k_r + 1
                }
            }
        }
        if (k_l < k_r) {
            while ((chp == 0) & (k_l < min(k_r, lur))) {
                x_temp_l <- x[left_points[k_l]:e]
                ipcl <- cusum_function(x_temp_l)
                pos_l[k_l] <- which.max(abs(ipcl)) + left_points[k_l] - 
                  1
                CUSUM_l[k_l] <- abs(ipcl[pos_l[k_l] - left_points[k_l] + 
                  1])
                if (CUSUM_l[k_l] > thr_fin) {
                  chp <- pos_l[k_l]
                }
                else {
                  k_l <- k_l + 1
                }
            }
        }
        if (chp == 0) {
            while ((chp == 0) & (k_l <= lur) & (k_r <= rur)) {
                x_temp_r <- x[s:right_points[k_r]]
                ipcr <- cusum_function(x_temp_r)
                pos_r[k_r] <- which.max(abs(ipcr)) + s - 1
                CUSUM_r[k_r] <- abs(ipcr[pos_r[k_r] - s + 1])
                if (CUSUM_r[k_r] > thr_fin) {
                  chp <- pos_r[k_r]
                }
                else {
                  x_temp_l <- x[left_points[k_l]:e]
                  ipcl <- cusum_function(x_temp_l)
                  pos_l[k_l] <- which.max(abs(ipcl)) + left_points[k_l] - 
                    1
                  CUSUM_l[k_l] <- abs(ipcl[pos_l[k_l] - left_points[k_l] + 
                    1])
                  if (CUSUM_l[k_l] > thr_fin) {
                    chp <- pos_l[k_l]
                  }
                  else {
                    k_r <- k_r + 1
                    k_l <- k_l + 1
                  }
                }
            }
        }
        if (chp != 0) {
            if (chp > ((e + s)/2)) {
                r <- pcm_th(x, s = s, e = chp, points = points, 
                  thr_fin = thr_fin, k_r = k_r, k_l = 1)
            }
            else {
                r <- pcm_th(x, s = chp + 1, e = e, points = points, 
                  thr_fin = thr_fin, k_r = 1, k_l = max(1, k_l - 
                    1))
            }
            cpt <- c(chp, r)
        }
        else {
            cpt <- chp
        }
    }
    cpt <- cpt[cpt != 0]
    return(sort(cpt))
}