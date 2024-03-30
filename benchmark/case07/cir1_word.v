module top_word(a, b, c, d, e, f, s, g);
input [7:0] a;
input [7:0] b;
input [7:0] c;
input [7:0] d;
input [7:0] e;
input [7:0] f;
input [3:0] s;
output [15:0] g;

wire [7:0] np3_a = -a;
wire [7:0] np3_b = b ^ 8'b01101001;
wire [7:0] np3_c = {{4{c[7]}}, {4{c[3]}}};
wire [7:0] np3_d = ~d;
wire [7:0] np3_e = e >>> 3;
wire [7:0] np3_f = f <<< 3;
wire [3:0] np3_s = -$signed(s ^ 4'b0110);

assign g = np3_s*np3_a*np3_b + np3_s*np3_c*np3_d + np3_s*np3_e*np3_f;

endmodule
