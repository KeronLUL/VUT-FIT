#!/bin/sh
export POSIXLY_CORRECT=yes

DIR="$PWD"
ND=1
NF=0
NORMALIZATION=0
size1=0
size2=0
size3=0
size4=0
size5=0
size6=0
size7=0
size8=0
size9=0
return=0

while getopts ":i:n" opt; do
    case ${opt} in
        i )
            REGEX="$OPTARG"
            ;;
        n )
            NORMALIZATION=1
            ;;
        \? )
            echo "Invalid option. Use -i or -n" >&2 ; exit 1
            ;;
        : )
            echo "Option -$OPTARG requires an argument" >&2 ; exit 1
            ;;
    esac
done
shift $((OPTIND -1))

if [ "$#" -gt "1" ]; then
    echo "Too many arguments." >&2 ; exit 1
elif [ "$#" -eq "1" ]; then
    if [ ! -d "$1" ]; then
        echo "$1 is not a directory." >&2 ; exit 1
    fi
    DIR=$(realpath "$1")    
fi

if [ -n "$REGEX" ]; then
    if printf "%s" "$(basename "$DIR")" | grep -qE -- "$REGEX" 2>/dev/null; then
		echo "FILE_ERE must not cover the name of root directory." >&2 ; exit 1
	fi
fi

echo "Root directory: $DIR"

read_files(){
    for file in "$DIR"/* "$DIR"/.*; do

        if [ "$(basename "$file")" = "." ] || [ "$(basename "$file")" = ".." ]; then
            continue
        fi

        if [ ! -r "$file" ]; then
            return=1
            continue
        fi

        if [ -n "$REGEX" ]; then
            if printf "%s" "$(basename "$file")" | grep -qE -- "$REGEX" 2>/dev/null; then
                continue
            fi
        fi

        if [ -d "$file" ]; then
            ND=$((ND + 1))
            DIR="$file"
            read_files
            continue
        fi

        if [ -f "$file" ]; then
            NF=$((NF + 1))
            size=$(wc -c < "$file")

            ret="$?"
            if [ "$ret" -ne 0 ]; then 
                return=1
                continue
            fi

            if [ "$size" -lt 100 ]; then
                size1=$((size1 + 1))
            elif [ "$size" -lt 1024 ]; then
                size2=$((size2 + 1))
            elif [ "$size" -lt 10240 ]; then
                size3=$((size3 + 1))
            elif [ "$size" -lt 102400 ]; then
                size4=$((size4 + 1))
            elif [ "$size" -lt 1048576 ]; then
                size5=$((size5 + 1))
            elif [ "$size" -lt 10485760 ]; then
                size6=$((size6 + 1))
            elif [ "$size" -lt 104857600 ]; then
                size7=$((size7 + 1))
            elif [ "$size" -lt 1073741824 ]; then
                size8=$((size8 + 1))
            elif [ "$size" -ge 1073741824 ]; then
                size9=$((size9 + 1))
            fi
        fi
    done
}

read_files

echo "Directories: $ND"
echo "All files: $NF"

normalize(){
    line=79
    biggest=0
    tmp=0

    if [ -t 1 ]; then
        line=$(tput cols)
        line=$((line - 1))
    fi

    max_hashes=$((line - 12))

    for i in $(seq 9); do
        eval tmp="\$size$i"
        if [ "$tmp" -gt "$biggest" ]; then
            biggest="$tmp"
        fi
    done

    used_line=$((biggest + 12))

    if [ "$used_line" -gt "$line" ]; then
        for k in $(seq 9); do
            eval "size$k"=$((size$k * max_hashes))
            eval "size$k"=$((size$k / biggest))
        done
    fi
}

if [ "$NORMALIZATION" -eq 1 ]; then
    normalize
fi

print_hist(){
    if [ "$1" -gt 0 ]; then
        for i in $(seq "$1"); do
			printf "#"
        done
    fi
	printf '\n'
}

echo "File size histogram:"

printf "  <100 B  : " ; print_hist "$size1"
printf "  <1 KiB  : " ; print_hist "$size2"
printf "  <10 KiB : " ; print_hist "$size3"
printf "  <100 KiB: " ; print_hist "$size4"
printf "  <1 MiB  : " ; print_hist "$size5"
printf "  <10 MiB : " ; print_hist "$size6"
printf "  <100 MiB: " ; print_hist "$size7"
printf "  <1 GiB  : " ; print_hist "$size8"
printf "  >=1 GiB : " ; print_hist "$size9"


if [ "$return" -eq 1 ]; then
    echo "Unreadable files or missing permissions for some files, counting may not be accurate" >&2
fi

exit "$return"