@echo off
REM =======================================================================
REM Script corrigido para rodar todos os testes.
REM =======================================================================

REM Altera para o diretório de trabalho
cd C:\Users\Leo\Documents\pc\vscode\TP2po

REM Prepara o arquivo de saída. O operador ">" sobrescreve qualquer conteúdo anterior.
echo Testes TP2 PO - Resultado Completo > resultado_completo.txt
echo =================================== >> resultado_completo.txt

REM Lista de arquivos de teste a serem executados
set "FILES=example01.txt example02.txt example03.txt example04.txt example05.txt example06.txt example07.txt example08.txt example09.txt example10.txt example11.txt example12.txt"

REM Loop para executar cada teste
for %%f in (%FILES%) do (
    echo. >> resultado_completo.txt
    echo -------------------------------------------------- >> resultado_completo.txt
    echo Executando teste: %%f >> resultado_completo.txt
    echo -------------------------------------------------- >> resultado_completo.txt

    REM Linha CORRIGIDA: Usa aspas duplas para garantir que o caminho seja lido como um único argumento
    julia tp2_2023087923.jl "testes/%%f" >> resultado_completo.txt
)

echo. >> resultado_completo.txt
echo =================================== >> resultado_completo.txt
echo Todos os testes foram concluídos. >> resultado_completo.txt
echo O resultado completo está em resultado_completo.txt >> resultado_completo.txt

pause
