module top_word(a, b, c, d);
input [7:0] a;
input [7:0] b;
input [7:0] c;
output [7:0] d;

wire [7:0] np3_a = {{4{a[7]}}, {4{~a[3]}}};
wire [7:0] np3_b = {b[7], b[7:1]};
wire [7:0] np3_c = c ^ 8'b01101010;

assign d = np3_a*np3_b + np3_a*np3_c;

endmodule
