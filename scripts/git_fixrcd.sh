commits=$(git log --grep='\bfix\b' -i --format="%h %s")

if [ -z "$commits" ]; then
    echo "No fixup commits found"
    exit 0
else
    echo "Fixup commits found:"
    echo "$commits"
    exit 1
fi
