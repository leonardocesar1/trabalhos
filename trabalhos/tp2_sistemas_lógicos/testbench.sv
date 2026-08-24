`timescale 1ns / 1ps

module testbench();

    reg clock;                                                                          // clock declarado como um registrador
    reg reset;                                                                          // reset declarado como um registrador
    reg start;                                                                          // start declarado como um registrador
    wire [3:0] count;                                                                   // count declarado como um fio
    wire rco;                                                                           // rco declarado como um fio

    contador_buffer streaming (clock, reset, start, count, rco);                        // conexão dos sinais do testbench às entradas e saídas do módulo

    initial begin
        clock = 0;
        forever #35 clock = ~clock;                                                     // atualização do clock indefinidamente durante a simulação
    end

    initial begin
        $dumpfile("dump.vcd");
        $dumpvars(1, testbench);

        reset = 1;                                                                      // começa com o reset para evitar comportamentos indesejados
        start = 0;                                                                      // gatilho ainda não ativado

        #105;
        reset = 0;                                                                      // permissão para novos pacotes

        #210;
        start = 1;                                                                      // começo do carregamento do vídeo

        #700;
        reset = 1;                                                                      // acionamento de reset depois de sete segundos
 
        #105;
        reset = 0;                                                                      // permissão para novos pacotes novamente

        #2000;
        start = 0;                                                                      // final da transmissão depois de vinte segundos

        $finish;
    end

    initial begin
        $monitor("Time: %0dns, clock: %b, reset: %b, start: %b, count: %d, rco: %b",
                 $time, clock, reset, start, count, rco);                               // monitoramento das variáveis para a exibição no console
    end

endmodule
