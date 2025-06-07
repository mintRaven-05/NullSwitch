commits=$(git log --grep='\bbuild\b' -i --format="%h %s")

if [ -z "$commits" ]; then
    echo "No builds committed"
    exit 0
else
    echo "Build commits found:"
    echo "$commits"
    exit 1
fi
