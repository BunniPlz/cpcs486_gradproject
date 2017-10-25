Mandy Chiu cccpsoda@gmail.com
I changed the colors of light0 and light1 to white.
In the fragment shader bronze.frag.glsl, I obtain the reflection vector of the 2 lights using the direction vectors and the normal vector and the reflect() function.
I set the values of W and P vectors to 1,1,1,1 and 1,2,20,1 respectively.
I wrote the function ComputeColor() which takes the
	- eyeDirection, the direction of the viewing vector
	- W, which I think is the base color of the fragment
	- P, which I think is a filter meant to dampen each value of W individually
	- reflection vector
For each light 0 and 1, I ComputeColor().
I get the final fragment color by adding the results of each computed value together.
