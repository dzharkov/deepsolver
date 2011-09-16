
NAME =tech-assignment

all: $(NAME).pdf

clean:
	/bin/rm -rf *.log *.aux *.toc  *.dvi *.ps *.pdf

$(NAME).pdf: *.tex
	/bin/rm -rf *.log *.aux *.toc  *.dvi *.ps *.pdf
	latex $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf $(NAME).ps $(NAME).pdf
	/bin/rm -rf *.log *.aux *.toc  *.ps
