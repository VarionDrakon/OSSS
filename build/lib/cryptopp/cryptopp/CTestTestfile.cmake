# CMake generated Testfile for 
# Source directory: /home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp
# Build directory: /home/varion_drakon/Documents/Dev/TestOfFate/OSSS/build/lib/cryptopp/cryptopp
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[cryptopp-build_cryptest]=] "/usr/bin/cmake" "--build" "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/build" "--target" "cryptest" "--config" "Debug")
set_tests_properties([=[cryptopp-build_cryptest]=] PROPERTIES  FIXTURES_SETUP "cryptest-build" LABELS "cryptopp;cryptopp-cryptest" _BACKTRACE_TRIPLES "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp/CMakeLists.txt;1442;add_test;/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp/CMakeLists.txt;0;")
add_test([=[cryptopp-cryptest]=] "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/build/lib/cryptopp/cryptopp/cryptest" "v")
set_tests_properties([=[cryptopp-cryptest]=] PROPERTIES  FIXTURES_REQUIRED "cryptest-build" LABELS "cryptopp;cryptopp-cryptest" WORKING_DIRECTORY "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/sources" _BACKTRACE_TRIPLES "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp/CMakeLists.txt;1455;add_test;/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp/CMakeLists.txt;0;")
add_test([=[cryptopp-cryptest-extensive]=] "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/build/lib/cryptopp/cryptopp/cryptest" "tv" "all")
set_tests_properties([=[cryptopp-cryptest-extensive]=] PROPERTIES  FIXTURES_CLEANUP "cryptest-build" LABELS "cryptopp;cryptopp-cryptest" WORKING_DIRECTORY "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/sources" _BACKTRACE_TRIPLES "/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp/CMakeLists.txt;1467;add_test;/home/varion_drakon/Documents/Dev/TestOfFate/OSSS/lib/cryptopp/cryptopp/CMakeLists.txt;0;")
