# an example compillation of program.sg

source core.sh # this already parses arguments and sets up fifo dir

check_io_files 1 0 # 1 input, 0 output files


fifo_a="${fifo_directory}/a"
fifo_b="${fifo_directory}/b"

mkfifo "$fifo_a"
cat "${input_files[0]}" | tee "$fifo_a" "$fifo_b" &

cat "$fifo_a"
cat "$fifo_b"


# cleanup

delete_fifo_dir
