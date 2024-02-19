cargo build --release
cd tools || exit
cargo run -r --bin gen ./seeds.txt
cat in/0000.txt | cargo run -r --bin tester ../target/release/atcoder-heuristic-contest-030.exe > ../out.txt
read -r -p "Press enter to continue"
