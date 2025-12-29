# compile gen_data_sim.cpp
g++ -Wall -O3 src/gen_data_sim.cpp -o bin/gen_data_sim

# compile main.cpp
g++ -Wall -O3 src/main.cpp lib/dynamics.cpp lib/GA.cpp -o bin/main
