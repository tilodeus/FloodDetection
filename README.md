# FloodDetection
This is a project for ELTE IK workshop, created by students with the purpose of detecting flooded areas on satelite images.
Gunna be so awsum'!


Project TODO list:
palnned functions:
cluster()
- iterate trough input image pixels
	- on different channels (4)
		- record intensity value and ++ the appropriate counter in the intensity matrix
    - ?

paint_cluster_elements()
- iterate through input image pixels
- check pixel in intensity matrix belonging to flood_cluster to natural_water mask
	- if pixel coo belonging to flood_cluster intensity is MATCH to nonzero pixel value on natural_water mask
		===> ignore pixel coo (better paint it GREY)
	- if pixel coo belong to other other_cluster intensities
		===> paint it accordingly


These approaches seem to be too memory thirsty methods. Any better ideas?
