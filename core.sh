# the streamgraph executable core

set -u # throw an error if unset variables are used

# get the arguments

input_files=() # unnamed arguments
output_files=() # -o option arguments

while [ $# -ge 1 ]; do

    # this can read unnamed arguments and option arguments
    # even if they're not in order

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
		echo -e "ERROR: Unknown option -$OPTARG\n" >&2
		exit 1
		;;
	    :)
		echo -e "Error: Option -$OPTARG requires an option argument\n" >&2
		exit 1
		;;
	esac
    done

    shift $(( $OPTIND - 1 ))

done


