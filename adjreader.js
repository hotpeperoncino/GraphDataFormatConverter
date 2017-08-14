function graphfileread(inputfile, cb) {

    var linenumber = 0;
    var error = false;
    var nodes;
    var edges;
    var pos_edges=0;
    var pos_nodes=0;
    var N, M;    
    function reserveint(x) {
	var i = 0;
	var a = [];
	while (i < x) {
	    a.push(0);
	    i++;
	}
	return a;
    }
    
    var llreader = require('line-by-line');
    lr = new llreader(inputfile);

    lr.on('error', function (err) {
	// 'err' contains error object
	error = true;
	cb(err);
    });

    lr.on('line', function (line) {
	linenumber++;
//	console.log(""+linenumber + ":" + line);
	if (error) return;
	// 'line' contains the current line without the trailing newline character.
	if (linenumber == 1) {
	    a = line.split(" ");
	    if (a.length < 2) {
//		console.log("error:" + linenumber + "format error");
		return;
	    }
	    N = parseInt(a[0]);
	    M = parseInt(a[1]);
	    nodes = reserveint(N+1);
	    edges = reserveint(M);
	    nodes[pos_nodes++] = 0;
	    return;
	}
	var pos = linenumber-2;

	if (linenumber <= N+1) {
	    var vals = line.split(" ");
//	    console.log("len=",vals.length);
	    var i = 0;
	    while (i < vals.length) {
		var neighbor = parseInt(vals[i]);
		if (isNaN(neighbor)) { i++; continue; }
//		console.log("neighbor=", neighbor);
		edges[pos_edges++] = neighbor-1;
		i++;
	    }
	    nodes[pos_nodes++] = pos_edges;
	}
    });

    lr.on('end', function () {
	// All lines are read, file is closed now.
//	console.log(pos_nodes, pos_edges);
	var adj_length = [];
	for (i = 0; i < nodes.length-1; i++) {
	    adj_length[i] = nodes[i+1]-nodes[i];
	}
	var j = 0;
	var k = 0;
	var adjs=[];
	for (i = 0; i < nodes.length-1; i++) {
	    adjs[i] = [];
	    j = 0;
	    while (j < adj_length[i]) {
		adjs[i].push(edges[k++]); 
		j++;
	    }
	}
	var adj_begin = [];
	var vertices = {};
	for (i = 0; i < nodes.length-1; i++) {
	    adj_begin[i] = nodes[i];
	    vertices[i] = i;	    
	}
	var arg = {
	    vertices: vertices,
	    num_vertices: N,
	    num_edges: M,
	    edges: edges,
	    nodes: nodes,
	    adj_begin: adj_begin,
	    adj_list: edges,
	    adj_length: adj_length,
	    adjs:adjs,
	};
	cb(null,arg);
    });

}

module.exports.read = graphfileread;
