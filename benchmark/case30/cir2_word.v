module top_word(in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, in13, in14, in15, in16, in17, in18, in19, in20, in21, in22, in23, in24, in25, out1);
input [8:0] in1, in8, in12, in16, in20, in24;
input [7:0] in2, in9, in13, in17, in21, in25;
input in3, in4, in6, in7, in10, in11, in14, in15, in18, in19, in22, in23;
input [6:0] in5;
output [16:0] out1;

assign out1 = - in2 * in1 * in3 + in2 * in1 - in13 * in12 * in10 + in13 * in12 - in17 * in16 * in14 + in17 * in16 - in21 * in20 * in18 + in21 * in20 - in25 * in24 * in22 + in25 * in24 + in5 - in9 * in8 * in6 + in9 * in8;

endmodule
