# TP1 - Composição Musical Algorítmica

Trabalho desenvolvido para a disciplina IA Generativa para Música, da UFMG.

O objetivo é gerar melodias a partir de músicas tradicionais irlandesas, utilizando cadeias de Markov e o dataset público [The Session Data](https://github.com/adactio/TheSession-data/tree/main).

## Arquivos

- `Markov.ipynb`: código principal, responsável por carregar os dados, filtrar as músicas, montar a cadeia de Markov e gerar novas melodias.
- `tunes.json`: conjunto de dados utilizado como fonte das melodias.
- `requirements.txt`: bibliotecas necessárias para executar o código.
- `musica_1.mid`, `musica_2.mid` e `musica_3.mid`: arquivos MIDI das músicas geradas.
- `musica_1.wav`, `musica_2.wav` e `musica_3.wav`: versões em áudio para facilitar a audição dos resultados.

## Execução

1. Abra a pasta do projeto em um ambiente com Python e Jupyter Notebook.
2. Instale as dependências com o comando:

   ```bash
   pip install -r requirements.txt
   ```

3. Abra o arquivo `Markov.ipynb`.
4. Execute todas as células usando a opção **Run All**.


## Músicas

Os três arquivos disponibilizados foram gerados com uma cadeia de Markov de ordem 2, utilizando diferentes estados iniciais sorteados a partir do conjunto de dados filtrado.

Ao executar o notebook novamente, os arquivos `musica_1`, `musica_2` e `musica_3` serão substituídos pelos novos resultados.

O código também permite alterar os parâmetros musicais e experimentar outras configurações.

## IA

Gemini foi utilizado como apoio durante o desenvolvimento, principalmente para esclarecer dúvidas de sintaxe específicas das bibliotecas, auxiliar no estudo dos conceitos da disciplina e revisar o texto do relatório. As decisões sobre a implementação e a execução do código foram feitas durante o desenvolvimento do trabalho.

[Histórico da Conversa no Gemini](https://share.gemini.google/s1im2TqP7HRb)
