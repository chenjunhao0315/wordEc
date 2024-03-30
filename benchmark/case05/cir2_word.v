module top_word(a, b, c, d);
input signed [7:0] a;
input signed [7:0] b;
input signed [7:0] c;
output signed [15:0] d;

assign d = a*b + a*c + b*c;

endmodule
