#!/bin/bash
last_mod=0

while true; do
    # Get current modification time
    current_mod=$(stat -c %Y control_flow.dot 2>/dev/null || echo 0)
    
    if [[ $current_mod -gt $last_mod ]]; then
        last_mod=$current_mod
        echo "Change detected: regenerating SVG..."
        dot -Tsvg control_flow.dot -o control_flow.svg
    fi

    sleep 1  # check every 1 second
done
