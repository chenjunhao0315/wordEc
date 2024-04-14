module top_word(a, b, c, d);
input signed [3:0] a, b, c;
output signed [3:0] d;

assign d = a * b + c;

endmodule
