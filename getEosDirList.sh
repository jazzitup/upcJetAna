#!/bin/bash

# Define the EOS base path
EOS_BASE="/eos/cms/store/group/phys_heavyions/jdlang/run3_2023Data_Jan2024ReReco"

# Output file
OUTPUT_FILE="hiForward_directories.txt"

# Remove existing output file if it exists
rm -f "$OUTPUT_FILE"

# Loop through all Run3_2023UPC_* directories and find their HIForward* subdirectories
for upc_dir in $(eos ls "$EOS_BASE" | grep "Run3_2023UPC_[0-9]*"); do
    eos ls "$EOS_BASE/$upc_dir" | grep "^HIForward[0-9]*$" | awk -v base="$EOS_BASE/$upc_dir" '{print base "/" $0}' >> "$OUTPUT_FILE"
done

# Print the number of directories found
echo "Found $(wc -l < "$OUTPUT_FILE") directories."
echo "List saved to $OUTPUT_FILE."

