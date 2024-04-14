module top_word(a, b, c, d);
input signed [7:0] a;
input signed [7:0] b;
input signed [7:0] c;
output signed [15:0] d;

wire signed [7:0] np3_a = a >>> 3;
wire signed [7:0] np3_b = -b;
wire signed [7:0] np3_c = c ^ 8'b10010110;

assign d = np3_a*np3_b + np3_a*np3_c + np3_b*np3_c;

endmodule
