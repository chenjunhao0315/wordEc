module top_word(a, b, c, d);
input signed [3:0] a, b, c;
output signed [3:0] d;

wire signed [3:0] np3_a = -a;
wire signed [3:0] np3_b = b ^ 4'b1011;
wire signed [3:0] np3_c = {~c[3:1], 1'b1};

assign d = np3_a * np3_b + np3_c;

endmodule
