#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
nbr_passed=0
nbr_failed=0
nbr_test=0
printf "\n\nSHREX TEST\n\n"
testing() 
{
    while IFS= read -r line
    do
        nbr_test=$((nbr_test + 1))
        expected=$(dash "$line")
        got=$(./42sh "$line")
        if [ "$expected" = "$got" ]; then
            nbr_passed=$((nbr_passed + 1))
            printf "Test_"$nbr_passed" : $line ${GREEN}Passed ${NC}\n"
        else
            nbr_failed=$((nbr_failed + 1))
            printf "Test_"$nbr_test": $line${RED} Failed${NC} => "
            printf "GOT :\n $got\nEXPECTED :\n $expected\n"
        fi

        echo "--------------------------------"
        echo "---------RESULTS BITCH!---------"
        echo "--------------------------------"
        printf "Failed : ${RED}$nbr_failed${NC}\n"
        printf "Passed : ${GREEN}$nbr_passed${NC}\n"

        printf "\n\n"
        nbr_passed=0
        nbr_failed=0
        nbr_test=0
    done 
}
export testing
testing "$0"

