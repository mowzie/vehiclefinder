**4-30-15**

  starting analyzing frequency sample size based on "yelp"
  toyed with overlap, this seems to help in detecting frequencies at the cost of additional CPU power
  FFT's generally have some level of overlap in the window.
  Will test different sample sizes with different size overlap (most likely in the range of: 1/4, 1/8, 1/16)

**Addendum**

  The test files found vary wildly in frequency range - not sure if this is because they were moving or because the recordings are crap.
  Either way - by utalizing overlap and simple Hann windowing, I can find max/min frequencies for the different police sirens.
  will continue characterizing other sirens tomorrow (ambulence, fire..)


**5-3/4-15**

randomly set the sample size to 140  
set overlap to be 1/2 the sample size (so we analyze the previous 70 samples with the next 70 samples)  
Fixed a major bug in the overlap: previous implementation did not overlap, but kept advancing.  
This was due to reading in the sample from disk instead of from array.  
Fixed by reading in the channel data into an array and passing the array into the process code  
Results are much better now!  
