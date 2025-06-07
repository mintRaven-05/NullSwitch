commits=$(git log --grep='\bchore\b' -i --format="%h %s")

if [ -z "$commits" ]; then
    echo "No chores committed"
    exit 0
else
    echo "Chore commits found:"
    echo "$commits"
    exit 1
fi
