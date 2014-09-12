#!/bin/sh
# "Guerilla-testing" script
#
# TODO:
# - pass all files through, even non-ebook junk
# - randomize user interactions

echo "Please take me away, take me away, so far away..."
echo ""

OCHER=build/ocher
LIBRARY=/pool/Library/books

if [ -n "$1" ]; then
    LIBRARY="$1"
fi
if [ ! -d "$1" ]; then
    echo "Usage:  $0 <dir-with-books>"
    exit 1
fi

N=0
M=0

runTest()
{
    M=$((M+1))
    echo "Testing $1"
    valgrind --log-file="$1.vg" --error-exitcode=42 $OCHER -vvv --driver fd "$1" > /dev/null
    #gdb --args $OCHER "$1"
    if [ $? -eq 42 ]; then
        echo "Failed; see $1.vg"
        return 1
    else
        N=$((N+1))
    fi
}

find "$LIBRARY" -type f | while IFS= read f; do
    case "$f" in
        *.epub)
            runTest "$f"
            ;;
        *.html)
            runTest "$f"
            ;;
        *.txt)
            runTest "$f"
            ;;
        *)
            ;;
    esac
    [ $? -ne 0 ] && break
done

echo "$N of $M succeeded"
