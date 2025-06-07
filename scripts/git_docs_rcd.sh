commits=$(git log --grep='\bdocs\b' -i --format="%h %s")

if [ -z "$commits" ]; then
    echo "No docs changed"
    exit 0
else
    echo "Docs commits found:"
    echo "$commits"
    exit 1
fi
