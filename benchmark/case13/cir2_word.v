module top_word(a, b, c, d);
input signed [18:0] a, b, c;
output signed [19:0] d;

assign d = a + b + c + 102;

endmodule
