var Int64 = require('int64-native'),
    chai = require('/Users/candu/git/travel-site/web/node_modules/chai'),
    expect = chai.expect;

describe('Int64', function testInt64() {
  it('can be constructed', function testConstructor() {
    var x = new Int64(),
        y = new Int64(42),
        z = new Int64(0xfedcba98, 0x76543210),
        w = new Int64('fedcba9876543210')
    expect(x.toString()).to.equal('0000000000000000');
    expect(y.toString()).to.equal('000000000000002a');
    expect(z.toString()).to.equal('fedcba9876543210');
    expect(w.toString()).to.equal('fedcba9876543210');
  });

  it('can be converted to Number', function testNumberConversion() {
    var a = new Int64(2),
        b = new Int64(3);
    expect(a + b).to.equal(5);
    var x = new Int64(),
        y = new Int64(42),
        z = new Int64(0xfedcba98, 0x76543210),
        w = new Int64('fedcba9876543210')
    expect(+x).to.equal(0);
    expect(+y).to.equal(42);
    expect(+z).to.equal(Infinity);
    expect(+w).to.equal(Infinity);
  });

  it('can be compared', function testComparison() {
    var a = new Int64(2),
        b = new Int64(3);
    expect(a.equals(a)).to.be.true;
    expect(a.equals(b)).to.be.false;
    expect(a.compare(a)).to.equal(0);
    expect(a.compare(b)).to.equal(-1);
    expect(b.compare(a)).to.equal(1);
  });

  it('can be bit-manipulated', function testBitManipulation() {
    var x = new Int64('fedcba9876543210');
    expect(x.high32().toString(16)).to.equal('fedcba98');
    expect(x.low32().toString(16)).to.equal('76543210');
    var y = x.and(new Int64(0xffff)),
        z = x.or(new Int64(0xffff)),
        w = x.xor(new Int64('fffffffffffffffff'));
    expect(y.toString()).to.equal('0000000000003210');
    expect(z.toString()).to.equal('fedcba987654ffff');
    expect(w.toString()).to.equal('0123456789abcdef');
    var a = new Int64(7),
        b = a.shiftLeft(1),
        c = a.shiftRight(1);
    expect(b.toString()).to.equal('000000000000000e');
    expect(c.toString()).to.equal('0000000000000003');
  });
});
