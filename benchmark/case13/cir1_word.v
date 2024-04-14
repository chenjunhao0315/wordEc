module top_word(a, b, c, d);
input signed [18:0] a, b, c;
output signed [19:0] d;

wire signed [18:0] np3_a = -a;
wire signed [18:0] np3_b = b ^ 19'b0011110001001100111;
wire signed [18:0] np3_c = {~c[18:1], 1'b0};

assign d = np3_a + np3_b + np3_c + 102;

endmodule
