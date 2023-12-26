mkdir -p build && cd build
cmake ..
cmake --build . -j 4
cd ..
cp build/examples/zero_eq_cl .
cp wordEc/target/release/wordEc egraph