# Confidentiality and Integrity Algorithms

### Build Executable File

1. mkdir build
2. cd build
3. cmake .. -DLOG_LEVEL=INFO

### Execution

1. cd build
2. cp ../pattern_cmodel/input.txt  .
3. ./SecCmodel


### Test Pattern

04
561eb2dd   <--- Count
00000014   <--- Bearer
0          <--- Direction
47054125   <--- Key
561eb2dd   <--- Key
a94059da   <--- Key
05097850   <--- Key
0000005A   <--- Length  90 bits
00000000   <--- Message
00000000   <--- Message
00000000   <--- Message


Specification of the 3GPP Confidentiality and Integrity 
Algorithms 128-EEA3 & 128-EIA3 
Document 3: Implementor’s Test Data

input_EIA3_Test1.txt   <----- 5.3 Test Set 1
input_EIA3_Test2.txt   <----- 5.3 Test Set 2
input_EIA3_Test3.txt   <----- 5.3 Test Set 3
output_EIA3_Test1.txt  <----- 5.3 Test Set 1 MAC value
output_EIA3_Test2.txt  <----- 5.3 Test Set 2 MAC value
output_EIA3_Test3.txt  <----- 5.3 Test Set 3 MAC value