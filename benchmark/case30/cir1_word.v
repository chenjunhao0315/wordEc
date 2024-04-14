module top_word(in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, in13, in14, in15, in16, in17, in18, in19, in20, in21, in22, in23, in24, in25, out1);
input [8:0] in1, in8, in12, in16, in20, in24;
input [7:0] in2, in9, in13, in17, in21, in25;
input in3, in4, in6, in7, in10, in11, in14, in15, in18, in19, in22, in23;
input [6:0] in5;
output [16:0] out1;

wire [8:0] np3_in1 = -in1;
wire [7:0] np3_in2 = in2 >> 2;
wire       np3_in3 = ~in3;
wire       np3_in4 = in4;
wire [6:0] np3_in5 = -in5;
wire       np3_in6 = ~in6;
wire       np3_in7 = in7;
wire [8:0] np3_in8 = in8 ^ 9'b100110011;
wire [7:0] np3_in9 = {{4{in9[7]}}, {4{in9[3]}}};
wire       np3_in10 = in10;
wire       np3_in11 = in11;
wire [8:0] np3_in12 = in12 ^ 9'b010010100;
wire [7:0] np3_in13 = -in13;
wire       np3_in14 = ~in14;
wire       np3_in15 = in15;
wire [8:0] np3_in16 = -in16;
wire [7:0] np3_in17 = in17 >>> 3;
wire       np3_in18 = in18;
wire       np3_in19 = in19;
wire [8:0] np3_in20 = in20;
wire [7:0] np3_in21 = -in21;
wire       np3_in22 = ~in22;
wire       np3_in23 = in23 >> 3;
wire [8:0] np3_in24 = in24 << 2;
wire [7:0] np3_in25 = in25 ^ 8'b10011101;

assign out1 = - np3_in2 * np3_in1 * np3_in3 + np3_in2 * np3_in1 - np3_in13 * np3_in12 * np3_in10 + np3_in13 * np3_in12 - np3_in17 * np3_in16 * np3_in14 + np3_in17 * np3_in16 - np3_in21 * np3_in20 * np3_in18 + np3_in21 * np3_in20 - np3_in25 * np3_in24 * np3_in22 + np3_in25 * np3_in24 + np3_in5 - np3_in9 * np3_in8 * np3_in6 + np3_in9 * np3_in8;
endmodule
