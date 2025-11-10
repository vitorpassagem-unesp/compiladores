CFLAGS = -g -I$(MYINCLUDE)

MYINCLUDE = .

reloc-list = lexer.o parser.o main.o

#executa o projeto
mybc: $(reloc-list)
	$(CC) -o $@ $^

#limpa os arquivos .o
clean:
	$(RM) $(reloc-list)

#limpa os arquivos .o e os arquivos temporários
mostlyclean: clean
	$(RM) *~

#empacota o projeto em um .targz
targz:
	/usr/bin/tar zcvf mybc_grp02.tar.gz *.[ch] Makefile
