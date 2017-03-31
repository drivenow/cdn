#!/bin/bash
rm -f /home/full/Desktop/cost.txt
for i in `seq 0 8`
do
	./bin/cdn case_example2/batch2/0/case$i.txt out.txt
done
for i in `seq 0 8`
do
	./bin/cdn case_example2/batch2/1/case$i.txt out.txt
done
for i in `seq 0 8`
do
	./bin/cdn case_example2/batch2/2/case$i.txt out.txt
done

