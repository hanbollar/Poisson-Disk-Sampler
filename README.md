# CIS700FANFU-Simulation

## Implement Fast Poisson Disk Sampling
### Overview: 

I am using the Bridson's paper on [Poisson Disk Sampling Method for Arbitrary Dimensions](./Part1Info/PoissonDiskPaper.pdf) to make a point cloud representation of a given mesh in both two and three dimensions. Previous sampling techniques include Cook's 1986 paper that discusses an implementation for Stochastic Sampling [a Monte Carlo technique] and Dunbar and Humphrey’s paper in 2006 that describes a dated implementation for the sampling technique. 


Cook’s paper describes a technique used to take the place of Poisson Disk Sampling due memory costs of the randomness of a look-up table for the original sampling technique. The Stochastic Sampling technique aimed to improve upon the issues of aliasing by allowing for noise instead; however, the paper focused on using the jitter of a regular grid to implement the sampling. Like actual Poisson Disk Sampling, jittering is well suited for image-rendering algorithms and removes some aliasing; however the results are not quite as good as those actually obtained with the original sampling technique. Additionally, the images still maintain a bit of aliasing and noise due to only a reduction but not complete elimination of sample clustering.


Dunbar and Humphrey's paper describes an improved implementation of Poisson sampling. By using an efficient data structure they are able to perform uniform sampling within the r and 2r radii around each sampled location, and by maintaining that each valid sample must contain at least one other sample within the range of those two radii, their algorithm works as intended. However, this technique does not easily generalize for uses in rendering with additional effects such as motion blur and depth of field in higher dimensions [aka in 3D rendering], therefore, Bridson's algorithm is the preferred implementation for this project.


As Bridson's previously linked paper delineates, his algorithm easily extends to n-dimensions while still running in O(N) time to generate the requested N Poisson Disk Samples [aka running proportional to the number of requested samples]. Additionally, in contrast with Dunbar and Humphrey's approach, once the samples are picked from the appropriate region, Bridson's algorithm uses rejection sampling [when an existing valid sample no longer has any other new/valid samples within its valid sample range it is no longer a valid sample] instead of using the clear-cut restrictions to compute the allowed sampling region. This is beneficial as it allows the sampling to develop more organically thus reducing the chaotic-ness of the random selection, speeding up the process as a whole.

### The Poisson Algorithm:

'''
Select an initial location to create the first sample s.t. it is within the given bounds. 
Add this to the grid and list of active samples
while the number of active samples > 0
	Pick a random sample x_i from the list of active samples
	Randomly sample K more locations within the R to 2R distance around x_i's location
	for each of these K random samples
		if the sample is in a location that is less than R distance away to any other sample on the grid:
			it is not a valid sample
		otherwise it is a valid sample:
			add it to the active samples list and to the background grid
	if no samples were added from the K samples chosen for this x_i sample:
		x_i is now a permanent sample so it is removed from the active sample listing
The final created background grid contains all valid samples as defined by the R to 2R restrictions
'''

### My Implementation:

- My implementation is almost exactly like the below algorithm except to speed up the number of mesh queries from the total number of possible samples including those that arent ultimately saved, I switched to just building the poisson samples relative to the objects overall bounding box and checking those final created samples against the mesh itself. This shortens the number of mesh queries to just being the same as the number of final samples created.
- BVH structure to speed up mesh checking for if a sample lies properly within the object.
- 2D/3D background grid used for allocation and speeding up search for nearby samples within the Radius and 2*Radius restriction.