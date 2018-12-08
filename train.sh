time=$(date +"%T")
echo $time

sh 00_clean_all.sh
sh 01_run_HCopy.sh >> temp
sh 02_run_HCompV.sh
sh 03_training.sh
sh 04_testing.sh > temp

time=$(date +"%T")
echo $time
cat result/accuracy >> record.dat
rm temp
