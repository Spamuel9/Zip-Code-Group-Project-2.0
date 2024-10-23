# Define output file path
$outputFile = "./output_by_latitude.txt"

# Clear previous content in the output file (optional)
Clear-Content $outputFile

# Write the contents of main.cpp to the output file
Get-Content ./main.cpp | Out-File -FilePath $outputFile -Append

# Add a separator or message to distinguish between code and program output
Add-Content $outputFile "`n--- Program Output ---`n"

# Compile the C++ code
g++ -std=c++11 -lstdc++ -o buffer_test main.cpp buffer.cpp

# Run the program and append the output to the same file
./buffer_test.exe | Out-File -FilePath $outputFile -Append

# Print message to confirm the script ran successfully
Write-Host "Program ran successfully. Output saved to output_by_latitude.txt"

