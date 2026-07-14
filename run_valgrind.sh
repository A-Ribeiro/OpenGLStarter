#!/bin/sh

# Display setup documentation
echo "================================================================="
echo "  Memory Leak Analysis Automation"
echo "================================================================="
echo "Configure your CMake project using the LEAK_ANALYSE_TOOL flag:"
echo ""
echo "  cmake .. -DLEAK_ANALYSE_TOOL=libasan"
echo "    -> Enables AddressSanitizer. Do NOT use this script."
echo "    -> Run your binary directly in the terminal."
echo ""
echo "  cmake .. -DLEAK_ANALYSE_TOOL=Valgrind"
echo "    -> Disables ASan and enables maximum debugging symbols."
echo "    -> Required flags injected by CMake for Valgrind:"
echo "       -g -O0 -fno-inline -fno-omit-frame-pointer -no-pie"
echo "================================================================="
echo " Running Valgrind... Check 'valgrind-out.txt' for the report."

# Execute Valgrind with standard leak-checking parameters
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         "$@"
