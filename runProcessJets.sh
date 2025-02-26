#!/bin/bash

# Input file containing the list of directories
INPUT_FILE="hiForward_directories.txt"

# Loop through each line in the file
while IFS= read -r dir; do
    # Extract Run3_2023UPC_xxxxxx and HIForwardXX from the directory path
    run_id=$(echo "$dir" | awk -F'/' '{for (i=1; i<=NF; i++) if ($i ~ /Run3_2023UPC_[0-9]+/) print $i}')
    hi_forward=$(basename "$dir")

    # Construct the input file path pattern
    input_pattern="${dir}/*/*/*/*.root"

    # Construct the output file name
    output_file="output_${run_id}_${hi_forward}.root"

    # Run ROOT macro with the generated parameters
#    echo "Processing: $dir"
    echo root -l -q "processJets.C'(\"${input_pattern}\", \"${output_file}\")'"

#    echo "Output saved to: ${output_file}"

done < "$INPUT_FILE"
