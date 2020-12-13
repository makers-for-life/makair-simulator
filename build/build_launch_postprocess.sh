#!/bin/bash
#creating necessary folders (if don't exists)
mkdir -p "../data/simulation_output"

printf "\ncompiling the code...\n"
make

printf "\nrunning the exectutable...\n"
./simulator

echo "lauching postprocess..."
cd ../data
#opening venv
source simulator_venv/bin/activate
python3 display_experiment_results.py simulation_output/log_simulator.csv 
