#s_e_points
function (r, l, s, e) 
{
    r <- sort(r)                        #arranging all the right interval end points in ascending order
    l <- sort(l, decreasing = TRUE)     #arranging all the left interval start points in descending order

    #just some checks on inputs...
    if (s > e) {
        stop("s should be less than or equal to e")
    }
    if (!(is.numeric(c(r, l, s, e))) | (r[1] <= 0) | (l[length(l)] <= 
        0) | s <= 0 | e <= 0) {
        stop("The input arguments must be positive integers")
    }
    if (any(abs(r - round(r)) > .Machine$double.eps^0.5)) {
        warning("The input for r should be a vector of positive integers. If there is at least a positive real\n            number then the integer part of that number is used.")
    }
    if (any(abs(l - round(l)) > .Machine$double.eps^0.5)) {
        warning("The input for l should be a vector of positive integers. If there is at least a positive real\n            number then the integer part of that number is used.")
    }
    if (abs(s - round(s)) > .Machine$double.eps^0.5) {
        warning("The input for s should be a positive integer. If it is a positive real\n            number then the integer part of that number is used.")
    }
    if (abs(e - round(e)) > .Machine$double.eps^0.5) {
        warning("The input for e should be a positive integer. If it is a positive real\n            number then the integer part of that number is used.")
    }

    #actual logic:
    #just truncating the right interval end points, left interval start points, global end and start pts as integers.
    r <- as.integer(r)
    l <- as.integer(l)
    e <- as.integer(e)
    s <- as.integer(s)
    #unique removes any duplicate elements in the vector
    e_points <- unique(c(r[which((r > s) & (r < e))], e))   #list of unique end points of right expanding intervals
    s_points <- unique(c(l[which((l > s) & (l < e))], s))   #list of unique start points of left expanding intervals
    return(list(e_points = e_points, s_points = s_points))
}