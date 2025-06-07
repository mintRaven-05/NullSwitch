commits=$(git log --grep='\brefactor\b' -i --format="%h %s")

if [ -z "$commits" ]; then
    echo "No refactors committed"
    exit 0
else
    echo "Refactor commits found:"
    echo "$commits"
    exit 1
fi
