# CIS700FANFU-Simulation

## Part 1 - Implement Fast Poisson Disk Sampling
### Overview: 

I am using the Bridson's paper on [Poisson Disk Sampling Method for Arbitrary Dimensions](./Part1Info/PoissonDiskPaper.pdf) to make a point cloud representation of a given mesh in both two and three dimensions. Previous sampling techniques include Cook's 1986 paper that discusses an implementation for Stochastic Sampling [a Monte Carlo technique] and Dunbar and Humphrey’s paper in 2006 that describes a dated implementation of an implementation for the sampling technique. 


The technique in Cook’s paper describes a technique used to take the place of Poisson Disk Sampling due memory costs of the randomness of a look-up table for the original sampling technique. The Stochastic Sampling technique aimed to improve upon the issues of aliasing by allowing for noise instead; however, the paper focused on using the jitter of a regular grid to implement the sampling. Like actual Poisson Disk Sampling, jittering is well suited for image-rendering algorithms and removes some aliasing; however the results are not quite as good as those actually obtained with the original sampling technique; additionally, the images still maintain a bit of aliasing and noise due to only a reduction but not complete elimination of sample clustering.


Dunbar and Humphrey's paper describes an improved implementation of Poisson sampling. By using an efficient data structure they are able to perform uniform sampling within the r and 2r radii around each sampled location, and by maintaining that each valid sample must contain at least one other sample within the range of those two radii, their algorithm works as intended. However, this technique does not easily generalize for uses in rendering with additional effects such as motion blur and depth of field in higher dimensions [mainly 3D for rendering], therefore, Bridson's algorithm is the preferred implementation for this project.


As Bridson's previously linked paper delineates, his algorithm easily extends to n-dimensions while still running in O(N) time to generate the requested N poisson disk samples [aka running proportional to the number of requested samples]. Additionally, in contrast with Dunbar and Humphrey's approach, once the samples are picked from the appropriate region, Bridson's algorithm uses rejection sampling [when an existing sample no longer has any new/valid samples within its valid sample range it is rejected] instead of using the clear-cut restrictions to compute the allowed sampling region. This is beneficial as it allows the sampling to develop more organically thus reducing the chaotic-ness of the random selection, speeding up the process as a whole.
