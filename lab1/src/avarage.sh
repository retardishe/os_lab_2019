sum=0
count=0
while [ -n "$1" ]
do
count=$(( $count+1 ))
sum=$(( $sum+$1 ))
shift
done
avg=$(( $sum/$count ))
echo "$avg"
