#!/bin/bash

printf "\ncompiling the code...\n"

make

printf "\nrunning the exectutable...\n"

./simulator

echo "lauching postprocess..."
cd ../data
#opening venv
source simulator_venv/bin/activate
python display_simulation_results.py
