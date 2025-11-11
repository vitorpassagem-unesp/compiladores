# MyBC - Interpretador de Expressões Aritméticas

**Grupo 2:**
- Gustavo Varjão de Frias
- Vitor Ferreira
- Vitor Alves Chuquer Zanetti Passagem

---

## Visão Geral

MyBC é um interpretador interativo de expressões aritméticas inspirado no comando `bc` do Unix. O projeto implementa um analisador léxico (lexer) e um analisador sintático (parser) que processam expressões matemáticas, respeitando precedência de operadores e suportando múltiplas bases numéricas.

---

## Características

### Operações Suportadas
- **Operadores aritméticos**: `+`, `-`, `*`, `/`
- **Operadores unários**: `-` (negativo)
- **Precedência de operadores**: Respeita ordem matemática padrão
- **Parênteses**: Controle de precedência com `(` e `)`
- **Atribuição**: Operador `:=` para variáveis

### Sistemas Numéricos Suportados
- **Decimal**: `123`, `0`
- **Ponto flutuante**: `3.14`, `.5`, `2.0e-3`

### Recursos Avançados
- **Tratamento de erros**: Mensagens detalhadas com linha e coluna
- **Recuperação de erros**: Interpretador continua após erro sintático
- **Interrupção controlada**: Captura `Ctrl+C` sem encerrar o programa
- **Comandos especiais**: `exit`, `quit` para sair

---

## Arquitetura

### Componentes Principais

#### 1. **Analisador Léxico (`lexer.c`)**
Responsável por transformar o fluxo de caracteres em tokens reconhecíveis:

- **Reconhecedores especializados**:
  - `isID()`: Identificadores e palavras reservadas
  - `isDEC()`: Números decimais
  - `isHEX()`: Números hexadecimais (0x...)
  - `isOCT()`: Números octais (0...)
  - `isNUM()`: Números de ponto flutuante
  - `isASGN()`: Operador de atribuição `:=`
  
- **Rastreamento de posição**:
  - `lineno`: Número da linha atual
  - `colno`: Coluna atual
  - `last_colno`: Última coluna válida antes de newline
  - `newline_flag`: Indica presença de quebra de linha

#### 2. **Analisador Sintático (`parser.c`)**
Implementa um parser descendente recursivo (LL(1)) baseado na gramática:

```
E -> T R
R -> + T R | - T R | ε
T -> F Q
Q -> * F Q | / F Q | ε
F -> ID | DEC | HEX | OCT | FLT | ( E )
```

- **Estruturas de dados**:
  - `acc`: Acumulador para resultados intermediários
  - `stack[]`: Pilha para salvar operandos
  - `sp`: Ponteiro da pilha
  
- **Técnicas de implementação**:
  - Labels `_Tbegin` e `_Fbegin` para processar precedência
  - Flags `oplus_flg`, `otimes_flg`, `ominus_flg` para operadores pendentes
  - `setjmp`/`longjmp` para recuperação de erros

---

## Compilação e Execução

### Requisitos
- Compilador GCC (ou compatível)
- Make
- Sistema Unix-like (Linux, macOS, WSL)

### Compilar
```bash
make
```

### Executar
```bash
./mybc
```

### Limpar arquivos objeto
```bash
make clean
```

---

## Exemplos de Uso

```
$ ./mybc
3+4*2
11
-5+10
5
(2+3)*4
20
3.14 * 2
6.28
exit
```

---

## Tratamento de Erros

### Erros Léxicos
```
Erro léxico na linha 1 e coluna 5: charactere inválido '@'
```

### Erros Sintáticos
```
3 + + 4
Erro de sintaxe na linha 1 e coluna 4.
Token Esperado: número --- Token no lookahead: '+'
```

### Operadores Faltantes
```
3 4
Erro de sintaxe na linha 1, coluna 4: é esperado um operador antes de número ('4')
```

---


## Detalhes Técnicos

### Fluxo de Execução
1. **Inicialização**: `main()` configura `source` e chama `mybc()`
2. **Loop principal**: `mybc()` lê comandos separados por `;` ou `\n`
3. **Análise léxica**: `gettoken()` identifica próximo token
4. **Análise sintática**: `E()` avalia expressão recursivamente
5. **Avaliação**: Resultado armazenado em `acc` e impresso
6. **Recuperação**: Em caso de erro, `longjmp()` retorna ao loop

### Precedência de Operadores
| Precedência | Operadores | Associatividade |
|-------------|-----------|-----------------|
| 1 (maior)   | `*`, `/`  | Esquerda       |
| 2           | `+`, `-`  | Esquerda       |
| 3           | `-` (unário) | n/a     |
| 4           | `:=` (binário) | Direita     |

### Convenções de Comentários
- `/**/`: Marca ações semânticas (atribuições, operações na pilha)
- `//`: Comentários explicativos sobre lógica e estrutura

---

## 🔧 Estrutura de Arquivos

```
compiladores/
├── lexer.c          # Analisador léxico
├── lexer.h          # Interface do lexer
├── parser.c         # Analisador sintático
├── parser.h         # Interface do parser
├── main.c           # Ponto de entrada
├── main.h           # Cabeçalho principal
├── tokens.h         # Definições de tokens
├── Makefile         # Script de compilação
├── readme.md        # Este arquivo
└── versioned/       # Versões anteriores do código
```
