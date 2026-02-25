#pcm_th
#A single object matching ‘pcm_th’ was found
#It was found in the following places
#  package:IDetect
#  namespace:IDetect
#with value

function (x, sigma = stats::mad(diff(x)/sqrt(2)), thr_const = 1, thr_fin = sigma * thr_const * sqrt(2 * log(length(x))), s = 1, 
e = length(x), points = 3, k_l = 1, k_r = 1) 
{   #s:these conditions need not be checked in every iteration
    if (!(is.numeric(x))) {
        stop("The input in `x' should be a numeric vector containing the data in\n           which you would like to find change-points.")
    }
    if ((thr_const <= 0) || (points <= 0)) {
        stop("The threshold constant as well as the `points' argument that represents the\n         magnitude of the expansion for the intervals should be positive numbers.")
    }
    if (abs(points - round(points)) > .Machine$double.eps^0.5) {
        warning("The input for `points' should be a positive integer. If it is a positive real\n            number then the integer part of the given number is used as the value of `points'.")
    }
    #s:no need to calculate these again
    points <- as.integer(points)
    l <- length(x)
    r_e_points <- seq(points, l, points)                          #all right end points
    l_e_points <- seq(l - points + 1, 1, -points)                 #all left start points
    #s:but chp should be made 0 after a CP is detected
    chp <- 0
    #s:the problem with this is that in the end if e-s = 1, the entire cpt = 0.
    if (e - s <= 1) {
        cpt <- 0
    }
    else {
        pos_r <- numeric()
        CUSUM_r <- numeric()
        pos_l <- numeric()
        CUSUM_l <- numeric()
        moving_points <- s_e_points(r_e_points, l_e_points, s, e)   #list of [all right end points, all right end points]
        right_points <- moving_points[[1]]                          #all right end points, sorted and made integers
        left_points <- moving_points[[2]]                           #all left start pts, sorted and made int
        lur <- length(left_points)
        rur <- length(right_points)
        #following two if loops make sure that after one CP is detected, the same intervals are not repeated
        if (k_r < k_l) {                                        #a CP was detected in left half of plane, so start from right exp interval until size of right int = left int
            while ((chp == 0) & (k_r < min(k_l, rur))) {        #no cp detected AND k_r < k_l, 
                x_temp_r <- x[s:right_points[k_r]]              #creating a vector of points for the interval under test
                ipcr <- cusum_function(x_temp_r)                #calculating cusum for a vector and storing in a vector
                pos_r[k_r] <- which.max(abs(ipcr)) + s - 1      #argument of maxima of cusum stat in the interval   #what is there's no max?- unlikely due to noise
                CUSUM_r[k_r] <- abs(ipcr[pos_r[k_r] - s + 1])   #absolute value of cusum stat at the maxima
                if (CUSUM_r[k_r] > thr_fin) {                   #comparing with threshold
                  chp <- pos_r[k_r]                             #assigning the arg with max cusum value only if it crosses threshold
                }
                else {
                  k_r <- k_r + 1                                #until k_l = k_r
                }
            }
        }
        if (k_l < k_r) {                                        #a CP was detected in right half of plane, so start from left exp interval until size of right int = left int
            while ((chp == 0) & (k_l < min(k_r, lur))) {        #similar to above
                x_temp_l <- x[left_points[k_l]:e]               #IUT
                ipcl <- cusum_function(x_temp_l)                #cusum calculation
                pos_l[k_l] <- which.max(abs(ipcl)) + left_points[k_l] - 1
                CUSUM_l[k_l] <- abs(ipcl[pos_l[k_l] - left_points[k_l] + 1])
                if (CUSUM_l[k_l] > thr_fin) {
                  chp <- pos_l[k_l]
                }
                else {
                  k_l <- k_l + 1                                #until k_l = k_r
                }
            }
        }
        if (chp == 0) {                                         #First the code comes here because k_l = k_r and no CPs detected yet
            while ((chp == 0) & (k_l <= lur) & (k_r <= rur)) {  #initally and when k_l = k_r, No CP detected AND not finished running cusum on all the left and right intervals
                x_temp_r <- x[s:right_points[k_r]]              #start with right interval
                ipcr <- cusum_function(x_temp_r)                #cusum calc
                pos_r[k_r] <- which.max(abs(ipcr)) + s - 1      #arg max
                CUSUM_r[k_r] <- abs(ipcr[pos_r[k_r] - s + 1])   #absoulte
                if (CUSUM_r[k_r] > thr_fin) {                   #compare to threshold
                  chp <- pos_r[k_r]                             #add as a CP
                }
                else {                                          #if given point has value less than thresh in the right interval
                  x_temp_l <- x[left_points[k_l]:e]             #if no CP, go to left interval
                  ipcl <- cusum_function(x_temp_l)              #cusum cal
                  pos_l[k_l] <- which.max(abs(ipcl)) + left_points[k_l] - 1
                  CUSUM_l[k_l] <- abs(ipcl[pos_l[k_l] - left_points[k_l] + 1])
                  if (CUSUM_l[k_l] > thr_fin) {                 #if greater than thresh in left interval
                    chp <- pos_l[k_l]
                  }
                  else {                                        #increment both until a CP is found, or we tested all the left/right intervals
                    k_r <- k_r + 1                              #If so, we won't enter the above two loops without user compulsion
                    k_l <- k_l + 1
                  }
                }
            }
        }
        if (chp != 0) {                                             #If a CP is detected
            if (chp > ((e + s)/2)) {                                #if its in the right half plane
                r <- pcm_th(x, s = s, e = chp, points = points, thr_fin = thr_fin, k_r = k_r, k_l = 1)  #running the next iteration in [s,chp], will start from left interval this time
            }
            else {                                                  #if CP is in left half plane
                r <- pcm_th(x, s = chp + 1, e = e, points = points, thr_fin = thr_fin, k_r = 1, k_l = max(1, k_l - 1)) #running the next iteration in [chp+1,e], will start from right interval
            }
            cpt <- c(chp, r)                                        #after a CP is detected, it is added to the cpt vector
        }
        else {                                                      #s:is this else really required? we are just assigning 0
            cpt <- chp
        }
    }
    cpt <- cpt[cpt != 0]                                            #removes all the zeros
    return(sort(cpt))
}