module top_word(a, b, c, d);
input [15:0] a, b, c;
output [31:0] d;

wire [15:0] np3_a = -a;
wire [15:0] np3_b = b ^ 16'b1011010011100101;
wire [15:0] np3_c = {8'b11111111, c[7:0]};


assign d = np3_a*np3_a + np3_b + np3_c;

endmodule
