make clean
make
srun -psc -N2 -n2 ./hw1 1000003 28.in 28.temp.out
make clean
# xxd 28.in
# xxd 28.out
# xxd 28.temp.out
