# Graph Data Format Converter

Command

    conv -f from -t to inputfile outputfile
    acceptable format is as from: edge/edge2/dimacs/adj, as to:edge


Background

There are several formats to represent graph. There is no kitchensink converter supporting translation vise versa.
Don't invent new one please.

Text Format

1. CSR: Compressed Sparse Row
2. MATRIX: matrix of row and col. row is current, col is next: the cell has cost.
3. EDGES: list of pair of vertex id

       each lines have: from to
       
4. EDEGS2:
       firstline:  edge-count
       after: from to
       OR
       firstline:  vertex-count edge-count
       following each lines have: from to
      
       vertex id starts with 0 or 1
      
       As cost representation, each lines might have cost as third parameter;
       
      
5. ADJACENT: adjacent list of vertex; line number is vertex id
6. ROWCOL: colptr and rowptr
7. DIMACS:
       http://prolland.free.fr/works/research/dsat/dimacs.html
8. BSMR: Bitmap-based sparse matrix representation:
9. DCSR:




References

    https://link.springer.com/article/10.1007/s41019-016-0024-y

