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

wire signed [5:0] np3_a = ~a;
wire signed [5:0] np3_b = ~b;
wire signed [5:0] np3_c = -c;
wire signed [5:0] np3_d = -d;
wire signed [5:0] np3_e = e >>> 4;
wire signed [5:0] np3_f = f >>> 4;
wire signed [5:0] np3_g = g <<< 4;
wire signed [5:0] np3_h = h <<< 4;
wire signed [5:0] np3_i = i;
wire signed [5:0] np3_j = j;
wire signed [5:0] np3_k = ~k;
wire signed [5:0] np3_l = ~l;
wire signed [5:0] np3_m = {6{m[0]}};
wire signed [5:0] np3_n = {6{n[0]}};
wire signed [5:0] np3_o = -o;
wire signed [5:0] np3_p = -p;

assign q = np3_a*np3_b + np3_c*np3_d + np3_e*np3_f + np3_g*np3_h + np3_i*np3_j + np3_k*np3_l + np3_m*np3_n + np3_o*np3_p;

endmodule
