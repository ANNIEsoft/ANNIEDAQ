#!/bin/bash
#echo "<html>"

    while IFS='' read -r line || [[ -n "$line" ]]; do
        echo "$line "
    done <  ../html/header.html


    while IFS='' read -r line || [[ -n "$line" ]]; do
        echo "$line "
    done <  ../html/footer.html

#echo "</html>"