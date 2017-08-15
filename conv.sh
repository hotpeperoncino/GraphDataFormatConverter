NODE="node --max-old-space-size=32192 --optimize-for-size --max-executable-size=32192  --max_old_space_size=32192 --optimize_for_size --max_executable_size=32192"

$NODE app.js -f adj -t edge kron_g500-logn20.graph output.txt
