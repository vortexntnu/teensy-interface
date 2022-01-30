# Directories, include, lib,
!if defined(DEBUG_BUILD)
LIB_TYPE = Debug
EXECUTABLE = debug
BUILDDIR = debug
!else
LIB_TYPE = Release
EXECUTABLE = release
BUILDDIR = release
!endif

TDT4102_DIR = "%AppData%/tdt4102/tdt4102"
GRAPH_LIB_DIR = $(TDT4102_DIR)/Graph_lib
FLTK_LIB_DIR = $(TDT4102_DIR)/FLTK-1.4/lib/$(LIB_TYPE)

GRAPH_LIB_INCLUDE = $(GRAPH_LIB_DIR)/include
FLTK_INCLUDE = $(TDT4102_DIR)/FLTK-1.4

TDT4102_INCLUDE = $(TDT4102_DIR)/TDT4102

GRAPH_LIB_LIBRARIES = $(GRAPH_LIB_DIR)/lib/$(LIB_TYPE)/*.lib
FLTK_LIBRARIES = $(FLTK_LIB_DIR)/*.lib
WIN_LIBRARIES = wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib \
				winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib \
				oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

# Compiler and linker config
CXX = clang-cl
CXXFLAGS = /std:c++17 -TP -m64 -fdiagnostics-absolute-paths /W4 /GR \
			/DEBUG /INCREMENTAL /nologo /EHsc \
			-I$(GRAPH_LIB_INCLUDE) \
			-I$(FLTK_INCLUDE) \
			-I$(TDT4102_INCLUDE)
DEBUG = /Od /Ob0 -DWIN32 /MDd /Zi /RTC1
RELEASE = /O2 /Ob2 -DNDEBUG -DWIN32_LEAN_AND_MEAN /MD
MACROS = -D_WINDOWS -DNOGDI -D_CRT_SECURE_NO_WARNINGS
DEBUGLD = /DEBUG /INCREMENTAL /Zi
RELEASELD = /INCREMENTAL:NO
LDFLAGS = -m64 $(GRAPH_LIB_LIBRARIES) $(FLTK_LIBRARIES) $(WIN_LIBRARIES)
LINK = $(CXX)

!if defined(DEBUG_BUILD)
CXXFLAGS = $(CXXFLAGS) $(DEBUG)
LDFLAGS = $(LDFLAGS) $(DEBUGLD)
!else
CXXFLAGS = $(CXXFLAGS) $(RELEASE)
LDFLAGS = $(LDFLAGS) $(RELEASELD)
!endif

!ifndef EXECUTABLE
EXECUTABLE = program
!endif

SOURCES = *.cpp

SOURCES_BUILDDIR = $(BUILDDIR)\\sources
TDT4102_BUILDDIR = $(BUILDDIR)\\tdt4102

SOURCES_OBJECTS = $(SOURCES_BUILDDIR)/$(SOURCES:.cpp=.obj)
TDT4102_OBJECTS = $(TDT4102_BUILDDIR)/AnimationWindow.obj \
				  $(TDT4102_BUILDDIR)/InputHandler.obj \
				  $(TDT4102_BUILDDIR)/MoveableImage.obj \
				  $(TDT4102_BUILDDIR)/ShapeDrawQueue.obj

# Inference rules, etc.
.SUFFIXES: .obj .cpp

{.}.cpp{$(SOURCES_BUILDDIR)}.obj:
	if not exist $(SOURCES_BUILDDIR) mkdir $(SOURCES_BUILDDIR)
	$(CXX) $(CXXFLAGS) /c $< /Fo$(@D)/

$(TDT4102_BUILDDIR):
	if not exist $(TDT4102_BUILDDIR) mkdir $(TDT4102_BUILDDIR)

$(TDT4102_BUILDDIR)/AnimationWindow.obj: $(TDT4102_BUILDDIR)
	$(CXX) $(CXXFLAGS) /c $(TDT4102_INCLUDE)/AnimationWindow.cpp /Fo$(@D)/

$(TDT4102_BUILDDIR)/InputHandler.obj: $(TDT4102_BUILDDIR)
	$(CXX) $(CXXFLAGS) /c $(TDT4102_INCLUDE)/InputHandler.cpp /Fo$(@D)/

$(TDT4102_BUILDDIR)/MoveableImage.obj: $(TDT4102_BUILDDIR)
	$(CXX) $(CXXFLAGS) /c $(TDT4102_INCLUDE)/MoveableImage.cpp /Fo$(@D)/

$(TDT4102_BUILDDIR)/ShapeDrawQueue.obj: $(TDT4102_BUILDDIR)
	$(CXX) $(CXXFLAGS) /c $(TDT4102_INCLUDE)/ShapeDrawQueue.cpp /Fo$(@D)/

$(BUILDDIR)/$(EXECUTABLE).exe: $(SOURCES_OBJECTS) $(TDT4102_OBJECTS)
	$(LINK) $(LDFLAGS) $(SOURCES_OBJECTS) $(TDT4102_OBJECTS) /Fe$@
	@echo ...
	@echo Successfully built ./$@

$(EXECUTABLE).exe: $(BUILDDIR)/$(EXECUTABLE).exe

# Catch-all, but can interfere with student's own files in build directory
#rmdir /s /q $(BUILDDIR)
clean:
	@echo Cleaning the output files in $(BUILDDIR)
	-del $(BUILDDIR)\$(EXECUTABLE).exe 2> NUL
	-del $(BUILDDIR)\$(EXECUTABLE).pdb 2> NUL
	-del $(BUILDDIR)\$(EXECUTABLE).ilk 2> NUL
	-del $(SOURCES_BUILDDIR)\*.obj 2> NUL
	if exist $(SOURCES_BUILDDIR) rmdir $(SOURCES_BUILDDIR)
cleanall: cleandebug
	-del $(TDT4102_BUILDDIR)\*.obj 2> NUL
	if exist $(TDT4102_BUILDDIR) rmdir $(TDT4102_BUILDDIR)
