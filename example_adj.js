var reader = require('./adjreader');

reader.read('adj.txt', function(err,g) {
    console.log("num_vertices=",g.num_vertices);
    console.log("num_edges=",g.num_edges);

    console.log("vertices=",g.vertices);
    console.log("adjs=",g.adjs);    
    console.log("adj_begin=",g.adj_begin);
    console.log("adj_length=",g.adj_length);
    console.log("adj_list=",g.adj_list)

//    console.log("n=",g.nodes)
//    console.log("e=",g.edges)    
});
