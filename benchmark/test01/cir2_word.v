module top_word(a, b, c, d);
input [7:0] a;
input [7:0] b;
input [7:0] c;
output [7:0] d;

assign d = a*b + a*c;

endmodule
