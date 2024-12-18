@echo off
echo Generating test data...
python genTestData.py 1000000 Resources/TestData/1MB.bin
python genTestData.py 10000000 Resources/TestData/10MB.bin
python genTestData.py 100000000 Resources/TestData/100MB.bin
python genTestData.py 1000000000 Resources/TestData/1000MB.bin