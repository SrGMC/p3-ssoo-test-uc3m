#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

function check_result() {
    retval=0
    max=10

    if [[ $# -gt 1 ]]; then
        echo " $(tput bold)Nota$(tput sgr0): Ejecutando test con $2 iteraciones"
        max=$2
    fi

    consecutive=0

    for n in $(seq 0 $max); do
        (./concurrent_market "$1") &> out.txt
        c_market_res=$?
        enqueues=$(cat out.txt | grep "enqueued operation" | wc -l)
        dequeues=$(cat out.txt | grep "dequeued operation" | wc -l)
        reads=$(cat out.txt | grep "MARKET STATUS REPORT START" | wc -l)

        nconsenq=$(cat out.txt | grep "operation" | uniq -c | sort | grep "enqueued" | wc -l)
        nconsdeq=$(cat out.txt | grep "operation" | uniq -c | sort | grep "dequeued" | wc -l)

        if [[ $nconsenq -eq 1 ]] || [[ $nconsdeq -eq 1 ]]; then
            consecutive=$(( consecutive + 1))
        fi

        if [[ $enqueues -eq $dequeues ]]; then
                if [[ $1 -eq 1 ]] 
                then
                    if [[ $enqueues -ne 12 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 2 ]] || [[ $1 -eq 3 ]]
                then
                    if [[ $enqueues -ne 24 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 4 ]]
                then
                    if [[ $enqueues -ne 9 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 5 ]]
                then
                    if [[ $enqueues -ne 38 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 6 ]]; then
                    cat out.txt | grep "ERROR: stock with ID" > /dev/null
                    if [[ $? -ne 0 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 7 ]]
                then
                    if [[ $enqueues -ne 12 ]] || [[ $reads -le 1 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 8 ]]
                then
                    if [[ $enqueues -ne 38 ]] || [[ $reads -le 1 ]]; then
                        retval=1
                        break
                    fi
                elif [[ $1 -eq 9 ]]
                then
                    if [[ $c_market_res -ne 0 ]]; then
                        retval=1
                        break
                    fi
                else
                    retval=1
                    break
                fi
        else 
            retval=1
            break
        fi
    done

    if [[ $consecutive -eq $max ]]; then
        retval=1
        break
    fi

    return $retval
}

#
# init
#

echo -e "Recompilando\n"
make clean &> /dev/null 
make &> /dev/null

for i in {1..9}; do
    echo "Ejecutando test $i"
    check_result $i $1
    if [[ $? -eq 0 ]]
    then
        printf " [${GREEN}✓${NC}] Test $i\n"
    else
        printf " [${RED}x${NC}] Test $i\n"
    fi
done

echo ""
echo "Limpiando"
rm out.txt