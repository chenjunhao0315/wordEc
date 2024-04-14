module top_word(a, b, c, d);
input [15:0] a, b, c;
output [31:0] d;

assign d = a*a + b + c;

endmodule
