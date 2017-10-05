for num_process in {1,4,16}
do
for num_objects in {256,1024,16384}
do 
for feature in {3,4,5}
do 
 sleep 10 
 bash ./test_single.sh ${num_objects} 8192 ${feature} ${num_process}
 sleep 10
done
done
done


