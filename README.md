# Zip-Code-Group-Project-2.0

Part 1 and Part 2 code. All that's left is:

Document (extensively) your C++ source code with comments and Doxygen tags.
Create a Doxygen PDF of your buffer classes and application program code.
Create a user guide showing how to use your program (including how to use the command line options, and how the output should appear)
Create a design document (a draft of this document should be submitted to the Dropbox by the end of one week)
Create a test document



To compile the code use the statement:
g++ -std=c++11 -lstdc++ -o buffer_test main.cpp buffer.cpp

The run statment is ./buffer_test.exe for generating the sorted table with the header (along with the lengthindicated and idex file)
The zip code search statement is ./buffer_test.exe -z56301 (or whatever zip code you are searching)

Remember that the first run statment must be ran before the search statement in order to generae the nessesary files.

You can also find a document generation of the sorted table in the sorted_state_boundaries.txt file.
