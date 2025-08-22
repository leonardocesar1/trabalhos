module contador_buffer (
    input wire clock,                                   // clock declarado como entrada
    input wire reset,                                   // reset declarado como entrada
    input wire start,                                   // start declarado como entrada
    output wire [3:0] count,                            // count declarado como saída
    output wire rco                                     // rco declarado como saída
);

    reg [3:0] buffer;                                   // registrador que armazena o valor do contador

    always @(posedge clock or posedge reset) begin      // utilização do posedge, como falado em sala
        if (reset) begin
            buffer <= 4'b0000;                          // esvaziando o buffer com o sinal de reset
        end else if (start) begin                       // verificando se o start está ativo
            if (buffer < 4'b1100) begin
                buffer <= buffer + 4'b0001;             // incrementando o contador na subida do clock
            end if (rco) begin
                buffer <= 4'b0000;                      // momento de reinicialização do contadro com o rco
            end
        end
    end

    assign count = buffer;                              // atribuição do buffer à saída
    assign rco = (buffer == 4'b1100);                   // ativa a propagação de um carry no valor máximo

endmodule
