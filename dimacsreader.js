function edgeread(inputfile, cb) {

    var linenumber = 0;
    var error = false;
    
    var llreader = require('line-by-line');
    lr = new llreader(inputfile);

    lr.on('error', function (err) {
	// 'err' contains error object
	error = true;
	cb(err);
    });

    var indexmap = {};
    var next_index = 0;
    var num_edges = 0;
    var num_vertices = 0;
    var from, to;
    var data = [];
    var N, M;
    
    lr.on('line', function (line) {
	linenumber++;

	a = line.split(" ");

	if (a[0] == "c")
	    return;
	if (a[0] == "p") {
	    if (a[1] != "edge") {
		console.log("format error");
		return;
	    }

	    N = parseInt(a[2]);
	    M = parseInt(a[3]);

	    return;
	}
	if (a[0] == "e") {
	    var from = parseInt(a[1]);
	    var to = parseInt(a[2]);
	    if (indexmap[from] == undefined) {
		indexmap[from] = next_index++;
	    }
	    if (indexmap[to] == undefined) {
		indexmap[to] = next_index++;
	    }
	    num_edges++;
	    data.push([a[1],a[2]]);
	}
    });

    var adj_begin = [];
    var adj_length = [];
    var adj_vec = [];
    var adj_list = [];

    function max(i, j) { return (i>j)?i:j; }
    function syscopy(targ, targfrom, src, srcfrom, count)
    {
//	console.log("syscpy", targfrom, srcfrom, count);
	var i = 0;
	while (i < count) {
	    targ[targfrom+i] = src[srcfrom+i];
	    i++;
	}
    }
    function alength(o) {
	return Object.keys(o).length;
    }
    lr.on('end', function () {
	if (error) return;
	
	// All lines are read, file is closed now.
	num_vertices = next_index;
//	console.log("um_vertices", 	num_vertices);
	var i = 0;
	while (i < num_vertices) {
	    adj_begin[i] = 0;
	    adj_length[i] = 0;
	    adj_vec[i] = [];
	    i++;
	}
	i = 0;
	while (i < num_edges) {
	    adj_list[i] = 0;
	    i++;
	}
	i = 0;
	var max_degree = -1;
	while (i < data.length) {
	    var a = data[i];
//	    console.log(a[0]);
	    from = a[0];
	    to = a[1];
//	    console.log("x=",indexmap[from]);
	    adj_vec[indexmap[from]].push(indexmap[to]);
	    i++;
	}

	var next_offset = 0;
	i = 0;
	while (i < num_vertices)
	{
	    var list_size = alength(adj_vec[i]);

	    adj_begin[i] = next_offset;
	    adj_length[i] = list_size;

//	    console.log("syscpy", next_offset, 0, list_size);
	    syscopy(adj_list, next_offset, adj_vec[i], 0, list_size);
	    next_offset += list_size;

	    i++;
	}


	var arg = {
	    vertices: indexmap,
	    num_vertices: num_vertices,
	    num_edges:num_edges,
	    max_degree: max_degree,
	    adj_begin: adj_begin,
	    adj_length: adj_length,
	    adjs: adj_vec,
	    adj_list: adj_list};
	cb(null, arg);
    });

}

module.exports.read = edgeread;
