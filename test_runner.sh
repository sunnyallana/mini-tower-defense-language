#!/bin/bash
# test_runner.sh - Run all MTDL compiler examples and save outputs

set -e  # Exit on any error

echo "=== MTDL Compiler Test Runner ==="
echo

# Create directories if they don't exist
mkdir -p examples
mkdir -p test_outputs
mkdir -p test_logs

# Clean previous test outputs but NOT example files
rm -f test_outputs/*.json test_outputs/*.txt test_logs/*.log 2>/dev/null || true

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to create example file only if it doesn't exist
create_example_if_missing() {
    local filename=$1
    local content=$2

    if [ -f "$filename" ]; then
        echo "  Skipping $filename (already exists)"
        return 0
    fi

    # Create the file
    cat > "$filename" << EOF
$content
EOF
    echo "  Created $filename"
}

# Build the compiler first - USING YOUR EXACT COMMAND
echo "Building MTDL compiler with: g++ -std=c++17 -o mtdl src/*.cpp -Iinclude"
if g++ -std=c++17 -o mtdl src/*.cpp -Iinclude 2>build.log; then
    echo -e "${GREEN}✓ Build successful${NC}"
    rm -f build.log
else
    echo -e "${RED}✗ Build failed${NC}"
    echo "Build errors:"
    cat build.log
    exit 1
fi
echo

# Create example files ONLY if they don't exist
echo "Checking example files..."

create_example_if_missing "examples/basic.mtdl" 'map CastleDefense {
    size = (30, 20);
    path = [(0,10), (10,10), (10,5), (20,5)];
}

enemy Goblin {
    hp = 50;
    speed = 1.5;
    reward = 10;
}

tower Archer {
    range = 4;
    damage = 20;
    fire_rate = 1.5;
    cost = 75;
}

wave Wave1 {
    spawn(Goblin, count=15, start=0, interval=1);
}

place Archer at (3,8);'

create_example_if_missing "examples/optimization_test.mtdl" 'map TestMap {
    size = (10, 10);
    path = [(0,5), (5,5)];
}

enemy UsedEnemy {
    hp = 50;
    speed = 1.5;
    reward = 10;
}

enemy UnusedEnemy {
    hp = 100;
    speed = 1.0;
    reward = 0;
}

tower Tower1 {
    range = 3;
    damage = 15;
    fire_rate = 2.0;
    cost = 50;
}

tower Tower2 {
    range = 5;
    damage = 25;
    fire_rate = 1.0;
    cost = 100;
}

wave TestWave {
    spawn(UsedEnemy, count=5, start=0, interval=2);
    spawn(UsedEnemy, count=3, start=0, interval=2);
}'

create_example_if_missing "examples/error_syntax.mtdl" 'map BadMap {
    size = (10, 10)
    path = [(0,5), (5,5)];
}'

create_example_if_missing "examples/error_semantic.mtdl" 'enemy BadEnemy {
    hp = -50;
    speed = 1.5;
}'

create_example_if_missing "examples/error_reference.mtdl" 'wave BadWave {
    spawn(UndefinedEnemy, count=5, start=0, interval=2);
}'

create_example_if_missing "examples/simple.mtdl" 'map SimpleMap {
    size = (5, 5);
    path = [(0,2), (4,2)];
}

enemy SimpleEnemy {
    hp = 25;
    speed = 1.0;
}

place SimpleEnemy at (1,1);'

echo -e "\n${GREEN}✓ Example files ready${NC}\n"

# Function to run a test and log results
run_test() {
    local input_file=$1
    local test_name=$2
    local output_file="test_outputs/${test_name}.json"
    local log_file="test_logs/${test_name}.log"

    echo -n "Testing ${test_name}... "

    # Run compiler and capture output
    if ./mtdl "$input_file" -o "$output_file" 2>"$log_file"; then
        echo -e "${GREEN}✓ PASSED${NC}"
        echo "  Output: $output_file"
        if [ -s "$log_file" ]; then
            echo -e "  ${YELLOW}Note: Warnings in $log_file${NC}"
        fi
    else
        echo -e "${RED}✗ FAILED${NC}"
        echo "  Error log: $log_file"
        echo "  Last few lines of error:"
        tail -5 "$log_file" | sed 's/^/    /'
    fi
    echo
}

# Function to run optimization comparison test
run_optimization_test() {
    local test_name=$1
    echo -e "${YELLOW}=== Optimization Test: ${test_name} ===${NC}"

    # Run with optimization
    if ./mtdl "examples/${test_name}.mtdl" -o "test_outputs/${test_name}_optimized.json" 2>"test_logs/${test_name}_opt.log"; then
        echo -e "  ${GREEN}Optimized compilation successful${NC}"
    else
        echo -e "  ${RED}Optimized compilation failed${NC}"
    fi

    # Run without optimization
    if ./mtdl "examples/${test_name}.mtdl" -no-opt -o "test_outputs/${test_name}_non_optimized.json" 2>"test_logs/${test_name}_noopt.log"; then
        echo -e "  ${GREEN}Non-optimized compilation successful${NC}"
    else
        echo -e "  ${RED}Non-optimized compilation failed${NC}"
    fi

    # Compare outputs
    if [ -f "test_outputs/${test_name}_optimized.json" ] && [ -f "test_outputs/${test_name}_non_optimized.json" ]; then
        if cmp -s "test_outputs/${test_name}_optimized.json" "test_outputs/${test_name}_non_optimized.json"; then
            echo -e "  ${YELLOW}Warning: Optimized and non-optimized outputs are identical${NC}"
        else
            echo -e "  ${GREEN}Optimizations are working (outputs differ)${NC}"
            # Show size difference
            opt_size=$(stat -f%z "test_outputs/${test_name}_optimized.json" 2>/dev/null || stat -c%s "test_outputs/${test_name}_optimized.json")
            noopt_size=$(stat -f%z "test_outputs/${test_name}_non_optimized.json" 2>/dev/null || stat -c%s "test_outputs/${test_name}_non_optimized.json")
            echo "  Optimized size: ${opt_size} bytes"
            echo "  Non-optimized size: ${noopt_size} bytes"
            echo "  Difference: $((noopt_size - opt_size)) bytes"
        fi
    fi
    echo
}

# Function to run error test (should fail)
run_error_test() {
    local test_name=$1
    local expected_to_fail=$2  # "yes" if should fail

    echo -n "Error test ${test_name}... "

    local output_file="test_outputs/${test_name}_error.json"
    local log_file="test_logs/${test_name}_error.log"

    # Run compiler - it should fail for error tests
    if ./mtdl "examples/${test_name}.mtdl" -o "$output_file" 2>"$log_file"; then
        if [ "$expected_to_fail" = "yes" ]; then
            echo -e "${RED}✗ FAILED (should have errored but didn't)${NC}"
        else
            echo -e "${GREEN}✓ PASSED${NC}"
        fi
    else
        if [ "$expected_to_fail" = "yes" ]; then
            echo -e "${GREEN}✓ PASSED (correctly failed)${NC}"
            echo "  Error captured in: $log_file"
            echo "  Error type: $(head -1 "$log_file")"
        else
            echo -e "${RED}✗ FAILED (should not have errored)${NC}"
        fi
    fi
    echo
}

# Run tests
echo "=== Running Tests ==="

# Basic tests
run_test "examples/basic.mtdl" "basic"
run_test "examples/simple.mtdl" "simple"

# Optimization comparison test
run_optimization_test "optimization_test"

# Error tests (these should fail)
run_error_test "error_syntax" "yes"
run_error_test "error_semantic" "yes"
run_error_test "error_reference" "yes"

# Run with readable output
echo -e "${YELLOW}=== Readable Output Tests ===${NC}"
echo -n "Generating readable output for basic.mtdl... "
if ./mtdl examples/basic.mtdl -readable -o test_outputs/basic_readable.txt 2>test_logs/basic_readable.log; then
    echo -e "${GREEN}✓ DONE${NC}"
    echo "  Output: test_outputs/basic_readable.txt"
else
    echo -e "${RED}✗ FAILED${NC}"
fi
echo

# Summary
echo "=== Test Summary ==="
echo "Outputs saved in: test_outputs/"
echo "Logs saved in: test_logs/"
echo
echo "Files created:"
ls -la test_outputs/ 2>/dev/null || echo "  No output files generated"
echo
echo "To view a specific output:"
echo "  cat test_outputs/basic.json"
echo "  cat test_outputs/optimization_test_optimized.json"
echo
echo "To check for DPS optimization (should appear in optimized, not in non-optimized):"
echo "  grep -n 'dps' test_outputs/optimization_test_optimized.json"
echo "  grep -n 'dps' test_outputs/optimization_test_non_optimized.json"
echo
echo -e "${GREEN}Test run completed!${NC}"
