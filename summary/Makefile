# Makefile for Summary
# Victor Manuel Fernandez Castro
# 3 April 2016

LATEX	= pdflatex -interaction nonstopmode
RMFILE	= rm -f

TARGET = summary

.PHONY: all clean

all: $(TARGET).pdf

clean:
	$(RM) *.aux *.dvi *.log *.pdf *.synctex.gz *.loc *.toc *.lof *.bbl *.blg \
	      *.nav *.out *.snm *.bak

%.pdf: %.tex images/*
	$(LATEX) $<
