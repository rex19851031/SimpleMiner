Keys:
	W,S,A,D		: Control direction and move the camera.
	+/-		: Put/Delete block under camera location.
	Z,X,C		: Move the camera downward.
	SPACEBAR	: Move the camera upward.
	Mouse		: pitch,yaw the camera facing direction.
	Shift		: Move with faster speed (8x)

Bugs:
	When you stand on exactly int-position and the adjacent block is air , the grounded is not quiet right.

SLN Name & Location:
	Basic3DWorld.sln

Executable File Name:
	Basic3DWorld.exe	

Deep Learning:

	This sprint is quiet fun , cause we are allowed to implement our own cool stuff. But it cost too much time working on 

that , so I faild to wrapping all the openGL code and windows code. Just hafe way through it , since some of the wrapping code 

will causing the weird bug. So I didn't implement them all. The physic part is quiet easy. Although still have some minor issue,

but I know most way to solve it , just need more time to working on that. I have some trouble when moving the global variable 

out of main , and put them into my renderer.hpp. Some of them shows error about multiple declare , so I can implement/replace

most of the function in main. It's really frustrate. Not familiar enough with way to declare the global variable. I need some 

help with declare the function before I can make a further step of the wrapper code. But I left it to the last day , so don't 

really have chance to ask for advice. :P