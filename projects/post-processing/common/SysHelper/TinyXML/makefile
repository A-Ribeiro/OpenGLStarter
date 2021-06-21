#
#  Makefile to automatic fetch dependeces and compile sources
#
# Autor: Alessandro Ribeiro da Silva
# email: alessandro.ribeiro.silva@gmail.com
#
#  Obs.: todos os arquivos fontes devem estar com a extensao .cpp
#

##########################
#    Binary definition
######################################################
BIN=main
PROJECT=OpenAL compilation test
ifndef SO
  SO=LINUX
endif
##########################
# .cpp sources
######################################################
ALLOBJ=xmltest.o 

##########################
#    Makefile automatic compilation
######################################################
DEFS=
LINKER=
ifndef SO
  ifeq ($(findstring all,$(MAKECMDGOALS)),all)
    $(error Invalid SO '$(SO)'. You must to choose the LINUX or WIN32 platform in makefile.)
  endif
else
  ifeq ($(SO),LINUX)
    DEFS+=-DLINUX
    LINKER+=./Release/libTinyXML.a
  else
    ifeq ($(SO),WIN32)
      DEFS+=-DWIN32
      LINKER+=./Release/libTinyXML.a
    else
      ifeq ($(findstring all,$(MAKECMDGOALS)),all)
        $(error Invalid SO '$(SO)'. You must to choose the LINUX or WIN32 platform in makefile.)
      endif
    endif
  endif
endif

CC=g++
FLAGS=-O2 -pipe -march=athlon64 -m3dnow -mmmx -msse -msse2 -msse3 -Wall
CFLAGS=$(FLAGS) $(DEFS)
#-lX11

#diz  quais entradas nao devem ser verificadas como arquivos para atualizacao
.PHONY : help execute all clean

help:
	@echo " "
	@echo "Makefile to automatic fetch dependeces and compile sources"
	@echo " "
	@echo "        Author: Alessandro Ribeiro da Silva"
	@echo "         email: alessandro.ribeiro.silva@gmail.com"
	@echo " "
	@echo ' Project: "$(PROJECT)"'
	@echo ' Binary Target: "$(BIN)"'
	@echo " "
	@echo " usage: make SO=[LINUX|WIN32] [all|clean|help|execute]"
	@echo " "

execute:
	./$(BIN)

clean:
	@echo clean all
	@rm -rf $(BIN) $(ALLOBJ) $(ALLOBJ:.o=.d)

all: $(BIN)

$(BIN): $(ALLOBJ)
	@echo " Linking objects"
	@$(CC) $(CFLAGS) -o $(BIN) $(ALLOBJ) $(LINKER)
	@echo " Finished!!!"

#fazer todos os arquivos .o especificados compilarem a partir de um .cpp no mesmo diretorio
#    $@ -> target
#    $? -> entradas (todas)
#    $< -> somente a primeira entrada
#    $* -> o nome da dependencia sem extensao
#.cpp.o :
%.o : %.cpp
	@echo " compiling obj:" $@
	@$(CC) $(CFLAGS) -c -o $@ $<

%.d : %.cpp
	@echo " build dependency:" $@
	@$(CC) -MM -MT '$@ $*.o' -MF '$@' $(CFLAGS) $<

#verificar se o cara ta executando o 'all' sozinho
ifeq ($(MAKECMDGOALS),all)
  #sinclude << tem o mesmo efeito
  -include $(ALLOBJ:.o=.d)
else
  ifeq ($(findstring all,$(MAKECMDGOALS)),all)
    $(error You must execute make with option 'all' alone)
  endif
endif

