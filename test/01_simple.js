var TCC = require('../index.js'),
    test = require('tap').test;

test(function (t) {
    var tcc = new TCC();
    tcc.compileString('int main() { return 4649; }');
    var ret = tcc.run();
    t.equals(ret, 4649);
    t.end();
});
