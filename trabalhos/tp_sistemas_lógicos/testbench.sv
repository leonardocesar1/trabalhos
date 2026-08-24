module testbench;

    // Declara sinais de teste e conecta tesla_autopilot
    reg [3:0] sensor;
    reg clock;
    reg reset;
    wire [2:0] state;

    tesla_autopilot ta (
    	.sensor(sensor),
    	.clock(clock),
    	.reset(reset),
    	.state(state)
    );

    // Alterna o sinal de clock
    always begin
    	#5 clock = ~clock;
    end

    // Define valores iniciais e simula entradas de teste
    initial begin
    	$dumpfile("dump.vcd");
    	$dumpvars;
    	clock = 1;
    	reset = 1;
    	#10 reset = 0;
        #20 sensor = 4'b0110;
        #20 sensor = 4'b1101;
        #20 sensor = 4'b1010;
        #20 sensor = 4'b0011;
        #20 sensor = 4'b1001;
        $finish;
    end

endmodule