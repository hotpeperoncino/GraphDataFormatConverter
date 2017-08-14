var linenumber = 0;

var llreader = require('line-by-line');
lr = new llreader(iputfile);

lr.on('error', function (err) {
    // 'err' contains error object
});

lr.on('line', function (line) {
    // 'line' contains the current line without the trailing newline character.
    if (linenumber == 0) {

    }
    
});

lr.on('end', function () {
    // All lines are read, file is closed now.
});
