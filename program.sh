# an example compillation of program.sg

# found out that at least one of the processes
# has to run in the foreground


# this parses arguments and sets up fifo dir
source core.sh 

check_io_files 1 0 # 1 input, 0 output files

fifo_a=$(get_fifo)
fifo_b=$(get_fifo)


function f {
    c=$(get_fifo)
    cat "$c" &
    sort > "$c"
    
}

# cat "${input_files[0]}" | sed '$a\' | tee "${fifo_b}" > "${fifo_a}" &
# cat "$fifo_a" | f  &

echo "asdf" > $fifo_a &


cat $fifo_a

# cat "$fifo_b" | f | rev


# cleanup

wait_for_all_fifos_empty

delete_fifo_dir

