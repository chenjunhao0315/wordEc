module top_word(a, b, c, d, e, f, s, g);
input signed [7:0] a;
input signed [7:0] b;
input signed [7:0] c;
input signed [7:0] d;
input signed [7:0] e;
input signed [7:0] f;
input signed [3:0] s;
output signed [15:0] g;

assign g = s*a*b + s*c*d + s*e*f;

endmodule
