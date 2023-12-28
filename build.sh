mkdir -p build && cd build
cmake ..
cmake --build . -j 4
make test
cd ..
cd wordEc
cargo build --release
cd ..
cp build/examples/zero_eq_cl .
cp wordEc/target/release/wordEc egraph