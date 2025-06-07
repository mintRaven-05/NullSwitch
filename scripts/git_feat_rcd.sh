commits=$(git log --grep='\bfeat\b' -i --format="%h %s")

if [ -z "$commits" ]; then
    echo "No feats committed"
    exit 0
else
    echo "Feat commits found:"
    echo "$commits"
    exit 1
fi
