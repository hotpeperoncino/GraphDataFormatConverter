var cli = require('cli');
var async = require('async');
function help()
{
    console.log("graphconv -f <from> -t <to> inputfile outputfile");
    console.log("acceptable format is as <from>: edge/edge2/dimacs/adj,as <to>:edge/edge2");
}
var options = cli.parse({
    ifmt: ['f', 'input format', 'string'],
    ofmt: ['t', 'output format', 'string'],
    help: ['h', 'help']    ,
    verbose: ['v', 'verbose']    
});


cli.main(function (args, options) {
    if (options.help) {
	help();
	return;
    }
    if (options.verbose)
	console.log("<from>", options.ifmt, "<to>", options.ofmt);

    
    var reader;
    if (options.ifmt == "edge") {
	reader = require('./edgereader');
    } else if  (options.ifmt == "edge2") {
	reader = require('./edge2reader');
    } else if  (options.ifmt == "dimacs") {
	reader = require('./dimacsreader');
    } else if  (options.ifmt == "adj") {
	reader = require('./adjreader');
    } else {
	reader = {read: function(filename, cb) { cb("error unknown input format"); } };
    }
    var writer;

    if (options.ofmt == "edge") {
	writer = require('./edgereader');
    } else if (options.ofmt == "edge2") {
	writer = require('./edge2reader');
    } else {
	writer = {write: function(filename, g, cb) { cb("error unknown write format"); } };
    }

    console.log(args);

    
    var filename = args[0] ? args[0] : "input.txt";
    var outputname = args[1] ? args[1] : "output.txt";
    
    reader.read(filename, function(err, g) {
	if (err) {
	    console.log(err);
	    return;
	}
	writer.write(outputname, g, function(err) {
	    if (err) {
		console.log(err);
		return;
	    }
	    
	});
    });
});
