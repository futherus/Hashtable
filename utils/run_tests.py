import os
import sys

# Expects argv[1]   = directory to run tests in
#         argv[2::] = binary files of tests

RED = '\033[31m'
ENDC = '\033[m'
GREEN = '\033[32m'

os.chdir(sys.argv[1])

print("\n-----------------------------------Running tests-----------------------------------\n")

for arg in sys.argv[2::]:
    output = os.popen(arg)
    
    succ =  RED + "[FAIL]" + ENDC
    if(output.close() == None):
        succ = GREEN + "[ OK ]" + ENDC
    
    print("Test: {:70s} {}".format(arg, succ))