# the streamgraph executable core
# this provides some setup and cleanup functionality that
# is needed when executing a sg program
# Pascal Sommer, November 2016

# most of this will probably be wrapped in functions so
# that it can be called from the compiled streamgraph
# program.

# sg program will set variable $program_name with .sg exension

set -u # throw an error if unset variables are used

# create the fifo directory while making sure that multiple
# sg scripts can be run at the same time, as long as they're
# not started within the same second
fifo_directory="/tmp/sg-fifos_$(date +%s)" 
if [ ! -e "$fifo_directory" ]; then
    mkdir "$fifo_directory"
elif [ ! -d "$fifo_directory" ]; then
    echo "Can't create $fifo_directory because file exists" >&2
else
    if [ ! -w "$fifo_directory" ]; then
	echo "Fifo directory not writable" >&2
    fi
fi



# get the arguments

input_files=() # unnamed arguments
output_files=() # -o option arguments

while [ $# -ge 1 ]; do

    # this construct can read unnamed arguments and option
    # arguments even if they're not in order

    while [ $# -ge 1 ] && [ ${1:0:1} != "-" ]; do
	input_files+=($1)
	shift
    done

    OPTIND=1
    
    while getopts ":o:" opt; do
	case $opt in
	    o)
		output_files+=($OPTARG)
		;;
	    \?)
		echo "ERROR: Unknown option -$OPTARG\n" >&2
		exit 1
		;;
	    :)
		echo "Error: Option -$OPTARG requires an option argument\n" >&2
		exit 1
		;;
	esac
    done

    shift $(( $OPTIND - 1 ))
done


function check_io_files {
    
    if [ "$#" -ne 2 ]; then
	echo "ERROR: function check_io_files requires two arguments!" >&2
	exit 1
    fi
    
    nr_input_files="$1"
    nr_output_files="$2"
    
    if [ $nr_input_files -ne ${#input_files[@]} ]; then
	echo "ERROR: incorrect number of input files given!" >&2
	echo "    Expected: $nr_input_files files" >&2
	echo "    Given: ${#input_files[@]} files" >&2
	exit 1
    fi
    
    if [ $nr_output_files -ne ${#output_files[@]} ]; then
	echo "ERROR: incorrect number of output files given!" >&2
	echo "    Expected: $nr_output_files files" >&2
	echo "    Given: ${#output_files[@]} files" >&2
	exit 1
    fi

    for f in "${input_files[@]}"; do
	if [ ! -f "$f" ]; then
	    echo "ERROR: file $f does not exist!" >&2
	    exit 1
	fi
    done
}



# this will create a fifo and return the path to it. The
# file fifo_counter in the fifo directory will hold the
# number of the last fifo created.
# maybe add a garbage collector at some point
function get_fifo {
    current_fifo_number=$(bump_fifo_number)
    name="fifo_${current_fifo_number}"
    path_to_fifo="${fifo_directory}/$name"
    
    mkfifo "${path_to_fifo}"
    echo "${path_to_fifo}"
}

# function reading the fifo_counter file, echoing the number
# incrementing the number, and writing the new number to the
# file. If the file does not exist, echo 0 and write 1 to the
# file.
function bump_fifo_number {
    path="${fifo_directory}/fifo_counter"
    if [ -f "$path" ]; then
	nr=$(cat "$path")
    else
	nr=0
    fi

    echo $nr

    ((nr++))
    
    echo $nr > "$path"
}




function delete_fifo_dir {
    rm -rf "$fifo_directory"
}
