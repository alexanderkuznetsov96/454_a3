PROG = rt

OBJS =	main.o arcballWindow.o scene.o sphere.o triangle.o light.o eye.o object.o \
	material.o texture.o vertex.o wavefrontobj.o wavefront.o linalg.o \
	gpuProgram.o axes.o arrow.o glad/src/glad.o # font.o

#LDFLAGS  = -Llib32 -lglfw -lGL -lpng12 -ldl -lfreetype
#CXXFLAGS = -g -I/usr/include/freetype2 -Wall -Wno-write-strings -Wno-parentheses -Wno-unused-variable -DLINUX -DUSE_FREETYPE

LDFLAGS  = -Llib32 -lglfw -lGL -lpng12 -ldl
CXXFLAGS = -g -Wall -Wno-write-strings -Wno-parentheses -Wno-unused-variable -DLINUX

CXX      = g++

$(PROG):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) *~ core rt Makefile.bak

depend:	
	makedepend -Y *.h *.cpp

# DO NOT DELETE

arcballWindow.o: headers.h glad/include/glad/glad.h
arcballWindow.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
arrow.o: object.h linalg.h material.h texture.h headers.h
arrow.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
arrow.o: include/GLFW/glfw3.h seq.h gpuProgram.h
axes.o: linalg.h gpuProgram.h headers.h glad/include/glad/glad.h
axes.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h
eye.o: linalg.h
gpuProgram.o: headers.h glad/include/glad/glad.h
gpuProgram.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
headers.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
headers.o: include/GLFW/glfw3.h linalg.h
light.o: linalg.h sphere.h object.h material.h texture.h headers.h
light.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
light.o: include/GLFW/glfw3.h seq.h gpuProgram.h
main.o: seq.h scene.h linalg.h object.h material.h texture.h headers.h
main.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
main.o: include/GLFW/glfw3.h gpuProgram.h light.h sphere.h eye.h axes.h
main.o: arrow.h rtWindow.h arcballWindow.h
material.o: linalg.h texture.h headers.h glad/include/glad/glad.h
material.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h seq.h
material.o: gpuProgram.h
object.o: linalg.h material.h texture.h headers.h glad/include/glad/glad.h
object.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h seq.h
object.o: gpuProgram.h
rtWindow.o: arcballWindow.h headers.h glad/include/glad/glad.h
rtWindow.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
rtWindow.o: scene.h seq.h object.h material.h texture.h gpuProgram.h light.h
rtWindow.o: sphere.h eye.h axes.h arrow.h
scene.o: seq.h linalg.h object.h material.h texture.h headers.h
scene.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
scene.o: include/GLFW/glfw3.h gpuProgram.h light.h sphere.h eye.h axes.h
scene.o: arrow.h
sphere.o: object.h linalg.h material.h texture.h headers.h
sphere.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
sphere.o: include/GLFW/glfw3.h seq.h gpuProgram.h sphere.h
texture.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
texture.o: include/GLFW/glfw3.h linalg.h seq.h
triangle.o: object.h linalg.h material.h texture.h headers.h
triangle.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
triangle.o: include/GLFW/glfw3.h seq.h gpuProgram.h vertex.h
vertex.o: linalg.h
wavefront.o: headers.h glad/include/glad/glad.h
wavefront.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefront.o: seq.h gpuProgram.h
wavefrontobj.o: object.h linalg.h material.h texture.h headers.h
wavefrontobj.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
wavefrontobj.o: include/GLFW/glfw3.h seq.h gpuProgram.h wavefront.h
arcballWindow.o: arcballWindow.h headers.h glad/include/glad/glad.h
arcballWindow.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
arrow.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
arrow.o: include/GLFW/glfw3.h linalg.h arrow.h object.h material.h texture.h
arrow.o: seq.h gpuProgram.h
axes.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
axes.o: include/GLFW/glfw3.h linalg.h axes.h gpuProgram.h
eye.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
eye.o: include/GLFW/glfw3.h linalg.h eye.h main.h seq.h scene.h object.h
eye.o: material.h texture.h gpuProgram.h light.h sphere.h axes.h arrow.h
eye.o: rtWindow.h arcballWindow.h
font.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
font.o: include/GLFW/glfw3.h linalg.h gpuProgram.h
gpuProgram.o: gpuProgram.h headers.h glad/include/glad/glad.h
gpuProgram.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
light.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
light.o: include/GLFW/glfw3.h linalg.h light.h sphere.h object.h material.h
light.o: texture.h seq.h gpuProgram.h main.h scene.h eye.h axes.h arrow.h
light.o: rtWindow.h arcballWindow.h
linalg.o: linalg.h
main.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
main.o: include/GLFW/glfw3.h linalg.h rtWindow.h arcballWindow.h scene.h
main.o: seq.h object.h material.h texture.h gpuProgram.h light.h sphere.h
main.o: eye.h axes.h arrow.h font.h
material.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
material.o: include/GLFW/glfw3.h linalg.h material.h texture.h seq.h
material.o: gpuProgram.h main.h scene.h object.h light.h sphere.h eye.h
material.o: axes.h arrow.h rtWindow.h arcballWindow.h
object.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
object.o: include/GLFW/glfw3.h linalg.h object.h material.h texture.h seq.h
object.o: gpuProgram.h main.h scene.h light.h sphere.h eye.h axes.h arrow.h
object.o: rtWindow.h arcballWindow.h
scene.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
scene.o: include/GLFW/glfw3.h linalg.h scene.h seq.h object.h material.h
scene.o: texture.h gpuProgram.h light.h sphere.h eye.h axes.h arrow.h
scene.o: rtWindow.h arcballWindow.h triangle.h vertex.h wavefrontobj.h
scene.o: wavefront.h font.h main.h
sphere.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
sphere.o: include/GLFW/glfw3.h linalg.h sphere.h object.h material.h
sphere.o: texture.h seq.h gpuProgram.h main.h scene.h light.h eye.h axes.h
sphere.o: arrow.h rtWindow.h arcballWindow.h
texture.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
texture.o: include/GLFW/glfw3.h linalg.h texture.h seq.h
triangle.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
triangle.o: include/GLFW/glfw3.h linalg.h triangle.h object.h material.h
triangle.o: texture.h seq.h gpuProgram.h vertex.h main.h scene.h light.h
triangle.o: sphere.h eye.h axes.h arrow.h rtWindow.h arcballWindow.h
vertex.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
vertex.o: include/GLFW/glfw3.h linalg.h vertex.h main.h seq.h scene.h
vertex.o: object.h material.h texture.h gpuProgram.h light.h sphere.h eye.h
vertex.o: axes.h arrow.h rtWindow.h arcballWindow.h
wavefront.o: headers.h glad/include/glad/glad.h
wavefront.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefront.o: gpuProgram.h wavefront.h seq.h
wavefrontobj.o: headers.h glad/include/glad/glad.h
wavefrontobj.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefrontobj.o: wavefrontobj.h object.h material.h texture.h seq.h
wavefrontobj.o: gpuProgram.h wavefront.h
