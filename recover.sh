#!/bin/bash

# Check if the file exists
if [ ! -f "recover_lines" ]; then
    echo "File 'recover' not found."
    exit 1
fi

# Initialize a counter
counter=1

# Loop through each line in the file
while IFS= read -r line; do
    # Run the git command and write output to a file
    output_file="recover/output_${counter}.txt"

    # Remove end-of-line characters
    command_without_eol=$(echo "$line" | tr -d '\r\n')

    git cat-file -p $command_without_eol > "$output_file" 2>&1
    #echo "$counter ====  $command_without_eol hi"

    # Check if the git command was successful
    if [ $? -eq 0 ]; then
        echo "Command $line successful. Output written to $output_file"
    else
        echo "Command $line failed. Error output written to $output_file"
    fi

    # Increment the counter
    ((counter++))
done < "recover_lines"