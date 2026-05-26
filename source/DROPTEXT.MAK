OPT        = 0
program    = DROPTEXT

.SUFFIXES: .c .opt .obj .ipf .inf .dll

#****************************************************************************
#* Abbreviations, set up for either debugging or optimisation               *
#****************************************************************************
!IF $(OPT)
OBJ      =     opt
LINKOPTS =     /EXEPACK /FARCALLTRANSLATION /ALIGN:16 /LI /NOP /NOL
!ELSE
OBJ      =     obj
LINKOPTS =     /CO /ALIGN:16 /LI /NOP /NOL
!ENDIF

OBJS = $(program).$(OBJ)

LIBS    = OS2386

ALL: $(program).EXE

{.}.c.obj:
   ICC.EXE /Ge+ /Gm+ /C+ /Kbcegpr /Ti+ /O- .\$*.c >$*.err 2>&1
   @attrib -a $@
   @attrib -a $*.err

{.}.c.opt:
   ICC.EXE /Ge+ /Gm+ /C+ /Kbcegpr /O+ /Fo$*.opt .\$*.c >$*.err 2>&1
   @attrib -a $@
   @attrib -a $*.err

{.}.rc.res:
   RC -r $<
   @attrib -a $@

droptext.$(OBJ): droptext.C \
 {.;$(INCLUDE);}droptext.h

$(program).RES:   $(program).RC   \
                  $(program).h    \
                  $(program).dlg  \
                  $(program).ICO  \
                  $(program).PTR

$(program).EXE:             \
             $(OBJS)        \
             $(program).RES
   @echo  $(OBJS)           > $(program).LNK
   @echo  $(program).EXE   >> $(program).LNK
   @echo  $(program).MAP   >> $(program).LNK
   @echo  $(LIBS)          >> $(program).LNK
   @echo  $(program).DEF   >> $(program).LNK
   LINK386.EXE $(LINKOPTS) @$(program).LNK
   RC $(program).RES $(program).EXE
   @attrib -a $@
   @attrib -a $*.map
   @erase $(program).LNK
