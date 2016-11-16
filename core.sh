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

function delete_fifo_dir {
    rm -rf "$fifo_directory"
}
