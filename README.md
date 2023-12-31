### Folder Structure

- R12K41016
	- README.txt
	- README.pdf
	- report
		- R12K41016_report.pdf
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
		- R12K41016_slides.pptx
	- references
		- ex1_ori.png
		- ex1_rw.png
		- ex2_ori.png
		- ex2_rw.png
		- ex3_ori.png
		- ex3_rw.png
		- Automatic_Datapath_Optimization_using_E-Graphs.pdf
		- Datapath_Verification_via_Word-Level_E-Graph_Rewriting.pdf
		- Equivalence_verification_of_arithmetic_datapaths_with_multiple_word-length_operands.pdf
		- Equivalence_verification_of_polynomial_datapaths_with_fixed-size_bit-vectors_using_finite_ring_algebra.pdf

### Source code
File: R12K41016/src
or Github: https://github.com/chenjunhao0315/wordEc.git

### How to compile and run
#### Compile
```
cd R12K41016/src/word-level
sh build.sh
```
#### Run
```
./egraph <spec> <impl>
```
#### Some examples
##### Small Example 1
Testing $$4x^2\equiv 4x$$ when $$x$$is $$3$$bits.
```
./egraph "(* 4 (* (var x 3) (var x 3) 3) (3))" "(* 4 (var x 3) 3)"
```
##### Small Example 2
Testing $$(A\ll M)\cdot (B\ll N)\equiv (A\cdot B)\ll (M+N)$$ when $$A$$ is $$12$$bits, $$B$$ is $$8$$bits, $$M, N$$ are $$4$$bits.
```
./egraph "(* (<< (var A 12) (var M 4) 31) (<< (var B 8) (var N 4) 31) 63)" "(<< (* (var A 12) (var B 8) 32) (+ (var M 4) (var N 4) 5) 63)"
```

##### Example 5.1
```
./egraph "(+ (+ (+ (+ (+ (+ (* 156 (* (* (var x 16) (var x 16) 16) (* (* (var x 16) (var x 16) 16) (* (var x 16) (var x 16) 16) 16) 16) 16) (* 62724 (* (var x 16) (* (* (var x 16) (var x 16) 16) (* (var x 16) (var x 16) 16) 16) 16) 16) 16) (* 17968 (* (* (var x 16) (var x 16) 16) (* (var x 16) (var x 16) 16) 16) 16) 16) (* 18661 (* (var x 16) (* (var x 16) (var x 16) 16) 16) 16) 16) (* 43593 (* (var x 16) (var x 16) 16) 16) 16) (* 40224 (var x 16) 16) 16) 13281 16)" "(+ (+ (+ (+ (+ (+ (* 156 (* (* (var x 16) (var x 16) 16) (* (* (var x 16) (var x 16) 16) (* (var x 16) (var x 16) 16) 16) 16) 16) (* 5380 (* (var x 16) (* (* (var x 16) (var x 16) 16) (* (var x 16) (var x 16) 16) 16) 16) 16) 16) (* 1584 (* (* (var x 16) (var x 16) 16) (* (var x 16) (var x 16) 16) 16) 16) 16) (* 10469 (* (var x 16) (* (var x 16) (var x 16) 16) 16) 16) 16) (* 27209 (* (var x 16) (var x 16) 16) 16) 16) (* 7456 (var x 16) 16) 16) 13281 16)"
```
##### Example 5.2
```
./egraph "(+ (- (+ (+ (+ (* 16384 (* (* (var A 12) (var A 12) 16) (* (var A 12) (var A 12) 16) 16) 16) (* 16384 (* (* (var B 8) (var B 8) 16) (* (var B 8) (var B 8) 16) 16) 16) 16) (- (* 64767 (* (var A 12) (var A 12) 16) 16) (* 64767 (* (var B 8) (var B 8) 16) 16) 16) 16) (var A 12) 16) (var B 8) 16) (* (* (* 57344 (var A 12) 16) (var B 8) 16) (- (var A 12) (var B 8) 16) 16) 16)" "(+ (+ (+ (+ (+ (+ (* 24576 (* (var B 8) (* (var A 12) (var A 12) 16) 16) 16) (* 15615 (* (var A 12) (var A 12) 16) 16) 16) (* 8192 (* (var A 12) (* (var B 8) (var B 8) 16) 16) 16) 16) (* 32768 (* (var A 12) (var B 8) 16) 16) 16) (var A 12) 16) (* 17153 (* (var B 8) (var B 8) 16) 16) 16) (* 65535 (var B 8) 16) 16)"
```
##### Example 5.3
```
./egraph "(* (<< (+ (- (+ (+ (+ (* 16384 (* (* (var A 12) (var A 12) 16) (* (var A 12) (var A 12) 16) 16) 16) (* 16384 (* (* (var B 8) (var B 8) 16) (* (var B 8) (var B 8) 16) 16) 16) 16) (- (* 64767 (* (var A 12) (var A 12) 16) 16) (* 64767 (* (var B 8) (var B 8) 16) 16) 16) 16) (var A 12) 16) (var B 8) 16) (* (* (* 57344 (var A 12) 16) (var B 8) 16) (- (var A 12) (var B 8) 16) 16) 16) (var M 4) 31) (<< (var C 16) (var N 4) 31) 63)" "(<< (* (+ (+ (+ (+ (+ (+ (* 24576 (* (var B 8) (* (var A 12) (var A 12) 16) 16) 16) (* 15615 (* (var A 12) (var A 12) 16) 16) 16) (* 8192 (* (var A 12) (* (var B 8) (var B 8) 16) 16) 16) 16) (* 32768 (* (var A 12) (var B 8) 16) 16) 16) (var A 12) 16) (* 17153 (* (var B 8) (var B 8) 16) 16) 16) (* 65535 (var B 8) 16) 16) (var C 16) 32) (+ (var M 4) (var N 4) 5) 63)"
```
