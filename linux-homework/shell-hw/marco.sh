#!/usr/bin/env bash
marco(){
    MARCO_DIR="$PWD"
    echo "save contents: $MARCO_DIR"
}

polo(){
    if [[ -z "$MARCO_DIR" ]]; then
        echo "error, please save marco contents"
        return 1
    fi
    cd "$MARCO_DIR" && echo "return: $MARCO_DIR"
}
