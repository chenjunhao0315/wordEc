### Folder Structure

- R12K41016
	- README.txt
	- report
	- src
		- word-level
			- src (source code of zero polynomial checking framework)
			- examples (source code of zero polynomial checking interface)
			- tests (unit tests for zero polynomial checking framework)
			- benchmark (some testing data for zero polynomial checking)
			- wordEc (bit-width concern egraph engine)
				- src (source code of bit-width concern egraph engine)
			- build.sh (build up script)
	- slides
	- references
	
### How to compile and run
#### Compile
```
cd R12k41016/src/word-level
sh build.sh
```
#### Run
```
./egraph
```

