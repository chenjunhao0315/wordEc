module top_word(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q);
input signed [5:0] a;
input signed [5:0] b;
input signed [5:0] c;
input signed [5:0] d;
input signed [5:0] e;
input signed [5:0] f;
input signed [5:0] g;
input signed [5:0] h;
input signed [5:0] i;
input signed [5:0] j;
input signed [5:0] k;
input signed [5:0] l;
input signed [5:0] m;
input signed [5:0] n;
input signed [5:0] o;
input signed [5:0] p;
output signed [8:0] q;

assign q = a*b + c*d + e*f + g*h + i*j + k*l + m*n + o*p;

endmodule
