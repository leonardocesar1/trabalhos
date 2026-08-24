module tesla_autopilot (
    input [3:0] sensor,
    input clock,
    input reset,
    output reg [2:0] state
);

// Sempre que o clock muda, divide ou não dependendo de reset
always @(posedge clock)
	if (reset) state <= 0;
	else state <= sensor % 3'b101;
  
endmodule